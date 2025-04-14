#include <libdragon.h>
#include "engine_filesystem.h"
#include "audioutils.h"
#include "engine_gamestatus.h"

wav64_t bgmusic;
wav64_t sound;

void audioutils_mixer_update(){
    float volume = gamestatus.state.audio.bgmusic_vol;
    mixer_try_play();
    if(gamestatus.state.audio.transitionstate == 1 && gamestatus.state.audio.transitiontime > 0){
        gamestatus.state.audio.transitiontime -= display_get_delta_time();

        if(gamestatus.state.audio.transitiontime <= 0) {
            gamestatus.state.audio.transitionstate = 2;
            if(gamestatus.state.audio.bgmusic_playing){
                gamestatus.state.audio.bgmusic_playing = false;
                mixer_ch_stop(AUDIO_CHANNEL_MUSIC);
                rspq_wait();
                if(bgmusic.st) wav64_close(&bgmusic);
            }
            if(gamestatus.state.audio.bgmusic_name[0]){
                wav64_open(&bgmusic, filesystem_getfn(DIR_MUSIC, gamestatus.state.audio.bgmusic_name).c_str());
                wav64_set_loop(&bgmusic, gamestatus.state.audio.loopingmusic);
                wav64_play(&bgmusic, AUDIO_CHANNEL_MUSIC);
                gamestatus.state.audio.bgmusic_playing = true;
            }
        }
        volume *= (gamestatus.state.audio.transitiontime / gamestatus.state.audio.transitiontimemax);
    }
    if(gamestatus.state.audio.transitionstate == 2 && gamestatus.state.audio.transitiontime < gamestatus.state.audio.transitiontimemax){
        gamestatus.state.audio.transitiontime += display_get_delta_time();
        volume *= (gamestatus.state.audio.transitiontime / gamestatus.state.audio.transitiontimemax);
    }
    mixer_ch_set_vol(AUDIO_CHANNEL_MUSIC, volume, volume);
}

void bgm_hardplay(const char* name, bool loop, float transition){
    gamestatus.state.audio.loopingmusic = loop;
    gamestatus.state.audio.transitionstate = 0;
    gamestatus.state.audio.transitiontime = 0;
    gamestatus.state.audio.transitiontimemax = 1;
    wav64_open(&bgmusic, filesystem_getfn(DIR_MUSIC, name).c_str());
    wav64_set_loop(&bgmusic, gamestatus.state.audio.loopingmusic);
    wav64_play(&bgmusic, AUDIO_CHANNEL_MUSIC);
    gamestatus.state.audio.bgmusic_playing = true;
    strcpy(gamestatus.state.audio.bgmusic_name, name);
}

void bgm_play(const char* name, bool loop, float transition){
    if(transition == 0) { bgm_hardplay(name, loop, transition); return;}
    bgm_stop(1);
    gamestatus.state.audio.loopingmusic = loop;
    gamestatus.state.audio.transitionstate = 1;
    gamestatus.state.audio.transitiontime = transition;
    gamestatus.state.audio.transitiontimemax = transition;
    strcpy(gamestatus.state.audio.bgmusic_name, name);
}


void bgm_hardstop(){
    gamestatus.state.audio.bgmusic_playing = false;
    gamestatus.state.audio.transitionstate = 0;
    gamestatus.state.audio.transitiontime = 0.1;
    gamestatus.state.audio.transitiontimemax = 0.1;
    gamestatus.state.audio.bgmusic_name[0] = 0;
    mixer_ch_stop(AUDIO_CHANNEL_MUSIC);
    rspq_wait();
    if(bgmusic.st) wav64_close(&bgmusic);
}

void bgm_stop(float transition){
    if(transition == 0) { bgm_hardstop(); return;}
    gamestatus.state.audio.transitionstate = 1;
    gamestatus.state.audio.transitiontime = 1;
    gamestatus.state.audio.transitiontimemax = 1;
    gamestatus.state.audio.bgmusic_name[0] = 0;
}

void sound_play(const char* name, bool loop){
    sound_stop();
    wav64_open(&sound, filesystem_getfn(DIR_SOUND, name).c_str());
    wav64_set_loop(&sound, loop);
    wav64_play(&sound, AUDIO_CHANNEL_SOUND);
    gamestatus.state.audio.sound_playing = true;
    strcpy(gamestatus.state.audio.sound_name, name);
}

void sound_stop(){
    if(gamestatus.state.audio.sound_playing){
        rspq_wait();
        mixer_ch_stop(AUDIO_CHANNEL_SOUND);
        if(sound.st) wav64_close(&sound);
    }
    gamestatus.state.audio.sound_playing = false;
    gamestatus.state.audio.sound_name[0] = 0;
}

void music_volume(float vol){
    gamestatus.state.audio.bgmusic_vol = vol;
}

void sound_volume(float vol){
    mixer_ch_set_vol(AUDIO_CHANNEL_SOUND, vol, vol);
    gamestatus.state.audio.sound_vol = vol;
}

float music_volume_get() {return gamestatus.state.audio.bgmusic_vol;};

float sound_volume_get() {return gamestatus.state.audio.sound_vol;};