
#ifndef __MIDIDATA_H__
#define __MIDIDATA_H__

#include <stdint.h>

#define NOTE_ON 0x90
#define NOTE_OFF 0x80

#define MAX_NOTE_NUM 8

enum
{
    META_SEQ_NO,
    META_TEXT_EVENT,
    META_COPY_RIGHT,
    META_TRACK_NAME,
    META_INSTRUMENT_NAME,
    META_LYRIC,
    META_MARKER,
    META_CUE_POINT,
    META_CHANNEL_PREFIX = 0x20,
    META_END_OF_TRACK = 0x2f,
    META_SET_TEMPO = 0x51,
    META_SMPTE_OFFSET = 0x54,
    META_TIME_SIGNATURE = 0x58,
    META_KEY_SIGNATURE = 0x59,
    META_SPECIFIC = 0x7f,
};

typedef struct str_mdata mdata_t;
typedef struct str_mdata_track mdata_track_t;

typedef struct str_mdata_note
{
    int duration;
    uint8_t pitch;
    uint8_t dummy[3];
} mdata_note_t;

typedef struct str_mdata_notes
{
    mdata_track_t *parent;
    mdata_note_t note[MAX_NOTE_NUM];
    int pos;
    uint8_t velocity;
    uint8_t dummy[3];
} mdata_notes_t;

typedef struct str_mdata_track
{
    mdata_notes_t *notes;
    uint32_t note_num;
    mdata_t *parent;
} mdata_track_t;

typedef struct str_mdata
{
    mdata_track_t *track;
    uint16_t track_num;
    uint16_t resolution;
    int version;
} mdata_t;
mdata_t *midi_analyze(const char *);

#endif