/* C89 that plays the first four bars of John Williams' Imperial March. */
#include "music.h"

int main(int argc, char** argv)
{
    struct Note note;
    struct AudioState* as;
    as = setup();
    if (as == NULL) return 1;

    /* Let it rip. */
    populate_note(&note, G4,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, G4,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, G4,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, EB4, QUAVER_D, QUAVER_D_BREATH);
    play_note(as, note);
    populate_note(&note, BB4, SQUAVER,  SQUAVER_BREATH);
    play_note(as, note);
    /* */
    populate_note(&note, G4,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, EB4, QUAVER_D, QUAVER_D_BREATH);
    play_note(as, note);
    populate_note(&note, BB4, SQUAVER,  SQUAVER_BREATH);
    play_note(as, note);
    populate_note(&note, G4,  MINIM,    MINIM_BREATH);
    play_note(as, note);
    /* */
    populate_note(&note, D5,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, D5,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, D5,  CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, EB5, QUAVER_D, QUAVER_D_BREATH);
    play_note(as, note);
    populate_note(&note, BB4, SQUAVER,  SQUAVER_BREATH);
    play_note(as, note);
    /* */
    populate_note(&note, GB4, CROTCHET, CROTCHET_BREATH);
    play_note(as, note);
    populate_note(&note, EB4, QUAVER_D, QUAVER_D_BREATH);
    play_note(as, note);
    populate_note(&note, BB4, SQUAVER,  SQUAVER_BREATH);
    play_note(as, note);
    populate_note(&note, G4,  MINIM,    MINIM_BREATH);
    play_note(as, note);

    teardown(as);

    return 0;
}
