
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

static void list_directory(const char *path) {
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("[error] opendir()");
        return;
    }

    struct dirent *entry;
    char fullpath[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        int written = snprintf(
            fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name
        );

        if (written < 0 || (size_t)written >= sizeof(fullpath)) {
            fprintf(stderr, "[error] Path too long: %s\n",
                    entry->d_name);
            continue;
        }

        struct stat info;

        if (stat(fullpath, &info) == -1) {
            fprintf(stderr, "[warning] Cannot inspect %s: %s\n",
                    entry->d_name, strerror(errno));
            continue;
        }

        struct tm *tm_info = localtime(&info.st_mtime);
        char date[16] = "??-??-????";
        char clock[8] = "??:??";

        if (tm_info != NULL) {
            strftime(date, sizeof(date), "%d-%m-%Y", tm_info);
            strftime(clock, sizeof(clock), "%H:%M", tm_info);
        }

        printf("%-10s %-5s ", date, clock);

        if (S_ISDIR(info.st_mode)) {
            printf("%8s  %s\n", "<DIR>", entry->d_name);
        } else {
            printf("%8lld  %s\n",
                   (long long)info.st_size, entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    const char *directory;

    if (argc > 1) {
        directory = argv[1];
    } else {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("[error] getcwd()");
            return EXIT_FAILURE;
        }

        directory = cwd;
    }

    printf("Directory of %s\n\n", directory);
    list_directory(directory);

    return EXIT_SUCCESS;
}
