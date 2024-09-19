#ifndef __MISC_H__
#define __MISC_H__

#include <stdbool.h>

bool file_exists(const char *);
char *get_pdf_path(char *pdf_path, char *path);
bool is_midi(const char *path);

#endif