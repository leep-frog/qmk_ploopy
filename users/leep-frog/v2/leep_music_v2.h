#pragma once

bool IsMuted(void);
void Mute_handled(keyrecord_t* record);
void LeepMute(void);
void MuteWithoutSound(bool pressed);
void MuteWithSound(bool pressed);

#ifdef AUDIO_ENABLE

#    define DEFINE_SONG_WITH_TEMPO(var_name, sound, tempo) \
        float   var_name##_song[][2] = sound;              \
        uint8_t var_name##_tempo     = tempo;

#    define DEFINE_SONG(var_name, sound)      \
        float   var_name##_song[][2] = sound; \
        uint8_t var_name##_tempo     = TEMPO_DEFAULT;

#    define LEEP_PLAY_SONG(sng, mute_sng) \
        if (!IsMuted()) {    \
            set_tempo(sng##_tempo);    \
            PLAY_SONG(sng##_song);     \
        } else { \
            set_tempo(mute_sng##_tempo);    \
            PLAY_SONG(mute_sng##_song);     \
        }

#    define LEEP_PLAY_LOOP(sng)     \
        if (!_leep_mute) {          \
            set_tempo(sng##_tempo); \
            PLAY_SONG(sng##_song);  \
        }

#else

#    define DEFINE_SONG_WITH_TEMPO(var_name, sound, tempo)
#    define DEFINE_SONG(var_name, sound)
#    define LEEP_PLAY_SONG(sng, mute_sng)
#    define LEEP_PLAY_LOOP(sng)
#    define SONG(sng)

#endif

// Explicitly only define each song once (not once per use) to limit memory usage.
// All song uses should exclusively be used through references to these
// existing songs (and not by creating new song instances).
DEFINE_SONG(zelda_discover, SONG(ZELDA_DISCOVERY));
// DEFINE_SONG(zelda_guardian_battle_song, SONG(ZELDA_GUARDIAN_BATTLE));
DEFINE_SONG(zelda_item_found, SONG(ZELDA_ITEM_FOUND));
DEFINE_SONG(zelda_spirit_orb, SONG(ZELDA_SPIRIT_ORB));

DEFINE_SONG(leep_start, SONG(LEEP_START_SNG));
DEFINE_SONG(leep_end, SONG(LEEP_END_SNG));

DEFINE_SONG(mario_1_up, SONG(MARIO_1_UP));
DEFINE_SONG(mario_lost_a_life, SONG(MARIO_LOST_A_LIFE));
DEFINE_SONG(mario_game_over, SONG(MARIO_GAME_OVER));

DEFINE_SONG(mario_uw_1, SONG(MARIO_UNDERWORLD_1));
DEFINE_SONG(mario_uw_2, SONG(MARIO_UNDERWORLD_2));

DEFINE_SONG(leep_dud, SONG(LEEP_DUD));

DEFINE_SONG(leep_silence, SONG());
DEFINE_SONG(leep_success, SONG(LEEP_SUCCESS));

// We define our own start-up song (instead of using STARTUP_SONG)
// so we can add logic that disables the custom startup song.
#    define SNG_STARTUP() LEEP_PLAY_SONG(zelda_discover, leep_silence)
#    define SNG_RESET() LEEP_PLAY_SONG(mario_game_over, leep_dud)
#    define SNG_MUTE() LEEP_PLAY_SONG(mario_lost_a_life, leep_silence)
#    define SNG_UNMUTE() LEEP_PLAY_SONG(mario_1_up, leep_silence)
#    define SNG_REC_START() LEEP_PLAY_SONG(leep_start, leep_start)
#    define SNG_REC_1_END() LEEP_PLAY_SONG(zelda_item_found, leep_end)
#    define SNG_REC_2_END() LEEP_PLAY_SONG(mario_1_up, leep_end)
#    define SNG_REC_1_PLAY() LEEP_PLAY_SONG(zelda_spirit_orb, leep_success)
#    define SNG_REC_2_PLAY() LEEP_PLAY_SONG(zelda_discover, leep_success)
#    define SNG_EYE_START() LEEP_PLAY_SONG(mario_uw_1, leep_success)
#    define SNG_EYE_END() LEEP_PLAY_SONG(mario_uw_2, leep_success)
#    define SNG_COPY() LEEP_PLAY_SONG(leep_start, leep_start)
#    define SNG_PASTE() LEEP_PLAY_SONG(leep_end, leep_end)
#    define SNG_DUD() LEEP_PLAY_SONG(leep_dud, leep_dud)
