
#ifndef __MUSICXML_H__
#define __MUSICXML_H__

#include <stdint.h>

typedef struct str_m2x_note
{
    uint32_t pos;
    uint16_t duration;
    uint8_t pitch;
    uint8_t velocity;
} m2x_note_t;

typedef struct str_m2x_track
{
    m2x_note_t *note;
    uint32_t note_num;
} m2x_track_t;

typedef struct str_m2x
{
    m2x_track_t *track;
    uint16_t track_num;
    uint16_t resolution;
} m2x_t;
char *to_musicxml(const char *);

#endif