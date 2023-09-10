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

void SNG_STARTUP(void);
void SNG_RESET(void);
void SNG_MUTE(void);
void SNG_UNMUTE(void);
void SNG_REC_START(void);
void SNG_REC_1_END(void);
void SNG_REC_2_END(void);
void SNG_REC_1_PLAY(void);
void SNG_REC_2_PLAY(void);
void SNG_EYE_START(void);
void SNG_EYE_END(void);
void SNG_COPY(void);
void SNG_PASTE(void);
void SNG_DUD(void);
