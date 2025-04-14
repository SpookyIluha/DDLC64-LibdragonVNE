#include <libdragon.h>
#include "engine.h"
#include "types.h"
#include "ddlc_main.h"

#define NUM_BUFFERS (is_memory_expanded()? QUAD_BUFFERED : DOUBLE_BUFFERED)

bool cont = false;

void game_start(){
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, NUM_BUFFERS, GAMMA_NONE, FILTERS_DEDITHER);

    game_intro_consent();
    while(true){
        if(!cont){
            game_intro_logo();
            game_menu();
            if(!cont)
                engine_newgame();
            else engine_continuegame();
        } else engine_continuegame();
    }
}

void game_menu_ghost(){
    sprite_t* fg = sprite_load(filesystem_getfn(DIR_IMAGE, "menu/characters_c").c_str());
    rspq_block_t* block = NULL;
    float time = 0;
    bool restored = false;
    float scale = 1;
    float offset = 0;
    bool exceptioned = false;
    while(true){
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        if(exception_reset_time() > 0 && !exceptioned) { sound_play("baa", false); exceptioned = true; rspq_wait(); offset = -110; scale = 2; rspq_block_free(block); block = NULL;}
        if(!block){
            rspq_block_begin();
            rdpq_clear(RGBA32(255,255,255,0));
            rdpq_set_mode_standard();
            rdpq_mode_filter(FILTER_BILINEAR);
            rdpq_blitparms_t parms; parms.scale_x = scale; parms.scale_y = scale; 
            rdpq_sprite_blit_anchor(fg, ALIGN_CENTER, VALIGN_BOTTOM, display_get_width() / 2 + offset, display_get_height(), &parms);
            block = rspq_block_end();
        } rspq_block_run(block);
        rdpq_textparms_t tparms = {0}; tparms.style_id = gamestatus.fonts.mainfontstyle;
        if(time > 20) rdpq_text_printf(&tparms, gamestatus.fonts.mainfont, 50,50, restored? dictstr("MM_reset_sys") : dictstr("MM_restore_game"));
        rdpq_detach_show();
        time += display_get_delta_time();
        if(time > 20){
            joypad_poll();
            joypad_buttons_t btn  = joypad_get_buttons_pressed(JOYPAD_PORT_1);
            if(btn.b && !restored) {
                sound_play("giggle", false);
                gamestatus.state_persistent.global_game_state = 0; 
                engine_eeprom_delete_saves();
                engine_eeprom_save_persistent(); 
                restored = true;
                rspq_wait();
                rspq_block_free(block); block = NULL;
            }
        }
    }
}

