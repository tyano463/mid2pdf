
#ifndef __MUSICXML_H__
#define __MUSICXML_H__

#include <stdint.h>

typedef struct str_smf_ext
{
    smf_t *smf;
    uint16_t track_num;
    uint16_t version;
    uint16_t resolution;
} smf_ext_t;
char *to_musicxml(const char *);

#endif