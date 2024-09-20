#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include "misc.h"
#include "dlog.h"

bool file_exists(const char *path)
{
    int status;
    bool ret = false;
    struct stat st_buf;

    ERR_RETn(!path);

    status = stat(path, &st_buf);
    ERR_RETn(status);

    ret = S_ISREG(st_buf.st_mode);

error_return:
    return ret;
}

char *get_pdf_path(char *pdf_path, char *path)
{
    int cnt = 1;
    char *ret = NULL;

    sprintf(pdf_path, "%s", path);
    char *p = strrchr(pdf_path, '.');
    ERR_RETn(!p);

    sprintf(p, ".pdf");
    while (file_exists(pdf_path))
    {
        sprintf(p, "_%d.pdf", cnt++);
    }
    ret = pdf_path;

error_return:
    return ret;
}

static bool hasext(const char *path, const char *expect)
{

    bool ret = false;

    ERR_RETn(!path);
    const char *ext = strrchr(path, '.');
    ret = ((ext != NULL) && (strcasecmp(ext, expect) == 0));

error_return:
    return ret;
}

bool is_midi(const char *path)
{
    return hasext(path, ".mid");
}

bool is_mxl(const char *path)
{
    return hasext(path, ".mxl");
}

int midival(const uint8_t *b)
{
    int ret = 0;
    for (int i = 0; i < 4; i++)
    {
        ret += (b[i] & 0x7f);
        if (b[i] & 0x80)
            ret *= 0x80;
        else
            break;
    }
    return ret;
}
