#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <smf.h>
#include "mididata.h"
#include "misc.h"
#include "dlog.h"

#define MAX_TRACK_NO 16

static uint8_t pitch(smf_event_t *event);
static int position(smf_event_t *event);
static uint8_t velocity(smf_event_t *event);
static void show_mdata(mdata_t *data);
static void dump_str(char *buf, const uint8_t *data, int len);

// static smf_t *g_smf;
// static int note_num[MAX_TRACK_NO];

#if 0
static bool event_is_tempo(smf_event_t *event)
{
    if (!smf_event_is_metadata(event))
        return false;

    return event->midi_buffer[1] == 0x51;
}
#endif

#if 0
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
#endif

static void get_meta_info(mdata_t *data, smf_t *smf)
{
    data->resolution = smf->ppqn;
    data->version = smf->format;
    data->track_num = 0;
    data->track = NULL;
}

static void meta_event_analyze(mdata_t *data, mdata_track_t *mtrack, smf_event_t *event)
{
    char *p;
    switch (event->midi_buffer[1])
    {
    case META_SEQ_NO:
        break;
    case META_TEXT_EVENT:
    case META_COPY_RIGHT:
        break;
    case META_TRACK_NAME:
        if (event->midi_buffer_length)
        {
            p = calloc(event->midi_buffer_length + 1, 1);
            snprintf(p, event->midi_buffer_length, "%s", event->midi_buffer + 2);
            // dump_str(p, event->midi_buffer, event->midi_buffer_length);
            dlog("%d name:%s", event->track_number, p);
        }
        break;
    case META_INSTRUMENT_NAME:
    case META_LYRIC:
    case META_MARKER:
    case META_CUE_POINT:
    case META_CHANNEL_PREFIX:
    case META_END_OF_TRACK:
    case META_SET_TEMPO:
    case META_SMPTE_OFFSET:
    case META_TIME_SIGNATURE:
    case META_KEY_SIGNATURE:
    case META_SPECIFIC:
    default:
        break;
    }
}

static void note_on(mdata_track_t *mtrack, smf_event_t *event)
{
    int note_no = -1;
    int i;

    if (mtrack->note_num > 0)
    {
        if (mtrack->notes[mtrack->note_num - 1].pos == event->time_pulses)
        {
            note_no = mtrack->note_num - 1;
        }
    }
    if (note_no < 0)
    {
        mtrack->notes = realloc(mtrack->notes, sizeof(mdata_notes_t) * ++mtrack->note_num);
        note_no = mtrack->note_num - 1;
        memset(&mtrack->notes[note_no], 0, sizeof(mdata_notes_t));
        mtrack->notes[note_no].parent = mtrack;
    }

    for (i = 0; i < mtrack->notes[note_no].note[i].pitch; i++)
        ;
    mtrack->notes[note_no].pos = position(event);
    mtrack->notes[note_no].note[i].pitch = pitch(event);
    mtrack->notes[note_no].velocity = velocity(event);
    dlog("note on pitch:%d notes:%p note_no:%d", mtrack->notes[note_no].note[i].pitch, mtrack->notes, note_no);
}

static uint8_t velocity(smf_event_t *event)
{
    return event->midi_buffer[2];
}
static uint8_t pitch(smf_event_t *event)
{
    return event->midi_buffer[1];
}

static int position(smf_event_t *event)
{
    return event->time_pulses;
}

static void note_off(mdata_track_t *mtrack, smf_event_t *event)
{
    bool found = false;
    for (int i = mtrack->note_num - 1; i >= 0; i--)
    {
        for (int j = 0; mtrack->notes[i].note[j].pitch; j++)
        {
            if (mtrack->notes[i].note[j].pitch == pitch(event))
            {
                found = true;
                dlog("note off pitch:%d", mtrack->notes[i].note[j].pitch);
                if (!mtrack->notes[i].note[j].duration)
                {
                    mtrack->notes[i].note[j].duration = position(event) - mtrack->notes[i].pos;
                }
                else
                {
                    dlog("note on not found. %02x %02x %02x", event->midi_buffer[0], event->midi_buffer[1], event->midi_buffer[2]);
                }
                break;
            }
        }
    }
    if (!found)
    {
        dlog("same pitch(%d) not exists.", pitch(event));
    }
}

static void note_event_analyze(mdata_track_t *mtrack, smf_event_t *event)
{
    ERR_RETn(event->midi_buffer_length < 3);

    switch (event->midi_buffer[0])
    {
    case NOTE_ON:
        if (event->midi_buffer[1] == 0)
        {
            note_off(mtrack, event);
        }
        else
        {
            note_on(mtrack, event);
        }
        break;
    case NOTE_OFF:
        note_off(mtrack, event);
        break;
    default:
        break;
    }

error_return:
    return;
}

