#include <stdio.h>    // I/O functions
#include <string.h>   // String functions
#include <stdlib.h>   // Memory, exit, qsort
#include <unistd.h>   // POSIX API, getopt
#include <sys/stat.h> // File status
#include <dirent.h>   // Directory traversal
#include <pwd.h>      // User info
#include <grp.h>      // Group info
#include <time.h>     // Time functions
#include <unistd.h>   // POSIX API

// Minimal direct struct
struct dentry {
    char *dname;    // file name
    int dlen;       // length
    off_t fsize;    // file size
    char *owner;    // owner name
    char *group;    // group name
    char *lastmod;  // last modification time
    char chcode;    // file type code
};

char file_type_chcode(const struct stat *st) {
    if (S_ISDIR(st->st_mode)) {
        return '/';
    }
    if (S_ISLNK(st->st_mode)) {
        return '@';
    }
    if (S_ISFIFO(st->st_mode)) {
        return '|';
    }
    if ((S_ISREG(st->st_mode)) && (st->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        return '*';
    }
    return ' ';
}

struct dentry *readDirectory(const char *path, int *num) {
    DIR *dp;             // directory stream
    struct dirent *de;   // directory entry
    int dcount = 0;      // entry count
    struct dentry *data = NULL; // array of entries
    int rcount = 0;      // entries stored
    struct stat st;      // file status
    struct passwd *pwd;  // user info
    struct group *grp;   // group info

    dp = opendir(path);
    if (dp == NULL) {
        return NULL;
    }

    while ((de = readdir(dp)) != NULL) dcount++; // count entries
    rewinddir(dp); // reset stream

    data = (struct dentry *)malloc(dcount * sizeof(struct dentry)); // allocate memory

    while ((de = readdir(dp)) != NULL) {
        if (lstat(de->d_name, &st) == -1) {
            continue;
        }
        data[rcount].dname = strdup(de->d_name); // copy name
        data[rcount].dlen = de->d_namlen;        // save name length
        data[rcount].fsize = st.st_size;
        pwd = getpwuid(st.st_uid);
        grp = getgrgid(st.st_gid);
        data[rcount].owner = strdup(pwd->pw_name);
        data[rcount].group = strdup(grp->gr_name);
        data[rcount].lastmod = strdup(ctime(&st.st_mtime));
        data[rcount].chcode = file_type_chcode(&st);

        rcount++; // increment stored count
    }
    closedir(dp); // close directory

    *num = rcount; // store total count
    return data; // return array
}

int cmp_dname(const void *a, const void *b) {
    const struct dentry *x = (const struct dentry *)a;
    const struct dentry *y = (const struct dentry *)b;
    return strcmp(x->dname, y->dname);  // alphabetic order
}

int main(int argc, char *argv[]) {
    int size;           // num of entries
    struct dentry *data = NULL;
    int i;

    int show_all = 0;   // -a flag
    int show_codes = 0; // -F flag
    int sort_names = 0; // -s flag

    int ch;
    while ((ch = getopt(argc, argv, "aFs")) != -1) {
        if (ch == 'a') show_all = 1;
        else if (ch == 'F') show_codes = 1;
        else if (ch == 's') sort_names = 1;
        else {
            fprintf(stderr, "USAGE: %s [-aFs] <directory_path>\n", argv[0]);
            return -1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "USAGE: %s [-aFs] <directory_path>\n", argv[0]);
        return -1;
    }
    const char *path = argv[optind];

    printf("Directory listing for [%s]:\n", argv[optind]);
    data = readDirectory(path, &size);

    if (!data) {
        fprintf(stderr, "ERROR: failed to open directory '%s': ", path);
        perror(NULL);
        return -1;
    }

    if (sort_names && size > 1) {
        qsort(data, size, sizeof(*data), cmp_dname); // sort data
    }

    for (i = 0; i < size; i++) {
        if (!show_all && data[i].dname[0] == '.') continue;  // -a check

        if (show_codes)
            printf("%-20s %c %5lld bytes %-5s %-5s %s",
                   data[i].dname, data[i].chcode, (long long)data[i].fsize,
                   data[i].owner, data[i].group, data[i].lastmod);
        else
            printf("%-20s   %5lld bytes %-5s %-5s %s",
                   data[i].dname, (long long)data[i].fsize,
                   data[i].owner, data[i].group, data[i].lastmod);
    }

    for (i = 0; i < size; i++) {
        free(data[i].dname);
        free(data[i].owner);
        free(data[i].group);
        free(data[i].lastmod);
    }
    free(data); // free array

    return 0;
}
