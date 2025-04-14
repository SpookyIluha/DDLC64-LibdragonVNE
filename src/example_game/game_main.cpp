#include <libdragon.h>
#include "engine.h"
#include "types.h"
#include "game_main.h"

#define NUM_BUFFERS (is_memory_expanded()? QUAD_BUFFERED : DOUBLE_BUFFERED)

bool cont = false;

void game_start(){
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, NUM_BUFFERS, GAMMA_NONE, FILTERS_DEDITHER);

    while(true){
        if(!cont){
            game_menu();
            if(!cont)
                engine_newgame();
            else engine_continuegame();
            cont = false;
        } else engine_continuegame();
    }
}

void game_menu(){
    sprite_t* bg = sprite_load(filesystem_getfn(DIR_IMAGE, "menu/menu").c_str() );
    sprite_t* button_a = sprite_load( filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.a_button_image).c_str());
    surface_t bgsurf = sprite_get_pixels(bg);
    rspq_block_t* block = NULL;

    rdpq_textparms_s parmstext; parmstext.style_id = gamestatus.fonts.titlefontstyle; parmstext.disable_aa_fix = true;

    float time = 0;
    int frame = 0;
    bool gamestart = false;
    float posoffset = 450;
    float posoffset2 = 450;
    float posoffset3 = 450;
    int selection = 0;
    if(gamestatus.state_persistent.lastsavetype == SAVE_NONE) selection = 1;
    while(!gamestart){
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);

        audioutils_mixer_update();
        if(time > 1 && !block) rspq_block_begin();
        if(!block){
            rdpq_set_mode_copy(true);
            rdpq_sprite_blit(bg, 0,0,0);
            if(time > 0) posoffset = gfx_lerp(posoffset, 0, 0.1f);
            if(time > 0) posoffset2 = gfx_lerp(posoffset2, 0, 0.1f);
        }
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        if(time > 1 && !block) {block = rspq_block_end();}
        if(block) rspq_block_run(block);
        if(time > 0){
                posoffset3 = gfx_lerp(posoffset3, 0, 0.1f);
                rdpq_sprite_blit(button_a, 30 - posoffset3,260 + selection*40, NULL);
            if(gamestatus.state_persistent.lastsavetype != SAVE_NONE)
                rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,280, dictstr("MM_continue"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,320, (filesystem_ismodded()? dictstr("MM_newgame_modded") :  dictstr("MM_newgame")));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,360, music_volume_get() > 0? dictstr("MM_music_on") : dictstr("MM_music_off"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,400, sound_volume_get() > 0? dictstr("MM_sounds_on") :dictstr ("MM_sounds_off"));
            rdpq_detach_show();
            audioutils_mixer_update();
            joypad_poll();

            joypad_buttons_t btn  = joypad_get_buttons_pressed(JOYPAD_PORT_1);

            if(btn.d_up) {selection--; sound_play(gamestatus.data.defaults.selectsound, false);}
            if(btn.d_down) {selection++; sound_play(gamestatus.data.defaults.selectsound, false);}
            if(selection < 0) selection = 3;
            if(selection > 3) selection = 0;
            if(gamestatus.state_persistent.lastsavetype == SAVE_NONE && selection == 0) selection = 3;
            if(btn.a) 
                switch(selection){
                    case 0:{
                        bgm_stop(0); 
                        if(gamestatus.state_persistent.lastsavetype == SAVE_MANUALSAVE) engine_eeprom_load_manual();
                        else engine_eeprom_load_autosave();
                        sound_play(gamestatus.data.defaults.selectsound, false); gamestart = true; cont = true;
                    } break;
                    case 1:{
                        sound_play(gamestatus.data.defaults.selectsound, false); 
                        overlays_message(dictstr("MM_tutorial")); 
                        cont = false;
                        gamestart = true;
                    } break;
                    case 2:{
                        {music_volume(1 - music_volume_get()); sound_play(gamestatus.data.defaults.selectsound, false);}
                    } break;
                    case 3:{
                        {sound_volume(1 - sound_volume_get()); sound_play(gamestatus.data.defaults.selectsound, false);}
                    } break;
                }
        } else rdpq_detach_show();
            time += display_get_delta_time(); frame++;
            audioutils_mixer_update();
    }

    rspq_wait();

    sprite_free(bg);
    sprite_free(button_a);
    rspq_block_free(block);
}


