#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // Open the directory
    if ((fd = open(path, 0)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    // Get status of the path
    if (fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Check if it's a directory
    if (st.type != T_DIR) {
        // If it's a file, compare with target
        if (st.type == T_FILE) {
            // Extract filename from path (last component after '/')
            p = path + strlen(path) - 1;
            while (p >= path && *p != '/')
                p--;
            p++; // Move past the '/'
            if (strcmp(p, target) == 0) {
                printf("%s\n", path);
            }
        }
        close(fd);
        return;
    }

    // Ensure path fits in buffer with room for subpaths
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    // Copy path to buffer and prepare to append
    strcpy(buf, path);
    p = buf + strlen(buf);
    if (*(p - 1) != '/') {
        *p++ = '/';
    }

    // Read directory entries
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) // Skip empty entries
            continue;

        // Skip "." and ".."
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // Construct full path for this entry
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; // Ensure null-terminated

        // Recursively search this entry
        find(buf, target);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: find <path> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}