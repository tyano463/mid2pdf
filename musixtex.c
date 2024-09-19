#include <stdio.h>
#include "musixtex.h"
#include "dlog.h"

bool is_installed_musixtex(void)
{
    return true;
}

static char *to_tex(const char *xml)
{
    return NULL;
}

static int tex2pdf(const char *tex, const char *pdf)
{
    return 1;
}

int to_pdf(const char *xml, const char *pdf)
{
    int ret = -1;

    char *tex = to_tex(xml);
    ERR_RET(!tex, "convert xml 2 tex failed.");

    ret = tex2pdf(tex, pdf);

error_return:
    return ret;
}