void game_menu(){
    if(gamestatus.state_persistent.global_game_state == 2) game_menu_ghost();
    sprite_t* bg = sprite_load(filesystem_getfn(DIR_IMAGE, "menu/menu_bg.ci4").c_str() );
    surface_t bgsurf = sprite_get_pixels(bg);
    sprite_t* sidebar = sprite_load(filesystem_getfn(DIR_IMAGE, "menu/sidebar").c_str());
    sprite_t* logo = sprite_load(filesystem_getfn(DIR_IMAGE, "menu/logo").c_str());
    sprite_t* characters = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.state_persistent.global_game_state == 1? "menu/characters_b" : "menu/characters_a").c_str());
    sprite_t* button_a = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.a_button_image).c_str());
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

        rdpq_set_mode_standard();
        rdpq_mode_tlut(TLUT_RGBA16);
        rdpq_texparms_t parms; 
        parms.s.translate = 64 - (frame % 64); parms.t.translate = 64 - (frame % 64);
        parms.s.repeats = REPEAT_INFINITE; parms.t.repeats = REPEAT_INFINITE; 
        rdpq_tex_upload(TILE0, &bgsurf, &parms);
        rdpq_tex_upload_tlut(sprite_get_palette(bg), 0, 16);
        rdpq_texture_rectangle(TILE0, 0,0, display_get_width(), display_get_height(), 0, 0);
            audioutils_mixer_update();
        if(time > 3 && !block) rspq_block_begin();
        if(!block){
            rdpq_set_mode_copy(true);
            rdpq_sprite_blit(sidebar, -posoffset / 3,0, NULL);
            rdpq_sprite_blit(characters, 210 + posoffset2,22, NULL);
            rdpq_sprite_blit(logo, 30,(int)(30 - posoffset2 / 2), NULL);

            if(time > 1) posoffset = gfx_lerp(posoffset, 0, 0.1f);
            if(time > 2) posoffset2 = gfx_lerp(posoffset2, 0, 0.1f);
        }
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        if(time > 3 && !block) {block = rspq_block_end();}
        if(block) rspq_block_run(block);
        if(time > 3){
                posoffset3 = gfx_lerp(posoffset3, 0, 0.1f);
                rdpq_sprite_blit(button_a, 30 - posoffset3,260 + selection*40, NULL);
            if(gamestatus.state_persistent.lastsavetype != SAVE_NONE)
                rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,280, dictstr("MM_continue"));
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 80 - posoffset3,320, gamestatus.state_persistent.global_game_state == 1? dictstr("MM_gibber") : (filesystem_ismodded()? dictstr("MM_newgame_modded") :  dictstr("MM_newgame")));
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
                        if(gamestatus.state_persistent.global_game_state == 1) {
                            bgm_stop(0); 
                            sound_stop();
                            timesys_init();
                            scripts_sequence_load_and_setlabel("ch0", "[Main_Epilogue]", 0);
                            cont = true;
                        }
                        else{
                            overlays_set_player_name(dictstr("MM_entername")); 
                            overlays_message(dictstr("MM_tutorial")); 
                            cont = false;
                        }
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
    sprite_free(sidebar);
    sprite_free(logo);
    sprite_free(characters);
    sprite_free(button_a);
    rspq_block_free(block);
}


void game_pause_menu(){
    if(gamestatus.state_persistent.global_game_state > 0) return;
    sound_play(gamestatus.data.defaults.selectsound, false);
    sprite_t* bg = sprite_load( filesystem_getfn(DIR_IMAGE, "menu/menu_bg.ci4").c_str());
    surface_t bgsurf = sprite_get_pixels(bg);
    sprite_t* sidebar = sprite_load( filesystem_getfn(DIR_IMAGE, "menu/sidebar").c_str());
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

        rdpq_set_mode_standard();
        rdpq_mode_tlut(TLUT_RGBA16);
        rdpq_texparms_t parms; 
        parms.s.translate = 64 - (frame % 64); parms.t.translate = 64 - (frame % 64);
        parms.s.repeats = REPEAT_INFINITE; parms.t.repeats = REPEAT_INFINITE; 
        rdpq_tex_upload(TILE0, &bgsurf, &parms);
        rdpq_tex_upload_tlut(sprite_get_palette(bg), 0, 16);
        rdpq_texture_rectangle(TILE0, 0,0, display_get_width(), display_get_height(), 0, 0);
            audioutils_mixer_update();
        if(time > 1 && !block) rspq_block_begin();
        if(!block){
            rdpq_set_mode_copy(true);
            rdpq_sprite_blit(sidebar, -posoffset / 3,0, NULL);
            rdpq_text_printf(&parmstext, gamestatus.fonts.titlefont, 500 + posoffset,40, dictstr("MM_paused"));
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
    sprite_free(sidebar);
    sprite_free(button_a);
    rspq_block_free(block);
}

void game_intro_logo(){
    sprite_t* splash = NULL;
    switch(gamestatus.state_persistent.global_game_state){
        case 0: splash = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/splash").c_str()); break;
        case 1: splash = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/splash-glitch2").c_str()); break;
        case 2: splash = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/white").c_str()); break;
        default: splash = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/splash").c_str()); break;
    }

    bgm_play(gamestatus.state_persistent.global_game_state == 2? "heartbeat" : "1", true, 0.1f);

    float time = 0;
    while(time < 3){
        rdpq_attach(display_get(), NULL);

        float alpha = time * 255;
        if(time > 1) alpha = (3 - time) * 255; 

        if(alpha > 250) alpha = 250;
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER1((TEX0,PRIM,PRIM_ALPHA,PRIM),(0,0,0,1)));
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, alpha));
        rdpq_sprite_blit(splash, 0,0, NULL);
        rdpq_detach_show();

        time += display_get_delta_time();
            audioutils_mixer_update();
    }
    time = 0;
    while(time < 3){
        rdpq_attach(display_get(), NULL);
        rdpq_clear(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));

        float alpha = time * 255;
        if(time > 1) alpha = (3 - time) * 255;

        if(alpha > 250) alpha = 250;
        rdpq_textparms_s parmstext; parmstext.style_id = 1;
        color_t color1 = RGBA32(0x00, 0x00, 0x00, alpha);
        color_t color2 = RGBA32(0xFF, 0xFF, 0xFF, 0xFF);
        rdpq_fontstyle_t parms; parms.color = color1; parms.outline_color = color2;
        rdpq_font_style(gamestatus.fonts.fonts[0], 1, &parms);
        rdpq_text_printf(&parmstext, gamestatus.fonts.mainfont, 180,220, gamestatus.state_persistent.global_game_state == 2? dictstr("MM_not_suitable") : dictstr("MM_not_suitable2"));
        rdpq_detach_show();

        time += display_get_delta_time();
            audioutils_mixer_update();
    }
    sprite_free(splash);
}

