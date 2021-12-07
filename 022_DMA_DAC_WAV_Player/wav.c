#include <inttypes.h>
#include "wav.h"
#include "player.h"
#include "xprintf.h"
#include "ff9/src/ff.h"

#define RIFF 'FFIR'
#define WAVE 'EVAW'
#define fmt  ' tmf'
#define data 'atad'

#define PCM 0x0001
#define MONO 1
#define BITS_PER_SAMPLE 8

struct ckhd {
    uint32_t ckID;
    uint32_t cksize;
    uint32_t riffFmt;
} header;

struct fmtck {
    uint32_t ckID;
    uint32_t cksize;
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
} format;

struct datack
{
    uint32_t ckID;
    uint32_t cksize;
} data_chunk;

uint8_t Audiobuf[AUDIOBUFSIZE] = {0};
int audioplayerHalf = 0;
int audioplayerWhole = 0;

static int is_valid_wav(FIL Fil, char *filename)
{
    UINT br;
    FRESULT rc;

    // reading header
    rc = f_read(&Fil, (void *) &header, 12, &br);
    if (rc || !br) return 0;
    if (header.ckID != RIFF) return 0;
    if (header.riffFmt != WAVE) return 0;

    // reading fmt sub-chunk
    rc = f_read(&Fil, (void *) &format, 24, &br);
    if (rc || !br) return 0;
    if (format.ckID != fmt) return 0;
    xprintf("\nReading file format\n");
    if (format.wFormatTag != PCM) return 0;
    xprintf("Sampling rate: %d\n", format.nSamplesPerSec);
    xprintf("PCM\n");
    xprintf("Channels: %d\n", format.nChannels);
    if (format.nChannels != MONO) return 0;
    xprintf("Bits per sample: %d\n", format.wBitsPerSample);
    if (format.wBitsPerSample != BITS_PER_SAMPLE) return 0;

    // reading data sub-chunk header
    rc = f_read(&Fil, (void *) &data_chunk, sizeof data_chunk, &br);
    if (rc || !br) return 0;
    if (data_chunk.ckID != data) return 0;

    return 1; // success validating
}

static void audio_bzero(uint8_t *arr, unsigned int bytes)
{
    for (int i = 0; i < bytes; i++) {
        arr[i] = 0;
    }
}

int wav_play(char *filename)
{
    FRESULT rc;
    FIL Fil;
    UINT br;
    
    rc = f_open(&Fil, filename, FA_READ);
    if (rc) return (rc);
    xprintf("\nValidating file %s\n", filename);
    if (!is_valid_wav(Fil, filename)) return -1;
    xprintf("The file is correct\n");

    audioplayerInit(format.nSamplesPerSec);

    // Filling the audio buffer
    f_read(&Fil, (void *) &Audiobuf, AUDIOBUFSIZE, &br);
    data_chunk.cksize -= br;

    audioplayerStart();

    // reading sound data
    while (1) {
        if (data_chunk.cksize <= 0) {
            break;
        }
        if (audioplayerHalf) { // Half Transfer complete
            if (data_chunk.cksize < AUDIOBUFSIZE/2)
                audio_bzero(Audiobuf, AUDIOBUFSIZE/2);
            f_read(&Fil, Audiobuf, AUDIOBUFSIZE/2, &br);
            data_chunk.cksize -= br;
            audioplayerHalf = 0;

            //xprintf("Half, %d\n", data_chunk.cksize);
        }
        if (audioplayerWhole) { // Transfer complete
            if (data_chunk.cksize < AUDIOBUFSIZE/2)
                audio_bzero(Audiobuf + (AUDIOBUFSIZE/2), AUDIOBUFSIZE/2);
            f_read(&Fil, Audiobuf + (AUDIOBUFSIZE/2), AUDIOBUFSIZE/2, &br);
            data_chunk.cksize -= br;
            audioplayerWhole = 0;

            //xprintf("Whole, %d\n", data_chunk.cksize);
        }
    }

    audioplayerStop();

    rc = f_close(&Fil);
    if (rc) return (rc);
    return 0;
}