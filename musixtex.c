#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "musixtex.h"
#include "dlog.h"

#define PATH_MAX 4096
#define TIMEOUT_MS 3

#define CMD_MUSIXTEX "musixtex"
#define CMD_TIMEOUT "timeout"


bool command_exists(const char *cmd)
{
    bool ret = false;
    char fpath[PATH_MAX];
    char *paths = NULL;
    char *tmp = getenv("PATH");
    ERR_RETn(!tmp);

    paths = malloc(strlen(tmp) + 1);
    ERR_RETn(!paths);
    strcpy(paths, tmp);

    char *dir = strtok(paths, ":");

    while (dir != NULL)
    {
        snprintf(fpath, PATH_MAX, "%s/%s", dir, cmd);
        if (access(fpath, X_OK) == 0)
        {
            ret = true;
            goto error_return;
        }
        dir = strtok(NULL, ":");
    }

error_return:
    if (paths)
        free(paths);
    return ret;
}

bool is_installed_musixtex(void)
{
    return command_exists(CMD_TIMEOUT) && command_exists(CMD_MUSIXTEX);
}

static char *to_tex(const char *xml)
{
    return NULL;
}

static int tex2pdf(const char *tex, const char *pdf)
{
    int ret = -1;
    char cmd[PATH_MAX];

    snprintf(cmd, PATH_MAX, "timeout %ds musixtex %s", TIMEOUT_MS, tex);

    FILE *fp = popen(cmd, "r");
    ERR_RETn(!fp);

    int status = pclose(fp);
    ERR_RETn(!status);

    ret = 0;

error_return:
    return ret;
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