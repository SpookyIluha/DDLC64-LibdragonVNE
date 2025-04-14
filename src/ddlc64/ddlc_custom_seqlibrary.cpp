#include <libdragon.h>
#include "tortellini.hh"
#include "engine.h"
#include "ddlc_poem_game.h"
#include "ddlc_custom_seqlibrary.h"

void game_seqlibrary_customs_load(){
    seqlibraryfuncs["poem_minigame"] =  ddlcseq_poem_minigame;
    seqlibraryfuncs["showpoem"] =       ddlcseq_showpoem;
    seqlibraryfuncs["ddlc_credits"] =   ddlcseq_credits;
    seqlibraryfuncs["ddlc_sayori_d"] =  ddlcseq_sayori_d;
    seqlibraryfuncs["ddlc_sayori_d2"] =  ddlcseq_sayori_d2;
}

actionfuncres_t ddlcseq_poem_minigame(std::vector<std::string>& argv){
    assert(argv.size() == 1);
    resolve_dirty_flag();
    poemgame::poem_minigame(atoi(argv[0].c_str()));
    return SEQ_CONTINUE;
}

actionfuncres_t ddlcseq_showpoem(std::vector<std::string>& argv){
    assert(argv.size() == 1);
    rspq_wait();
    resolve_dirty_flag();
    sound_play("pageflip", false);
    sprite_t* background = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/poem").c_str());
    sprite_t* button_a   = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.a_button_image).c_str());
    rspq_block_t* block = NULL;

    tortellini::ini ini;
	std::ifstream in(filesystem_getfn(DIR_SCRIPT_LANG, (std::string("poems/") + argv[0] + ".ini").c_str()).c_str());
	in >> ini;

    std::string author =    ini["Poem"]["author"]   | "unknown";
    std::string music =     ini["Poem"]["music"]    | "";
    std::string title =     ini["Poem"]["title"]    | "unknown";
    std::string text =      ini["Poem"]["text"]     | "unknown";
    std::string text_full = title + "\n\n" + text;
    std::string::size_type index = 0;
    while ((index = text_full.find("\\n", index)) != std::string::npos) {
        text_full.replace(index, 2, "\n");
        ++index;
    }
    int font = ini["Poem"]["font"] | 1;

    if(!music.empty())
        bgm_play(music.c_str(), true, 1);
    
    bool pressed_a = false;
    float offset = 0;

    float transition = 0;
    while(transition < 1){
        float alpha = transition*255;
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_set_prim_color(RGBA32(alpha,alpha,alpha, 255));
        if(!block){
            rspq_block_begin();
            rdpq_sprite_blit(background, 0,0,0);
            block = rspq_block_end();
        }   rspq_block_run(block);
        transition += display_get_delta_time();
        rdpq_detach_show();
    }
    rdpq_textparms_t textparms;
    textparms.width = 500;
    textparms.align = ALIGN_LEFT; 
    textparms.valign = VALIGN_TOP;
    textparms.wrap = WRAP_WORD;

    int nbytes = strlen(text_full.c_str());
    rdpq_paragraph_t* paragraph = rdpq_paragraph_build(&textparms, font, text_full.c_str(), &nbytes);
    float height = paragraph->bbox.y1 - paragraph->bbox.y0 - 300;
    if(height < 0 ) height = 0;

    while(!pressed_a){
        audioutils_mixer_update();
        joypad_poll();

        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
        if(pressed.a) pressed_a = true;
        if(held.d_down) offset += 3;
        if(held.d_up)   offset -= 3;
        if(offset < 0)  offset  = 0;
        if(offset > height) offset = height;

        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_copy(true);
        rspq_block_run(block);
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(button_a, 580, 430, NULL);
        rdpq_paragraph_render(paragraph, 80, 50 - offset);

        rdpq_detach_show();
    }
    rspq_wait();
    sound_play("pageflip", false);
    sprite_free(background); background = NULL;
    sprite_free(button_a);   button_a = NULL;
    rdpq_paragraph_free(paragraph); paragraph = NULL;
    if(block) {rspq_block_free(block);} block = NULL;
    return SEQ_CONTINUE;
}