void game_intro_consent(){
    sprite_t* bg1 = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/warning").c_str());
    sprite_t* bg2 = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/warning2").c_str());
    sprite_t* button = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.button_select).c_str());
    sprite_t* button_a = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.a_button_image).c_str());

    bool consent = false;
    float time = 0;
    while(!consent){
        rdpq_attach(display_get(), NULL);

        rdpq_set_mode_copy(false);
        rdpq_sprite_blit(bg1, 0,0, NULL);
        rdpq_set_mode_standard();
        rdpq_sprite_blit(button, 120,400, NULL);
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        if(time > 5) rdpq_sprite_blit(button_a, 80,400, NULL);

        rdpq_textparms_s parmstext = {0}; parmstext.valign = VALIGN_CENTER; parmstext.align = ALIGN_CENTER; parmstext.style_id = gamestatus.fonts.mainfontstyle;
        rdpq_text_printf(&parmstext, gamestatus.fonts.mainfont, 120,70, dictstr("WARN_label_a"));
        rdpq_text_printf(&parmstext, gamestatus.fonts.mainfont, 120,120, dictstr("WARN_label_b"));
        rdpq_text_printf(&parmstext, gamestatus.fonts.mainfont, 120,210, dictstr("WARN_label_c"));
        rdpq_text_printf(&parmstext, gamestatus.fonts.mainfont, 120,280, dictstr("WARN_label_d"));
        parmstext.width = button->width; parmstext.height = button->height;
        rdpq_text_printf(&parmstext, gamestatus.fonts.mainfont, 120,400, dictstr("WARN_label_e"));

        rdpq_detach_show();

        joypad_poll();
        joypad_buttons_t btn  = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        if(btn.a && time > 5) {
            consent = true;
            sound_play(gamestatus.data.defaults.selectsound, false);
        }
            audioutils_mixer_update();
        time += display_get_delta_time();
    }

    time = 0;
    while(time < 3)
    {
        time += display_get_delta_time();
        rdpq_attach(display_get(), NULL);

        rdpq_set_mode_copy(false);
        rdpq_sprite_blit(bg1, 0,0, NULL);
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_set_fog_color(RGBA32(0, 0, 0, time * 75));
        rdpq_sprite_blit(bg2, 0,0, NULL);
        rdpq_detach_show();
            audioutils_mixer_update();
    }

    sprite_free(bg1);
    sprite_free(bg2);
    sprite_free(button);
    sprite_free(button_a);
}