#include <libdragon.h>
#include "tortellini.hh"
#include "engine.h"
#include "game_custom_seqlibrary.h"

void game_seqlibrary_customs_load(){
    seqlibraryfuncs["custom_credits"] =   customseq_credits;
}

actionfuncres_t customseq_credits(std::vector<std::string>& argv){
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
    bgm_play("bgm1", true, 0.5f);
    time = 0;
    while(time < 10){
        float alpha = 255;
        if(time < 2) alpha = time * 127;
        if(time > 6) alpha = (10 - time) * 63;
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        rdpq_clear(RGBA32(0,0,0,0));
        rdpq_textparms_t textparms; textparms.align = ALIGN_CENTER;
        textparms.width = display_get_width();
        rdpq_text_printf(&textparms, gamestatus.fonts.titlefont, 0, display_get_height() / 2, end.c_str());
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
                    rdpq_set_env_color(RGBA32(0,0,0,0)); // add noise to grayscale images
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
    bgm_stop(0.5f);
    return SEQ_CONTINUE;
}