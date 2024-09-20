#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <smf.h>
#include "musicxml.h"
#include "misc.h"
#include "dlog.h"

#define MAX_TRACK_NO 16

static smf_t *g_smf;
static int note_num[MAX_TRACK_NO];

static bool event_is_tempo(smf_event_t *event)
{
    if (!smf_event_is_metadata(event))
        return false;

    return event->midi_buffer[1] == 0x51;
}

static int get_note_num(smf_t *smf, int track_no)
{
    smf_event_t *event;
    int cnt = 0;
    if (g_smf == smf)
    {
        return note_num[track_no];
    }

    g_smf = smf;

    for (int i = 0; i < smf->number_of_tracks; i++)
    {
        smf_track_t *track = smf_get_track_by_number(smf, i + 1);
        while ((event = smf_track_get_next_event(track)))
        {
            if (smf_event_is_sysex(event))
            {
                // printf("sysex\n");
                continue;
            }
            if (smf_event_is_system_common(event))
            {
                // printf("sys common\n");
                continue;
            }
            if (smf_event_is_system_realtime(event))
            {
                // printf("sys realtime\n");

                continue;
            }
            if (smf_event_is_textual(event))
            {
                if (event_is_tempo(event))
                {
                    uint32_t qms = midival(&event->midi_buffer[3]);
                    if (qms)
                        printf("tempo:%d\n", 60000000 / qms);
                }
                else
                {
                }
            }
            if (smf_event_is_metadata(event))
            {
                // printf("sys textual\n");
                continue;
            }
            // 3cがド
            printf("%7d %02x %02x %02x\n", event->time_pulses, event->midi_buffer[0], event->midi_buffer[1], event->midi_buffer[2]);
            cnt++;
        }
        note_num[i] = cnt;
    }
    return note_num[track_no];
}

static int get_track_note_num(smf_track_t *track)
{
    for (int i = 0; i < track->smf->number_of_tracks; i++)
    {
        if (track == smf_get_track_by_number(track->smf, i + 1))
        {
            return get_note_num(track->smf, i);
        }
    }
    return 0;
}

static bool has_note(smf_track_t *track)
{
    return get_track_note_num(track) > 0;
}

static char *convert(smf_t *smf, m2x_t* m2x)
{
    smf_track_t *track;

    printf("ver:%d tracks:%d ppqn:%d\n", smf->format, smf->number_of_tracks, smf->ppqn);

    for (int i = 0; i < smf->number_of_tracks; i++)
    {
        int note = 0;
        int meta = 0;

        track = smf_get_track_by_number(smf, i + 1);
        if (!track)
            continue;
        note = get_track_note_num(track);
        meta = track->number_of_events - note;

        printf("track:%d note:%d, else:%d\n", i, note, meta);
    }

    smf_delete(smf);
    return NULL;
}

char *to_musicxml(const char *path)
{
    char *xml = NULL;
    smf_t *smf;
    m2x_t m2x;
    smf = smf_load(path);
    ERR_RETn(!smf);

    xml = convert(smf, &m2x);

error_return:

    return xml;
}