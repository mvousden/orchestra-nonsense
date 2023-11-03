#include "music.h"

/* Conveniently populates a Note */
void populate_note(struct Note* note,
                   double frequency, unsigned duration, unsigned pauseAfter)
{
    note->pitch = frequency;
    note->length = duration;
    note->breath = pauseAfter;
}

/* Callback function for the SDL wave. Populates all elements of the buffer
 * with a sine wave. The frequency of the sine wave is defined by soundData:
 *  - zeroth element is phase shift (usually zero)
 *  - first element determines frequency, accounting for the frequency of the
 *    audio device
 *  - second element determines amplitude ([0, 1]) */
void wavcb(void* data, uint8_t* buffer, int bufferLength)
{
    int i;
    double* soundData = (double*)(data);
    for (i = 0; i < bufferLength; i++)
    {
        buffer[i] =
            (uint8_t)((sin(soundData[0] / soundData[1] * 3.14159265) + 1)
                      * soundData[2] * 127.5);
        soundData[0]++;
    }
}

/* Makes some noise (and then doesn't). Blocking. */
void play_note(struct AudioState* as, struct Note note)
{play_wave(as, note.pitch, note.length, note.breath);}
void play_wave(struct AudioState* as,
    double frequency, unsigned duration, unsigned pauseAfter)
{
    ((double*)as->wavSpec.userdata)[1] = 44100 / frequency;
    SDL_PauseAudioDevice(as->deviceId, 0);
    SDL_Delay(duration);
    SDL_PauseAudioDevice(as->deviceId, 1);
    SDL_Delay(pauseAfter);
}

struct AudioState* setup(void)
{
    struct AudioState* as;
    SDL_AudioSpec of;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("%s\n", SDL_GetError());
        return NULL;
    }

    as = (struct AudioState*)malloc(sizeof(struct AudioState));
    as->soundData[0] = 0;
    as->soundData[1] = 0;
    as->soundData[2] = 0.1;

    as->wavSpec.freq = 44100;
    as->wavSpec.format = AUDIO_S16SYS;
    as->wavSpec.channels = 1;
    as->wavSpec.samples = 2048;
    as->wavSpec.callback = wavcb;
    as->wavSpec.userdata = &as->soundData;
    as->deviceId = SDL_OpenAudioDevice(NULL, 0, &as->wavSpec, &of,
                                       SDL_AUDIO_ALLOW_ANY_CHANGE);
    if((as->wavSpec.format != of.format) || as->deviceId == 0)
    {
        printf("%s\n", SDL_GetError());
        return NULL;
    }
    return as;
}

void teardown(struct AudioState* as)
{
    SDL_CloseAudioDevice(as->deviceId);
    free(as);
    SDL_Quit();
}