actionfuncres_t ddlcseq_credits(std::vector<std::string>& argv){
    rspq_wait();
    bgm_stop(1);
    float time = 0;

    tortellini::ini ini;
	std::ifstream in(filesystem_getfn(DIR_SCRIPT_LANG, "credits.ini").c_str());
	in >> ini;

    std::string end = ini["Credits"]["end"] | "unknown";

    while(time < 2) {
        audioutils_mixer_update();
        rdpq_attach_clear(display_get(), NULL);
        rdpq_detach_show();
        time += display_get_delta_time();
    }
    time = 0;
    bgm_play("monika-end", false, 1);
    while(time < 10){
        float alpha = 255;
        if(time < 2) alpha = time * 127;
        if(time > 6) alpha = (10 - time) * 63;
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        rdpq_clear(RGBA32(0,0,0,0));
        rdpq_textparms_t textparms; textparms.align = ALIGN_CENTER;
        textparms.width = display_get_width();
        rdpq_text_printf(&textparms, 9, 0, display_get_height() / 2, end.c_str());
        rdpq_set_mode_standard();
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_set_prim_color(RGBA32(0,0,0,255 - alpha));
        rdpq_fill_rectangle(0,0,display_get_width(), display_get_height());
        rdpq_detach_show();
        time += display_get_delta_time();
    }
    rspq_block_t* block = NULL;
    std::string title, description;
    char keyt[8]; char keyd[8]; size_t index = 0;
    bool allcgunlocked = true;
    do {
        time = 0;
        sprintf(keyt, "t%i", index + 1); sprintf(keyd, "d%i", index + 1);
        title =         ini["Credits"][keyt] | "";
        description =   ini["Credits"][keyd] | "";
        size_t idx = 0; while ((idx = description.find("\\n", idx)) != std::string::npos) {
            description.replace(idx, 2, "\n");
            ++idx;
        }
        sprite_t* background = NULL;
        if(index < scripts_cglist_get_count()){
            char backgroundfn[LONGSTR_LENGTH];
            sprintf(backgroundfn, "%s/%s.sprite", gamestatus.data.cgfolder, scripts_cglist_get_filename_at(index).c_str());
            debugf("loading cg: %s\n", backgroundfn);
            background = sprite_load(filesystem_getfn(DIR_CG, scripts_cglist_get_filename_at(index).c_str()).c_str());
        }
        while(time < 6){
            audioutils_mixer_update();
            float alpha = 255;
            if(time < 1) alpha = time * 255;
            if(time > 4) alpha = (6 - time) * 127;
            rdpq_attach(display_get(), NULL);
            rdpq_set_fog_color(RGBA32(0,0,0,alpha));
            if(!block){
                rspq_block_begin();
                if(!background) rdpq_clear(RGBA32(0,0,0,0));
                else  {
                    rdpq_set_mode_standard(); 
                    rdpq_mode_dithering(DITHER_SQUARE_SQUARE);
                    surface_t surf = sprite_get_pixels(background);
                    if(!scripts_cglist_get_viewed_flag(index)) {
                        surf.flags = FMT_IA16; // color to grayscale hacky interpret
                        allcgunlocked = false;
                    }
                    rdpq_set_env_color(RGBA32(11,11,11,0)); // add noise to grayscale images
                    rdpq_mode_combiner(RDPQ_COMBINER1((NOISE,0,ENV,TEX0),(0,0,0,1)));
                    rdpq_mode_blender(RDPQ_BLENDER((IN_RGB, FOG_ALPHA, FOG_RGB, INV_MUX_ALPHA)));
                    rdpq_tex_blit(&surf,0,0,NULL);
                }
            rdpq_textparms_t textparms; textparms.align = ALIGN_CENTER; textparms.style_id = gamestatus.fonts.titlefontstyle;
            textparms.width = display_get_width();
            rdpq_text_printf(&textparms, gamestatus.fonts.titlefont, 0, display_get_height() / 2 ,        title.c_str());
            textparms.style_id = gamestatus.fonts.mainfontstyle;
            rdpq_text_printf(&textparms, gamestatus.fonts.mainfont, 0, display_get_height() / 2 + 24 ,   description.c_str());
            block = rspq_block_end();
        } rspq_block_run(block);
            rdpq_detach_show();
            time += display_get_delta_time();
        }
        rspq_wait();
        if(background){ sprite_free(background); background = NULL;}
        if(block)     { rspq_block_free(block);  block = NULL;}
        index++;
    } while(!title.empty());
    if(allcgunlocked){
        std::vector<std::string> poemargs = std::vector<std::string>();
        poemargs.push_back("poem_m4");
        ddlcseq_showpoem(poemargs);
    }
    assertf(!&"Oh jeez...I didn't break anything, did I? Hold on a sec, I can probably fix this...I think... Actually, you know what? This would probably be a lot easier if I just deleted her. She's the one who's making this so difficult. Ahaha! Well, here goes notFOvdo=43FHY;U4.", "sequence ch5.script, label [Main], reached the end of label without jump action, pc is 22 size is 22\nPlease reset your system.");
    return SEQ_CONTINUE;
}

