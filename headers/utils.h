#ifndef UTILS_H
#define UTILS_H

int string_ends_with(const char * str, const char * suffix);

int number_of_files_in_directory(const char * directory_path);

int all_files_are_bmp(char *directory_path);

int directory_exists(char *directory_name);

#endif //UTILS_H
