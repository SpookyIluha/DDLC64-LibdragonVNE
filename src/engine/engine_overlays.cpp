#include <libdragon.h>
#include <vector>
#include <string>
#include "audioutils.h"
#include "engine_logic.h"
#include "engine_gamestatus.h"
#include "engine_filesystem.h"
#include "engine_gfx.h"
#include "engine_overlays.h"

overlays_t overlaystate;

sprite_t*  a_button_sprite;
sprite_t*  choice_sprite;
sprite_t*  choice_sprite_select;
rspq_block_t* overlayblock;

// vector has to interleave names with script [labels], i.e: choice1 [label1] choice2 [label2]...
void overlays_activate_choice(std::vector<std::string>& choices_labels){
    for (size_t i = 0; i < choices_labels.size(); i++) debugf("varchoice %s\n", choices_labels[i].c_str());
    int size = choices_labels.size() - 1;
    assert(size % 2 == 0 && size > 0);
    gamestatus.state.game.overlay_active = true;
    overlaystate.choiceoverlay.choices_labels = choices_labels;
    overlaystate.choiceoverlay.enabled = true;
    overlaystate.choiceoverlay.selectedchoice = 0;
    overlaystate.choiceoverlay.choiceframe = 0;
    overlaystate.choiceoverlay.choicescount = size / 2;
    if(!a_button_sprite)        a_button_sprite =       sprite_load(filesystem_getfn(DIR_IMAGE,gamestatus.data.defaults.a_button_image).c_str());
    if(!choice_sprite)          choice_sprite =         sprite_load(filesystem_getfn(DIR_IMAGE,gamestatus.data.defaults.button_idle ).c_str());
    if(!choice_sprite_select)   choice_sprite_select =  sprite_load(filesystem_getfn(DIR_IMAGE,gamestatus.data.defaults.button_select ).c_str());
}

void overlays_update(){
    if(!overlaystate.choiceoverlay.enabled) return;

    int* selection = &overlaystate.choiceoverlay.selectedchoice;
    int* count = &overlaystate.choiceoverlay.choicescount;
    int  oldsel = *selection;

    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(pressed.d_up)
        (*selection)--;
    if(pressed.d_down)
        (*selection)++;
    if((*selection) < 0) (*selection) = (*count) - 1;
    if((*selection) >= (*count)) (*selection) = 0;
    if((*selection) != oldsel) {
        sound_play(gamestatus.data.defaults.selectsound, false);
        if(overlayblock) rspq_block_free(overlayblock);
        overlayblock = NULL;
    }

    if(pressed.a && overlaystate.choiceoverlay.choiceframe > 5){
        overlaystate.choiceoverlay.enabled = false;
        gamestatus.state.game.overlay_active = false;
        sound_play(gamestatus.data.defaults.selectsound, false);
        if(a_button_sprite)  {sprite_free(a_button_sprite); a_button_sprite = NULL;}
        if(choice_sprite)   {sprite_free(choice_sprite); choice_sprite = NULL;}
        if(choice_sprite_select) {sprite_free(choice_sprite_select); choice_sprite_select = NULL;}
        if(overlayblock) rspq_block_free(overlayblock);
        overlayblock = NULL;
        scripts_sequence_setlabel(overlaystate.choiceoverlay.choices_labels[(*selection)*2 + 2].c_str());
        scripts_sequence_line(0);
        resolve_dirty_flag();
        gamestatus.state.seq.lastresult = SEQ_CONTINUE_LINEJMP;
        overlaystate.choiceoverlay.choices_labels.clear();
    }
    overlaystate.choiceoverlay.choiceframe++;
}

void overlays_draw(){
    if(!overlaystate.choiceoverlay.enabled) return;
    if(!overlayblock){
        rspq_block_begin();
        float cheight = 300;
        float width2 = display_get_width() / 2;
        float height2 = display_get_height() / 2;
        float start = height2 - cheight / 2;
        float incr = cheight / (overlaystate.choiceoverlay.choicescount + 1);
        start += incr;
        rdpq_textparms_t parms;
        parms.align = ALIGN_CENTER;
        parms.valign = VALIGN_CENTER;
        parms.width = choice_sprite->width;
        parms.height = choice_sprite->height;
        parms.style_id = gamestatus.fonts.mainfontstyle;
        for(int i = 0; i < overlaystate.choiceoverlay.choicescount; i++){
            parms.style_id = gamestatus.fonts.mainfontstyle;
            const char* name = overlaystate.choiceoverlay.choices_labels[(i*2) + 1].c_str();
            rdpq_set_mode_copy(true);
            if(i == overlaystate.choiceoverlay.selectedchoice){
                parms.style_id = gamestatus.fonts.mainfontselected;
                rdpq_sprite_blit_anchor(choice_sprite_select, ALIGN_CENTER, VALIGN_CENTER, width2, start, NULL);
                rdpq_set_mode_standard();
                rdpq_mode_combiner(RDPQ_COMBINER_TEX);
                rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
                rdpq_sprite_blit_anchor(a_button_sprite, ALIGN_RIGHT, VALIGN_CENTER, width2 - choice_sprite_select->width / 2, start, NULL);
            } else rdpq_sprite_blit_anchor(choice_sprite, ALIGN_CENTER, VALIGN_CENTER, width2, start, NULL);
            rdpq_text_printf(&parms, gamestatus.fonts.mainfont, width2 - choice_sprite->width / 2, start - choice_sprite->height / 2, name);
            start += incr;
        }
        overlayblock = rspq_block_end();
    } rspq_block_run(overlayblock);
}