actionfuncres_t ddlcseq_sayori_d(std::vector<std::string>& argv){
    gamestatus.state_persistent.global_game_state = 1;
    engine_eeprom_save_persistent();
    engine_eeprom_delete_saves();
    bgm_play("d", false, 0.1f);
    sprite_t* background = sprite_load(filesystem_getfn(DIR_IMAGE, "d/s_kill_bg").c_str());
    sprite_t* foreground = sprite_load(filesystem_getfn(DIR_IMAGE, "sayori/s_kill.rgba32").c_str());
    sprite_t* monika     = sprite_load(filesystem_getfn(DIR_IMAGE, "monika/g1").c_str());
    sprite_t* yuri       = sprite_load(filesystem_getfn(DIR_IMAGE, "yuri/0b").c_str());
    //rspq_block_t* block = NULL;

    float time = 0; int state = 0;
    float scale_bg = 1.2f;
    float scale_fg = 1.3f;
    rdpq_blitparms_t parms_bg; parms_bg.cx = 320; parms_bg.cy = 240; parms_bg.filtering = true;
    rdpq_blitparms_t parms_fg; parms_fg.cx = 60; parms_fg.cy =  180; parms_fg.filtering = true;
    while (time < 4.35f){
        audioutils_mixer_update();
        if(time > 0){
            scale_bg -= 0.1 * 0.25 * display_get_delta_time();
            scale_fg -= 0.05 * display_get_delta_time();
        }
        if(time >= 4 && state == 0){state = 1; rspq_wait(); sprite_free(background); sprite_free(foreground); foreground = sprite_load(filesystem_getfn(DIR_IMAGE, "d/s_kill2").c_str()); background = sprite_load(filesystem_getfn(DIR_IMAGE, "d/s_kill_bg2").c_str()); sound_play("s_kill_glitch1", false); }
        parms_bg.scale_x = scale_bg; parms_bg.scale_y = scale_bg; 
        parms_fg.scale_x = scale_fg; parms_fg.scale_y = scale_fg; 
        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_standard();
        rdpq_mode_filter(FILTER_BILINEAR);
        rdpq_sprite_blit(background, 320,240, &parms_bg);
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(foreground, 180,180, &parms_fg);
        rdpq_detach_show();
        time += display_get_delta_time();
    }
    time = 0; state = 0;
    scale_bg = 2;
    scale_fg = 2;
    float alpha = 0;
    float noisealpha = 0;
    rspq_wait();
    sprite_free(background); sprite_free(foreground); 
    foreground = sprite_load(filesystem_getfn(DIR_IMAGE, "sayori/s_kill.rgba32").c_str()); background = sprite_load(filesystem_getfn(DIR_IMAGE, "d/s_kill_bg").c_str());
    while (time < 22){
        audioutils_mixer_update();
        if(noisealpha < 75) noisealpha += display_get_delta_time() * 25.0f;
        if(time >= 4 && state == 0) state = 1;
        if(time >= 8 && state == 1) {state = 2; rspq_wait(); sprite_free(background); scale_bg = 1.2f; background = sprite_load(filesystem_getfn(DIR_IMAGE, "bg/splash-glitch2").c_str()); sound_play("s_kill_glitch1", false); }
        if(time >= 10 && state == 2){state = 3; sound_play("s_kill_glitch1", false); }
        if(time >= 16 && state == 3){state = 4; rspq_wait(); sprite_free(background); scale_bg = 1.1f; background = sprite_load(filesystem_getfn(DIR_IMAGE, "d/s_kill_bg3").c_str()); sound_play("s_kill_glitch1", false); }
        scale_bg -= 0.005 * display_get_delta_time();
        scale_fg += 0.01 * display_get_delta_time();
        parms_bg.scale_x = scale_bg; parms_bg.scale_y = scale_bg; 
        parms_fg.scale_x = scale_fg; parms_fg.scale_y = scale_fg; 
        if(time > 18) alpha += display_get_delta_time() * 63;
        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_standard();
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_mode_filter(FILTER_BILINEAR);
        audioutils_mixer_update();
        rdpq_set_prim_color(RGBA32(noisealpha + sinf(time) * 50, noisealpha, noisealpha + sinf(time) * 50,150)); // add noise to grayscale images
        rdpq_mode_combiner(RDPQ_COMBINER1((NOISE,TEX0,PRIM,TEX0),(0,0,0,ENV)));
        rdpq_set_env_color(RGBA32(0,0,0, 100 + sinf(time) * 100));
        rdpq_set_fog_color(RGBA32(255,255,255,alpha));
        rdpq_set_blend_color(RGBA32(0,0,0,0));
        rdpq_mode_blender(RDPQ_BLENDER2((IN_RGB, IN_ALPHA, FOG_RGB, INV_MUX_ALPHA), (BLEND_RGB, FOG_ALPHA, CYCLE1_RGB, INV_MUX_ALPHA)));
        rdpq_sprite_blit(background, 320 + rand() % 2 ,240 + rand() % 2, &parms_bg);
        if(state == 3) rdpq_sprite_blit(yuri, rand() % 640 , rand() % 480, NULL);
        rdpq_mode_combiner(RDPQ_COMBINER1((NOISE,TEX0,PRIM,TEX0),(0,0,0,TEX0)));
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_mode_combiner(RDPQ_COMBINER1((NOISE,TEX0,PRIM,TEX0),(TEX0,0,PRIM,0)));
        rdpq_sprite_blit(foreground, 320 + rand() % 2 + sinf(time)*20, 360 + rand() % 2 + cosf(time/2)*20, &parms_fg);
        rdpq_sprite_blit(foreground, 320 + rand() % 2 + sinf(time + 0.7)*20, 360 + rand() % 2 + cosf(time/2 + 0.7)*20, &parms_fg);
        rdpq_set_mode_copy(true);
        if(state == 1) rdpq_sprite_blit(monika, rand() % 640 , rand() % 480, NULL);
        rdpq_detach_show();
        audioutils_mixer_update();
        time += display_get_delta_time();
    }
    rspq_wait();
    sprite_free(background);
    sprite_free(foreground);
    sprite_free(monika);
    sprite_free(yuri);

    bgm_play("d2", false, 0.1f);
    
    return SEQ_CONTINUE;
}

actionfuncres_t ddlcseq_sayori_d2(std::vector<std::string>& argv){
    sound_play("eyes", true);
    gamestatus.state_persistent.global_game_state = 2;
    engine_eeprom_save_persistent();
    engine_eeprom_delete_saves();
    sprite_t* background = sprite_load(filesystem_getfn(DIR_IMAGE, "d/eyes").c_str());
    surface_t bgsf = sprite_get_pixels(background);
    float offset = 0;
    float time = 0;
    while(true){
        time += display_get_delta_time();
        offset -= display_get_delta_time() * (350.0f * ((time / 45.0f) + 1));
        if(offset < -480) offset = 0;
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER1((NOISE,TEX0,PRIM,TEX0), (0,0,0,1)));
        if(rand() % 150 == 0) bgsf.flags = FMT_YUV16;
        else bgsf.flags = FMT_RGBA16;
        rdpq_set_prim_color(RGBA32(30 + sin(time/3)*20,30,20,50));
        rdpq_tex_blit(&bgsf, 0, 0 + offset, NULL);
        rdpq_tex_blit(&bgsf, 0, 480 + offset, NULL);
        rdpq_detach_show();
    }
}