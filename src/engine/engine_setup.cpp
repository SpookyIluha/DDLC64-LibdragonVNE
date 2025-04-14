#include <libdragon.h>
#include "engine_filesystem.h"
#include "intro.h"

void check_memory_expanded(){
    if(!is_memory_expanded()){
        display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_DEDITHER);
        sprite_t* sprite = sprite_load(filesystem_getfn(DIR_IMAGE, "intro/expansionpak.rgba16").c_str());
        rdpq_textparms_t textparms; textparms.align = ALIGN_CENTER;
        textparms.style_id = gamestatus.fonts.titlefontstyle;
        textparms.width = display_get_width();
        while(true){
            rdpq_attach(display_get(), NULL);
            rdpq_set_mode_copy(false);
            rdpq_sprite_blit(sprite, 320 - 160, 120, NULL);
            rdpq_text_printf(&textparms, gamestatus.fonts.titlefont, 0, 40, dictstr("EXP_pak"));
            rdpq_detach_show();
        }
    }
}

void check_language_config(){
    engine_load_languages();
    assertf(languages.size(), "Game doesn't contain any language packs");
    if(engine_get_language()){
        debugf("Game language: %s\n", engine_get_language());
        scripts_config_load();
        engine_load_dictionary();
        return;
    }
    else{

        std::vector<const char*> languages_names;
        std::vector<const char*> languages_keys;
        // for each language add it to vector of strings
        for(auto& pair : languages){
            languages_keys.push_back(pair.first.c_str());
            languages_names.push_back(unquote(pair.second).c_str());
        }
        int selection = 0;
        bool isselected = false;
        engine_set_language(languages_keys[0]);
        scripts_config_load();
        engine_load_dictionary();
        display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_DEDITHER);

        while(!isselected){
            rdpq_textparms_t textparms; textparms.align = ALIGN_CENTER;
            textparms.width = display_get_width();
            joypad_poll();
            int oldsel = selection;
            joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
            if(pressed.d_up || pressed.c_up) selection --;
            if(pressed.d_down || pressed.c_down) selection ++;
            if(pressed.start || pressed.a) isselected = true;
            selection = selection % languages_keys.size();
            if(oldsel != selection){
                engine_set_language(languages_keys[selection]);
                scripts_config_load();
                engine_load_dictionary();
            }

            rdpq_attach(display_get(), NULL);
            rdpq_clear(RGBA32(0,0,0,0));
            textparms.style_id = gamestatus.fonts.titlefontstyle;
            rdpq_text_printf(&textparms, gamestatus.fonts.titlefont, 0, 80, dictstr("choose_lang"));
            rdpq_set_mode_standard();
            rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
            rdpq_set_prim_color(RGBA32(50,50,50,255));
            int y = 200;
            textparms.style_id = 0;
            textparms.height = 30;
            textparms.valign = VALIGN_CENTER;
            rdpq_fill_rectangle(0, y + 30*selection, display_get_width(), y + 30*selection + 30);
            for( const char* n : languages_names){
                rdpq_text_printf(&textparms, gamestatus.fonts.titlefont, 0, y, n);
                y += 30;
            }
            rdpq_detach_show();
        }

        rspq_wait();
        display_close();
    }
}