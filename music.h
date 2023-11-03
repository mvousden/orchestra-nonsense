#ifndef MUSIC_H
#define MUSIC_H
#include <SDL.h>
#include <SDL_mixer.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

struct AudioState
{
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec wavSpec;
    double soundData[3]; /* Phase shift, frequency, amplitude [0,1] */
};

struct Note
{
    double pitch;
    unsigned length;
    unsigned breath;
};

/* Arbitrary */
#define SQUAVER 125
#define SQUAVER_BREATH 12
/* Relative */
#define QUAVER SQUAVER*2
#define QUAVER_BREATH SQUAVER_BREATH * 2
#define QUAVER_T QUAVER / 3
#define QUAVER_T_BREATH QUAVER_BREATH / 3
#define QUAVER_D QUAVER + SQUAVER
#define QUAVER_D_BREATH QUAVER_BREATH + SQUAVER_BREATH
#define CROTCHET QUAVER * 2
#define CROTCHET_BREATH QUAVER_BREATH * 2
#define MINIM CROTCHET * 2
#define MINIM_BREATH CROTCHET_BREATH * 2

/* Hz */
#define GB3 185.00
#define G3  196.00
#define EB4 311.13
#define GB4 369.99
#define G4  392.00
#define BB4 466.16
#define D5  587.33
#define EB5 622.25

void populate_note(struct Note*, double, unsigned, unsigned);
void play_wave(struct AudioState*, double, unsigned, unsigned);
void play_note(struct AudioState*, struct Note);
void wavcb(void*, uint8_t*, int);
struct AudioState* setup(void);
void teardown(struct AudioState*);
#endif