void overlays_message(const char* msg){
    if(!a_button_sprite)
        a_button_sprite = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.a_button_image).c_str());

    sprite_t* frame = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.overlay_frame).c_str());
    bool skip = false;

    rdpq_textparms_t textparms;
    textparms.width = frame->width;
    textparms.height = frame->height;
    textparms.align = ALIGN_CENTER; 
    textparms.valign = VALIGN_CENTER;
    textparms.wrap = WRAP_WORD;
    textparms.style_id = gamestatus.fonts.mainfontstyle;

    while(!skip){
        audioutils_mixer_update();
        joypad_poll();
        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        if(pressed.a) {skip = true; sound_play(  gamestatus.data.defaults.selectsound, false);}

        rdpq_attach(display_get(), NULL);
        rdpq_clear(RGBA32(255,255,255,0));
        rdpq_set_mode_copy(true);
        rdpq_sprite_blit_anchor(frame, ALIGN_CENTER, VALIGN_CENTER, display_get_width() / 2, display_get_height() / 2, NULL );
        rdpq_text_printf(&textparms, gamestatus.fonts.mainfont, display_get_width() / 2 - frame->width / 2, display_get_height() / 2 - frame->height / 2, msg);
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(a_button_sprite, 580, 430, NULL);
        rdpq_detach_show();
    }
    rspq_wait();
    sprite_free(frame); frame = NULL;
    if(a_button_sprite)  {sprite_free(a_button_sprite); a_button_sprite = NULL;}
}


void overlays_set_player_name(const char* msg){
    if(!a_button_sprite)
        a_button_sprite = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.start_button_image).c_str());

    sprite_t* frame = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.overlay_frame).c_str());
    bool skip = false;

    rdpq_textparms_t textparms;
    textparms.width = frame->width;
    textparms.height = frame->height;
    textparms.align = ALIGN_CENTER; 
    textparms.valign = VALIGN_CENTER;
    textparms.wrap = WRAP_WORD;
    textparms.style_id = gamestatus.fonts.mainfontstyle;

    rdpq_textparms_t textparms2 = textparms;
    textparms2.height = 40;
    
    #define NAME_MAX_CHARS 12
    int nameidx[NAME_MAX_CHARS] = {0}; nameidx[0] = 13; nameidx[1] = 3; // "MC"
    std::string nameprint;
    std::string namesave;
    int selectedchar = 0;
    int frames = 0;

    while(!skip){
        frames++;
        audioutils_mixer_update();
        joypad_poll();
        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
        if(pressed.start) {skip = true; sound_play(gamestatus.data.defaults.selectsound, false);}

        if(pressed.d_left)   selectedchar--;
        if(pressed.d_right) selectedchar++;
        if(selectedchar < 0) selectedchar = NAME_MAX_CHARS - 1;
        if(selectedchar >= NAME_MAX_CHARS) selectedchar = 0;

        if(held.d_up && frames % 5 == 0) nameidx[selectedchar]--;
        if(held.d_down && frames % 5 == 0) nameidx[selectedchar]++;
        if(nameidx[selectedchar] < 0) nameidx[selectedchar] = 52;
        if(nameidx[selectedchar] >= 53) nameidx[selectedchar] = 0;

        nameprint.clear();
        namesave.clear();
        for(int i = 0; i < NAME_MAX_CHARS; i++){
            char letter = ' ';
            if(nameidx[i] >= 1 && nameidx[i] < 27) letter = 'A' + (nameidx[i] - 1); 
            if(nameidx[i] >= 27 && nameidx[i] < 53) letter = 'a' + (nameidx[i] - 27); 
            namesave += letter;
            if(letter == ' ' && i == selectedchar) letter = '_';
            if(i == selectedchar) nameprint += "^0" + std::to_string(gamestatus.fonts.titlefontstyle);
            nameprint += letter;
            if(i == selectedchar) nameprint += "^00";
        }

        rdpq_attach(display_get(), NULL);
        rdpq_clear(RGBA32(255,255,255,0));
        rdpq_set_mode_copy(true);
        rdpq_sprite_blit_anchor(frame, ALIGN_CENTER, VALIGN_CENTER, display_get_width() / 2, display_get_height() / 2, NULL );
        rdpq_text_printf(&textparms, gamestatus.fonts.mainfont, display_get_width() / 2 - frame->width / 2, display_get_height() / 2 - frame->height / 2, msg);
        rdpq_text_printf(&textparms2, gamestatus.fonts.titlefont, display_get_width() / 2 - frame->width / 2, display_get_height() / 2 + frame->height / 2 - 80, nameprint.c_str());
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
        rdpq_set_prim_color(RGBA32(0,0,0,255));
        rdpq_fill_rectangle(display_get_width() / 2 - 100, display_get_height() / 2 + frame->height / 2 - 50, display_get_width() / 2 + 100, display_get_height() / 2 + frame->height / 2 - 48);
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(a_button_sprite, 580, 430, NULL);
        rdpq_detach_show();
    }

    {
        int ch = NAME_MAX_CHARS - 1;
        while(namesave[ch] == ' '){
            namesave[ch] = '\0';
            ch--;
        } strcpy(gamestatus.state.game.playername, namesave.c_str());
    }

    rspq_wait();
    sprite_free(frame); frame = NULL;
    if(a_button_sprite)  {sprite_free(a_button_sprite); a_button_sprite = NULL;}
}