// static int evcnt = 0;
static void event_analyze(mdata_t *data, mdata_track_t *mtrack, smf_event_t *event)
{
    // printf("ev:%d @%p %p\n", evcnt++, event->track, event);
    if (smf_event_is_metadata(event))
    {
        meta_event_analyze(data, mtrack, event);
    }
    else
    {
        note_event_analyze(mtrack, event);
    }
}

char debug_str[512];
static void dump_str(char *buf, const uint8_t *data, int len)
{
    char *p = buf;
    for (int i = 0; i < len; i++)
    {
        p += (int)sprintf(p, "%02x ", data[i]);
    }
}
static char *dump(const uint8_t *data, int len)
{
    dump_str(debug_str, data, len);
    return debug_str;
}

static void track_analyze(mdata_t *data, smf_track_t *track, int track_no)
{
    mdata_track_t *mtrack = NULL;
    smf_event_t *event;

    printf("data->track_num:%d track_no:%d\n", data->track_num, track_no);
    ERR_RETn(track_no != 0 && data->track_num >= track_no);

    if (track_no > 0)
    {
        data->track_num++;
        data->track = realloc(data->track, sizeof(mdata_track_t) * data->track_num);
        mtrack = &data->track[track_no - 1];
        mtrack->parent = data;
        mtrack->note_num = 0;
        mtrack->notes = NULL;
    }

    track->next_event_number = 1;
    dlog("track:%p event_num:%d", track, track->number_of_events);
    while ((event = smf_track_get_next_event(track)) != NULL)
    {
        if (!event->event_number)
        {
            event->event_number = track->next_event_number - 1;
        }
        // dlog("n:%d c:%d", track->next_event_number, event->event_number);
        if (event->track != track)
        {
            char *p = dump(event->midi_buffer, event->midi_buffer_length);
            dlog("event->track:%p track:%p evn:%d buf:%s", event->track, track, event->event_number, p);
            event->track = track;
        }
        event_analyze(data, mtrack, event);
    }
error_return:
    return;
}

static mdata_t *analyze(smf_t *smf)
{
    mdata_t *data = NULL;
    smf_track_t *track;

    printf("ver:%d tracks:%d ppqn:%d\n", smf->format, smf->number_of_tracks, smf->ppqn);

    data = calloc(sizeof(mdata_t), 1);
    ERR_RETn(!data);

    dlog("format:%d", smf->format);
    switch (smf->format)
    {
    case 0:
        track = smf_get_track_by_number(smf, 0);
        track_analyze(data, track, 1);
        break;
    case 1:
        get_meta_info(data, smf);

        for (int i = 0; i < smf->number_of_tracks; i++)
        {
            track = smf_get_track_by_number(smf, i + 1);
            if (!track)
                continue;
            dlog("track:%p", track);
            track_analyze(data, track, i);
        }
        break;
    case 2:
        break;
    }

    smf_delete(smf);

error_return:
    return data;
}

mdata_t *midi_analyze(const char *path)
{
    mdata_t *data = NULL;
    smf_t *smf;
    dlog("IN");
    smf = smf_load(path);
    ERR_RETn(!smf);

    data = analyze(smf);

#if DEBUG
    show_mdata(data);
#endif

error_return:
    dlog("OUT");
    return data;
}

static void dumpnote(char *buf, mdata_notes_t *note)
{
    char *p = buf;
    for (int i = 0; i < MAX_NOTE_NUM; i++)
    {
        if (!note->note[i].pitch)
            break;
        p += (int)sprintf(p, "%d/%.1f,", note->note[i].pitch, (double)note->note[i].duration / note->parent->parent->resolution);
    }
    if (p != buf)
        p[-1] = 0;
}

static void show_mdata(mdata_t *data)
{
    dlog("mdata: version:%d\n\tres:%d track:%d", data->version, data->resolution, data->track_num);
    for (int i = 0; i < data->track_num; i++)
    {
        dlog("track:%d notes:%d", i, data->track[i].note_num);
        for (int j = 0; j < data->track[i].note_num; j++)
        {
            mdata_notes_t *notes = &data->track[i].notes[j];
            char buf[256];
            dumpnote(buf, notes);
            dlog("\t%08d: v:%d\t\t%s", notes->pos, notes->velocity, buf);
        }
    }
}