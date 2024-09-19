#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "musicxml.h"
#include "musixtex.h"
#include "misc.h"
#include "dlog.h"

#define PATH_MAX 4096

static char *pdf_path;
static char *midi_path;

void usage(void)
{
    char buf[PATH_MAX];

    FILE *fp = fopen("/proc/self/comm", "r");
    ERR_RETn(!fp);

    char *p = fgets(buf, PATH_MAX, fp);
    ERR_RETn(!p);

    buf[strcspn(buf, "\n")] = '\0';
    fclose(fp);

    printf("usage:\n\t%s [midifile] [pdffile]\n", buf);

error_return:
    return;
}

bool env_check(int argc, char *argv[])
{
    bool ret = false;

    dlog("arg:%d", argc);
    ERR_RETn(argc < 2);

    ERR_RETn(!file_exists(argv[1]));
    ERR_RETn(!is_midi(argv[1]));

    midi_path = argv[1];

    if (argc >= 3)
    {
        ERR_RETn(file_exists(argv[2]));
    }
    else
    {
        pdf_path = malloc(PATH_MAX);
        pdf_path = get_pdf_path(pdf_path, argv[1]);
    }

    ret = true;

error_return:
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = -1;
    if (!env_check(argc, argv))
    {
        usage();
        goto error_return;
    }

    char *xml = to_musicxml(midi_path);
    ERR_RET(!xml, "convert to musicxml failed.");

    ret = to_pdf(xml, pdf_path);
    ERR_RET(ret, "convert to pdf failed.");

    ret = 0;

error_return:
    return ret;
}
