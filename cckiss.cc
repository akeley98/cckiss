#include <assert.h>
#include <errno.h>
#include <fstream>
#include <stdio.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace {

// Arguably evil, but allow timespecs to be compared like ordinary
// darn numbers.
inline bool operator< (struct timespec left, struct timespec right) {
    return left.tv_sec < right.tv_sec ||
        (left.tv_sec == right.tv_sec && left.tv_nsec < right.tv_nsec);
}
inline bool operator> (struct timespec left, struct timespec right) {
    return right < left;
}
inline bool operator<= (struct timespec left, struct timespec right) {
    return !(left > right);
}
inline bool operator>= (struct timespec left, struct timespec right) {
    return !(left < right);
}

// If str starts with the string pointed to by prefix, return a pointer
// to the remaining part of str. Otherwise return null.
const char* skip_prefix(const char* str, const char* prefix) {
    while (*prefix != 0) {
        if (*str++ != *prefix++) return 0;
    }
    return str;
}

// For the named file, return
//
// 1. Whether the file exists, and
// 2. If so, the modification time, through *timespec.
//
// Terminates if the file exists but could not be accessed.
bool file_exists_mtim(const std::string& name, struct timespec *timespec)
{
    struct stat statbuf;
    int return_code = 1;
    constexpr int max_tries = 10000;
    for (int tries = 0; return_code != 0; ++tries) {
        return_code = stat(name.c_str(), &statbuf);
        if (return_code == 0) break;

        // File doesn't exist:
        if (errno == ENOENT) return false;

        // Unix stupidity:
        if (errno == EINTR && tries < max_tries) continue;

        printf("Could not access \"%s\": %s\n", name.c_str(), strerror(errno));
        exit(1);
    }
    *timespec = statbuf.st_mtim;
    return true;
}

// Read the dependency file correspoding to the named source file, and
// return a vector of dependency file names within (newline separated
// in deps file).  If the dependency file doesn't yet exist, the
// dependency file itself is returned as the only dependency (bit of a
// hack). If the dependency file cannot be read for any other reason,
// terminate.
std::vector<std::string> get_deps_for_source(
    const std::string& source_file_name)
{
    std::vector<std::string> deps;
    std::string deps_file_name = "cckiss/deps/" + source_file_name + ".txt";

    std::ifstream stream(deps_file_name);
    if (!stream.is_open()) {
        struct stat statbuf;
        int return_code = stat(deps_file_name.c_str(), &statbuf);

        // Check "impossible" error: fstream can't open the file but
        // stat can (race condition possible I suppose).
        if (return_code == 0) {
            printf(
                "Could not read \"%s\" for unknown reason.\n",
                source_file_name.c_str());
            fprintf(
                stderr,
                "Could not read \"%s\" for unknown reason.\n",
                source_file_name.c_str());
            exit(1);
        }

        // Failed to open deps file because it doesn't exist: okay,
        // just return the deps file itself as the missing dependency.
        if (errno == ENOENT) {
            deps.push_back(std::move(deps_file_name));
            return deps;
        }

        // Otherwise, something else isn't right and needs to be reported.
        const char* reason = strerror(errno);
        if (return_code == 0) {
            printf(
                "Could not read \"%s\": %s\n",
                source_file_name.c_str(),
                reason);
            fprintf(
                stderr,
                "Could not read \"%s\": %s\n",
                source_file_name.c_str(),
                reason);
            exit(1);
        }
    }

    std::string dep_file_name;

    do {
        std::getline(stream, dep_file_name, '\n');
        if (stream.eof()) return deps;
        if (stream.bad()) {
            fprintf(
                stderr,
                "Internal error: ifstream badbit set for \"%s\".\n",
                deps_file_name.c_str());
        }
        if (stream.fail()) {
            fprintf(
                stderr,
                "Internal error: ifstream failbit set for \"%s\".\n",
                deps_file_name.c_str());
        }
        deps.push_back(std::move(dep_file_name));
    } while (1);
}

