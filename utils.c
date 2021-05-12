#define _DEFAULT_SOURCE
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/utils.h"

int string_ends_with(const char * str, const char * suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return
            (str_len >= suffix_len) &&
            (0 == strcmp(str + (str_len-suffix_len), suffix));
}

int number_of_files_in_directory(const char * directory_path) {
    int file_count = 0;
    struct dirent * entry;

    DIR * dirp = opendir(directory_path);
    if(dirp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            file_count++;
        }
    }

    closedir(dirp);
    return file_count;
}

int all_files_are_bmp(char *directory_path) {
    struct dirent * entry;

    DIR * dirp = opendir(directory_path);
    if(dirp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dirp)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        if(!string_ends_with(entry->d_name, ".bmp")) {
            return 0; // Failure
        }
    }
    return 1; // Success
}