void game_pause_menu(){
    if(gamestatus.state_persistent.global_game_state > 0) return;
    sound_play(gamestatus.data.defaults.selectsound, false);
    sprite_t* bg = sprite_load( filesystem_getfn(DIR_IMAGE, "menu/menu").c_str());
    surface_t bgsurf = sprite_get_pixels(bg);
    sprite_t* button_a = sprite_load( filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.a_button_image).c_str());
    rspq_block_t* block = NULL;

    rdpq_textparms_s parmstext; parmstext.style_id = gamestatus.fonts.titlefontstyle; parmstext.disable_aa_fix = true;

    float time = 0;
    int frame = 0;
    bool gamestart = false;
    float posoffset = 450;
    float posoffset3 = 450;
    int selection = 0;
    selection = 0;
    while(!gamestart){
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        audioutils_mixer_update();
        if(time > 1 && !block) rspq_block_begin();
        if(!block){
            rdpq_set_mode_copy(true);
            rdpq_sprite_blit(bg, 0,0, NULL);
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 500 + posoffset,400, dictstr("MM_paused"));
            if(time > 0) posoffset = gfx_lerp(posoffset, 0, 0.1f);
        }
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        if(time > 1 && !block) {block = rspq_block_end();}
        if(block) rspq_block_run(block);
        if(time > 1){
                posoffset3 = gfx_lerp(posoffset3, 0, 0.1f);
                rdpq_sprite_blit(button_a, 30 - posoffset3,160 + selection*40, NULL);
            
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,180, dictstr("MM_continue"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,220, dictstr("MM_autosave"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,260, dictstr("MM_loadlast"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,300, dictstr("MM_savegame"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,340, dictstr("MM_mainmenu"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,380, music_volume_get() > 0? dictstr("MM_music_on") : dictstr("MM_music_off"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,420, sound_volume_get() > 0? dictstr("MM_sounds_on") : dictstr("MM_sounds_off"));
            rdpq_detach_show();
            joypad_poll();
            audioutils_mixer_update();

            joypad_buttons_t btn  = joypad_get_buttons_pressed(JOYPAD_PORT_1);

            if(btn.d_up) {selection--; sound_play(gamestatus.data.defaults.selectsound, false);}
            if(btn.d_down) {selection++; sound_play(gamestatus.data.defaults.selectsound, false);}
            if(selection < 0) selection = 6;
            if(selection > 6) selection = 0;
            if(btn.a || btn.start) {
                if(btn.start) selection = 0;
                sound_play(gamestatus.data.defaults.selectsound, false);
                switch(selection){
                    case 0:{
                        goto pmenu_end;
                    } break;
                    case 1:{
                        if(gamestatus.state_persistent.autosaved){
                            bgm_stop(0); 
                            engine_eeprom_load_autosave();
                            gamestart = true; cont = true;
                            gamestatus.state.seq.lastresult = SEQ_RETURN_TO_MENU;
                            goto pmenu_end;
                        } else overlays_message(dictstr("MM_autosavenfound"));
                    } break;
                    case 2:{
                        if(gamestatus.state_persistent.manualsaved){
                            bgm_stop(0); 
                            engine_eeprom_load_manual();
                            gamestart = true; cont = true;
                            gamestatus.state.seq.lastresult = SEQ_RETURN_TO_MENU;
                            goto pmenu_end;
                        } else overlays_message(dictstr("MM_savenfound"));
                    } break;
                    case 3:{
                        engine_eeprom_save_manual(); overlays_message(dictstr("MM_saved"));
                    } break;
                    case 4:{
                        cont = false;
                        gamestatus.state.seq.lastresult = SEQ_RETURN_TO_MENU;
                        goto pmenu_end;
                    } break;
                    case 5:{
                        {music_volume(1 - music_volume_get());}
                    } break;
                    case 6:{
                        {sound_volume(1 - sound_volume_get());}
                    } break;
                }
            }
        } else rdpq_detach_show();

            time += display_get_delta_time(); frame++;
            audioutils_mixer_update();
    }
    pmenu_end:

    rspq_wait();

    sprite_free(bg);
    sprite_free(button_a);
    rspq_block_free(block);
}