// Given the name of a target file (compiled object/assembly file in
// cckiss/asm/), return true if we should recompile it. This is the
// case if any of the following hold:
//
// 1. Target file doesn't yet exist.
//
// 2. The target's corresponding source file was modified.
//
// 3. One of the dependencies listed in the source file's
//    corresponding dependency file was modified.
//
// 4. Said dependency file does not exist.
bool should_recompile_named_file(const std::string& compiled_file_name)
{
    // These vars are all up here because of the goto (maybe that should be
    // a helper function instead or something...)
    struct timespec target_mtim;
    struct stat statbuf;
    size_t sz = 0;
    int return_code = 0;
    bool compiled_file_exists = false;

    // Need to recover the name of the source file for the given
    // target (compiled) file. Expect compiled_file_name =
    // "cckiss/asm/{source/file}.[s|o]"
    std::string source_file_name;
    const char* skipped_prefix = skip_prefix(
        compiled_file_name.c_str(),
        "cckiss/asm/");
    if (!skipped_prefix) goto bad_file_name;

    // Now source_file_name = {source/file}.[s|o] Remove the .s or .o
    source_file_name = std::string(skipped_prefix);
    sz = source_file_name.size();
    if (sz <= 2) goto bad_file_name;
    if (source_file_name[sz-1] != 's' && source_file_name[sz-1] != 'o') {
        goto bad_file_name;
    }
    if (source_file_name[sz-2] != '.') goto bad_file_name;
    source_file_name.pop_back();
    source_file_name.pop_back();

    // Now we extracted the source file name from the target file
    // name, and we just have to see if either the source file or any
    // of its dependency files changed after the last compilation.
    compiled_file_exists =
        file_exists_mtim(compiled_file_name, &target_mtim);

    // If the target file doesn't exist yet, we obviously must compile.
    if (!compiled_file_exists) {
        printf("New target file \"%s\".\n", compiled_file_name.c_str());
        return true;
    }
    else {
        // Otherwise, compare the dependency modification times with the
        // target file modification time.

        // Source file (required).
        struct timespec dep_mtim;
        bool source_exists = file_exists_mtim(source_file_name, &dep_mtim);
        if (!source_exists) {
            printf(
                "Missing needed source file \"%s\".\n",
                source_file_name.c_str());
            fprintf(
                stderr,
                "Missing needed source file \"%s\".\n",
                source_file_name.c_str());
            exit(1);
        }
        if (dep_mtim >= target_mtim) {
            printf(
                "\"%s\" modified, needed by \"%s\".\n",
                source_file_name.c_str(),
                compiled_file_name.c_str());
            return true;
        }

        // Check dependency files (possibly including the dependency
        // list file generated by cckiss).
        auto deps_file_names = get_deps_for_source(source_file_name);
        for (const std::string& dep_file_name : deps_file_names) {
            bool dep_exists = file_exists_mtim(dep_file_name, &dep_mtim);
            if (!dep_exists) {
                printf("Missing \"%s\", originally needed by \"%s\".\n",
                    dep_file_name.c_str(),
                    compiled_file_name.c_str());
                return true;
            }

            if (dep_mtim >= target_mtim) {
                printf("Dependency \"%s\" modified; needed by \"%s\".\n",
                    dep_file_name.c_str(),
                    compiled_file_name.c_str());
                return true;
            }
        }

        // No dependency changes detected at this point.
        return false;
    }

bad_file_name:
    printf(
        "Expected target (compiled) file name to be of the form\n"
        "cckiss/asm/{path/to/source/file}.[s|o]. Got:\n\"%s\".",
        compiled_file_name.c_str());
    fprintf(
        stderr,
        "Expected target (compiled) file name to be of the form\n"
        "cckiss/asm/{path/to/source/file}.[s|o]. Got:\n\"%s\".",
        compiled_file_name.c_str());
    exit(1);
}

int cckiss_main(std::vector<std::string> args)
{
    bool b = should_recompile_named_file(args[1]);
    printf("%s\n", b ? "yes" : "no");

    return 0;
}

} // end anonymous namespace

int main(int argc, char** argv)
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    return cckiss_main(std::move(args));
}
