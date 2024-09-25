#ifndef __MUSIXTEX_H__
#define __MUSIXTEX_H__

#include <stdbool.h>

bool is_installed_musixtex(void);
int to_pdf(const mdata_t *, const char *pdf);
#endif