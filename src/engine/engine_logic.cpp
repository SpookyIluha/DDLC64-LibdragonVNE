#include <libdragon.h>
#include <fstream>
#include <map>
#include "audioutils.h"
#include "engine_gamestatus.h"
#include "tortellini.hh"
#include "engine_library.h"
#include "engine_overlays.h"
#include "engine_eeprom.h"
#include "engine_gfx.h"
#include "engine_filesystem.h"
#include "engine_logic.h"

// current sequence data
sequencefile_t cursequence = sequencefile_t();
// global character data
std::map<std::string, character_t> characters;
// max number of characters in the game
int maxcharacters = 0;
// cg data
std::vector<std::string> cglist;

int scripts_cglist_getindex(const char* cgfn){
    size_t index = 0; bool found = false;
    while(!found && index < cglist.size()){
        if(!strcmp(cglist[index].c_str(), cgfn)) found = true;
        else index++;
    } if(!found) return -1;
      else return index;
}

void scripts_cglist_set_viewed_flag(const char* cgfn, bool value){
    int index = scripts_cglist_getindex(cgfn);
    if(index < 0) return;
    uint64_t val = 1 << index;
    if(value) gamestatus.state_persistent.cg_unlocked_bitfield |= val;
    else      gamestatus.state_persistent.cg_unlocked_bitfield &= ~val;
}

bool scripts_cglist_get_viewed_flag(int index){
    uint64_t val = 1 << index;
    return gamestatus.state_persistent.cg_unlocked_bitfield & val;
}

std::string& scripts_cglist_get_filename_at(int index){
    return cglist[index];
}

size_t scripts_cglist_get_count(){
    return cglist.size();
}

int scripts_cglist_glob_handler(const char *fn, dir_t *dir, void *data){
    std::string name = std::string(fn);
    name = name.substr(name.find_last_of("/\\") + 1);
    size_t lastindex = name.find_last_of("."); 
    std::string rawname = name.substr(0, lastindex); 
    for(const std::string& n : cglist) if(!strcmp(n.c_str(), rawname.c_str())) return DIR_WALK_CONTINUE;
    cglist.push_back(rawname);
    if(gamestatus.state.scripts.debug) debugf("found cg: %s\n", rawname.c_str());
    return DIR_WALK_CONTINUE;
}

/// @brief Load a single .ini character data file in a global array of characters as a part of a glob process
/// @param fn Filename to load
/// @param dir directory of the file (not used)
/// @param data not used
/// @return continue the glob function
int scripts_characters_glob_handler(const char *fn, dir_t *dir, void *data) {
    debugf("found character: %s\n", fn);
    tortellini::ini ini;
	std::ifstream in(fn);
	in >> ini;

    std::string shortname = ini["Character"]["shortname"] | "unknown";
    std::string shortname2 = ini["Character"]["shortname2"] | "unknown";
    if(characters.count(shortname)) {
        debugf("character is duplicate: %s, skipping\n", shortname.c_str());
        return DIR_WALK_CONTINUE;
    }
    character_t chr = character_t();

    strcpy(chr.imagesfolder, ((ini["Data"]["images"] | "unknown")).c_str());
    strcpy(chr.name, ((ini["Character"]["name"] | "No Name")).c_str());
    strcpy(chr.shortname, shortname.c_str());
    strcpy(chr.shortname2, shortname2.c_str());
    chr.entityindex = maxcharacters;

    // iterate over all composite section
    tortellini::ini::section composites = ini["Composite"];
    // for each entry parse a list of image files into an array of strings
    for(auto& pair : composites._mapref){
        std::vector<std::string> imgs = std::vector<std::string>();
        std::istringstream iss = std::istringstream(pair.second);
        std::string s;
        while(getline( iss, s, ' ' )) {
            s.erase(remove( s.begin(), s.end(), '\"' ),s.end());
            imgs.push_back(s);
        }
        chr.composites[pair.first] = imgs;
    }
    
    characters[shortname] = chr;
    maxcharacters++;
    return DIR_WALK_CONTINUE;
}

/// @brief Load all .ini files for all the characters described in the game into a global array (rom://scripts/characters/*.ini)
void scripts_characters_load(){
    characters.clear();
    maxcharacters = 0;
    dir_glob("*.ini", (filesystem_getfolder(DIR_SCRIPT_LANG, true, false) + "characters/").c_str(), scripts_characters_glob_handler, NULL);
    dir_glob("*.ini", (filesystem_getfolder(DIR_SCRIPT_LANG, true, true) + "characters/").c_str(), scripts_characters_glob_handler, NULL);
    dir_glob("*.ini", (filesystem_getfolder(DIR_SCRIPT_LANG, false, false) + "characters/").c_str(), scripts_characters_glob_handler, NULL);
    dir_glob("*.ini", (filesystem_getfolder(DIR_SCRIPT_LANG, false, true) + "characters/").c_str(), scripts_characters_glob_handler, NULL);
}

void scripts_cglist_load(){
    cglist.clear();
    dir_glob("*.sprite", filesystem_getfolder(DIR_CG, true, false).c_str(), scripts_cglist_glob_handler, NULL);
    dir_glob("*.sprite", filesystem_getfolder(DIR_CG, true, true).c_str(), scripts_cglist_glob_handler, NULL);
    dir_glob("*.sprite", filesystem_getfolder(DIR_CG, false, false).c_str(), scripts_cglist_glob_handler, NULL);
    dir_glob("*.sprite", filesystem_getfolder(DIR_CG, true, true).c_str(), scripts_cglist_glob_handler, NULL);
}

/// @brief Load a default .ini configuration file with all the info on the game's structure (rom://scripts/config.ini)
void scripts_config_load(){
    tortellini::ini ini;

	// (optional) Read INI in from a file.
	// Default construction and subsequent assignment is just fine, too.
	std::ifstream in(filesystem_getfn(DIR_SCRIPT_LANG, "config.ini").c_str());
	in >> ini;

    strcpy(gamestatus.data.imagesfolder, ((ini["Data"]["images"] | "unknown")).c_str());
    strcpy(gamestatus.data.bgfolder, ((ini["Data"]["backgrounds"] | "unknown")).c_str());
    strcpy(gamestatus.data.cgfolder, ((ini["Data"]["cgbackgrounds"] | "unknown")).c_str());
    strcpy(gamestatus.data.fontfolder, ((ini["Data"]["fonts"] | "unknown")).c_str());
    strcpy(gamestatus.data.bgmfolder, ((ini["Data"]["music"] | "unknown")).c_str());
    strcpy(gamestatus.data.sfxfolder, ((ini["Data"]["sounds"] | "unknown")).c_str());
    
    strcpy(gamestatus.data.startupscript, (ini["Defaults"]["startupscript"] | "unknown").c_str());
    strcpy(gamestatus.data.startuplabel, ((ini["Defaults"]["startuplabel"] | "[Main]")).c_str());

    strcpy(gamestatus.data.defaults.selectsound, (ini["Defaults"]["selectsound"] | "select").c_str());
    strcpy(gamestatus.data.defaults.a_button_image, ((ini["Defaults"]["a_button_image"] | "unknown_a_button")).c_str());
    strcpy(gamestatus.data.defaults.b_button_image, (ini["Defaults"]["b_button_image"] | "unknown_b_button").c_str());
    strcpy(gamestatus.data.defaults.start_button_image, ((ini["Defaults"]["start_button_image"] | "unknown_start_button")).c_str());
    strcpy(gamestatus.data.defaults.textbox_image, (ini["Defaults"]["textbox_image"] | "unknown_textbox_img").c_str());
    strcpy(gamestatus.data.defaults.namebox_image, ((ini["Defaults"]["namebox_image"] | "unknown_namebox_img")).c_str());
    strcpy(gamestatus.data.defaults.overlay_frame, ((ini["Defaults"]["overlay_frame"] | "unknown_frame_img")).c_str());
    strcpy(gamestatus.data.defaults.button_idle, (ini["Defaults"]["button_idle"] | "unknown_buttonidle_img").c_str());
    strcpy(gamestatus.data.defaults.button_select, ((ini["Defaults"]["button_select"] | "unknown_buttonselect_img")).c_str());

    gamestatus.state.scripts.debug =  ini["Scripts"]["debug"] | false;

    gamestatus.fonts.mainfont       =  ini["Fonts"]["mainfont"]         | 1;
    gamestatus.fonts.mainfontstyle  =  ini["Fonts"]["mainfontstyle"]    | 0;
    gamestatus.fonts.mainfontselected  =  ini["Fonts"]["mainfontselected"]    | 0;
    gamestatus.fonts.titlefont      =  ini["Fonts"]["titlefont"]        | 1;
    gamestatus.fonts.titlefontstyle =  ini["Fonts"]["titlefontstyle"]   | 0;

    if(gamestatus.fonts.fontcount){
        for(int i = 1; i <= gamestatus.fonts.fontcount; i++){
            rdpq_text_unregister_font(i);
            if(gamestatus.fonts.fonts[i - 1]) {rdpq_font_free(gamestatus.fonts.fonts[i - 1]), gamestatus.fonts.fonts[i - 1] = NULL;}
        }
    }
    {
        int fontid = 1;
        char keyfont[128]; char keyfontcolor[128]; char keyfontcoloroutline[128];
        std::string font;
        do{
            sprintf(keyfont, "font%i", fontid);
            font = ini["Fonts"][keyfont] | "";
            if(!font.empty()){
                std::string fontfn = filesystem_getfn(DIR_FONT, (font).c_str());
                debugf("Font %i: %s\n", fontid,fontfn.c_str() );
                gamestatus.fonts.fonts[fontid - 1] = rdpq_font_load(fontfn.c_str());
                rdpq_text_register_font(fontid, gamestatus.fonts.fonts[fontid - 1]);
                gamestatus.fonts.fontcount = fontid;

                int styleid = 0;
                uint32_t color1 = 0x000000FE; uint32_t color2 = 0xFEFEFEFE;
                do{
                    sprintf(keyfontcolor, "font%i_color%itext", fontid, styleid);
                    sprintf(keyfontcoloroutline, "font%i_color%ioutline", fontid, styleid);
                    color1 = ini["Fonts"][keyfontcolor] | 0xFAFAFAFE;
                    color2 = ini["Fonts"][keyfontcoloroutline] | 0xFEFEFEFE;
                    debugf("Font %i Style %i: %08lx %08lx\n", fontid, styleid, color1, color2);
                    rdpq_fontstyle_t style; style.color = color_from_packed32(color1); style.outline_color = color_from_packed32(color2);
                    rdpq_font_style(gamestatus.fonts.fonts[fontid - 1], styleid, &style);
                    styleid++;
                } while (color1 != 0xFAFAFAFE);
            }

            fontid++;
        } while (!font.empty() && fontid < MAX_FONTS);
    }

    in.close();
}

/// @brief Load a text sequence file .script, overriding the current sequence instance
/// @param fn Filename to load
void scripts_sequence_load(const char *fn){
    rspq_wait();
    // clear the current sequence
    for(auto& [name, block] : cursequence){
        for(auto& action : block){
            action.functionname.clear();
            action.parameters.clear();
            action.parameters.clear();
        }
        block.clear();
    }
    cursequence.clear();
    debugf("loading sequence script: %s\n", fn);
    std::ifstream infile(fn);
    std::string line = std::string();

    actionblock_t curblock = actionblock_t();
    std::string curblockname = std::string();

    // read and parse the file line by line
    while (std::getline(infile, line))
    {
        std::stringstream ss(line);
        std::vector<std::string> words = std::vector<std::string>();
        std::string word = std::string();
        action_t action = action_t();

        // convert a line into token array (with quoted strings)
        while (ss >> std::quoted(word)) {
            words.push_back(word);
        }

        // parse the tokens
        std::string front; if(words.size() > 0) front = words.front();

        if(front.front() != '#') // comment line parse
        switch(words.size()){
            case 0: break;
            case 1: {
                // this there's only one word in a line, its either a label name, or a short way to "say" narrative
                bool islabel = (front.front() == '[');
                if(islabel){
                    // if its a label, add a new action block and set it as a new destination
                    if(!curblockname.empty()){
                        cursequence[curblockname] = curblock;
                        curblock = actionblock_t();
                        curblockname = front;
                    } else { curblockname = front; }
                } else {
                    // else just write is as a say function with one argument
                    action.functionname = "say";
                    action.parameters.push_back(front);
                    curblock.push_back(action);
                }
                break;
            }
            default: {
                // if this is a short way to say something, aka it starts with a shortname of a character, then set the function as "say"
                if(characters.count(front)){
                    action.functionname = "say";
                } else { // else its just a function call with parameters
                    action.functionname = front;
                    words.erase(words.begin());
                }
                action.parameters = words;
                curblock.push_back(action);
                break;
            }
        } 
        if(curblock.size()){
            curblock.back().parameters_cstr.clear();
            for(std::string& cstr : curblock.back().parameters) curblock.back().parameters_cstr.push_back(cstr.c_str());
        }
    }
    cursequence[curblockname] = curblock;
    std::string fnstr = std::string(fn);
    fnstr = fnstr.substr(fnstr.find_last_of("/\\") + 1);
    fnstr = fnstr.substr(0, fnstr.find_last_of("."));
    strcpy(gamestatus.state.seq.currentfile, fnstr.c_str());  

    // debug info printf
    if(gamestatus.state.scripts.debug){
        debugf("sequence script load complete: %s, blocks found %i\n\n", fn, cursequence.size());
        for(const auto &[name, block] : cursequence){
            debugf("Block: %s\n", name.c_str());
                int i = 0;
                for(const auto &action : block){
                    debugf("Action %i: %s, parms: ", i, action.functionname.c_str());
                    for(const auto &parm : action.parameters) debugf("| %s |", parm.c_str());
                    debugf("\n");
                    i++;
                }
        }
    }
}

void scripts_sequence_setlabel(const char *label){
    if(label != gamestatus.state.seq.currentlabel)
        strcpy(gamestatus.state.seq.currentlabel, label);
    gamestatus.state.seq.curline = 0;
    gamestatus.state.seq.lastresult = SEQ_CONTINUE_LINEJMP;
}

void scripts_sequence_line(uint32_t line){
    gamestatus.state.seq.curline = line;
    gamestatus.state.seq.lastresult = SEQ_CONTINUE_LINEJMP;
}

void scripts_sequence_load_and_setlabel(const char *fn, const char *label, uint32_t line){
    std::string fname = std::string(fn); fname += ".script";
    std::string fnamefull = filesystem_getfn(DIR_SCRIPT_LANG, fname.c_str() );
    scripts_sequence_setlabel(label);
    scripts_sequence_line(line);
    scripts_sequence_load(fnamefull.c_str());
}

typedef class{
public:
    sprite_t* background = NULL;
    sprite_t* namebox = NULL;
    sprite_t* textbox = NULL;
    sprite_t* button_a = NULL;
    rspq_block_t* backgroundblock = NULL;
    rspq_block_t* charactersblock = NULL;
    char curbackground[LONGSTR_LENGTH] = {0};

    struct{
        sprite_t* entsprite = {0};
        surface_t entspritesurf = {0};
        char expressionfn[SHORTSTR_LENGTH] = {0};
        bool active = false;
    } ents[MAX_CHARACTERS_LIMIT];
    int entsactive = 0;

    struct{
        int textcurlen = 0, textlen = 0;
        long long frame = 0;
        int curline = 0;
        const char* ptr = NULL;
    } dialogue;

    uint32_t seqline;
} runtime_t;
runtime_t RUNT = runtime_t();

void engine_newgame(){
    // load all the game and sequence data and start the sequence
    engine_gameruntfree();
    seqlibrary_library_load();
    scripts_characters_load();
    scripts_cglist_load();
    scripts_sequence_load_and_setlabel(gamestatus.data.startupscript, gamestatus.data.startuplabel, 0);
    engine_loop();
}

void engine_continuegame(){
    engine_gameruntfree();
        debugf("music %s\n", gamestatus.state.audio.bgmusic_name);
        if(gamestatus.state.audio.bgmusic_name[0]) bgm_play(gamestatus.state.audio.bgmusic_name, gamestatus.state.audio.loopingmusic, 0);
        else bgm_stop(0);
        // load all the game and sequence data and start the sequence
        seqlibrary_library_load();
        scripts_characters_load();
        scripts_cglist_load();
        scripts_sequence_load_and_setlabel(gamestatus.state.seq.currentfile, gamestatus.state.seq.currentlabel, gamestatus.state.seq.curline);
        gamestatus.state.seq.lastresult = SEQ_CONTINUE_LINEJMP;
        engine_loop();
}

surface_t scripts_composite_image(const char* basefolder, const std::vector<std::string>& imagesfn){
    if(gamestatus.state.scripts.debug){
        debugf("Composite vector (size %u): ", imagesfn.size());
        debugf("| %s |", imagesfn[0].c_str());}
    char fn[512] = {0}; sprintf(fn, "%s/%s", basefolder, imagesfn[0].c_str());
    sprite_t* input = sprite_load(filesystem_getfn(DIR_IMAGE, fn).c_str()); assertf(input, "Image couldn't load %s", fn);
    assertf(input->width && input->height, "Image has no dimentions %s", fn);
    surface_t output = surface_alloc(FMT_RGBA16, input->width, input->height);
    rdpq_attach(&output, NULL);
    rdpq_clear(RGBA32(0,0,0,0));
    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(input, 0,0, NULL);
    for(uint32_t i = 1; i < imagesfn.size(); i++){
        rspq_wait();
        sprite_free(input); input = NULL;
        sprintf(fn, "%s/%s", basefolder, imagesfn[i].c_str());
        if(gamestatus.state.scripts.debug) debugf("| %s |", imagesfn[i].c_str());
        input = sprite_load(filesystem_getfn(DIR_IMAGE, fn).c_str());
        rdpq_sprite_blit(input, 0,0, NULL);
    } if(gamestatus.state.scripts.debug)debugf("\n");
    rdpq_detach();
    rspq_wait();
    sprite_free(input); 
    return output;
}

character_t* scripts_find_character(const std::string& charname){
    character_t* chr = NULL;
    for(auto& ch : characters) if (!strcmp(ch.second.shortname2, charname.c_str())) chr = &ch.second;
    if(!chr) {if(characters.count(charname)) chr = &characters[charname];}
    assertf(chr, "Character not found %s", charname.c_str());
    return chr;
}

void game_background_transition(){
    float t = 1;

    while(t > 0 && RUNT.background){
        audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_set_prim_color(RGBA32((uint8_t)(t*250.0),(uint8_t)(t*250.0),(uint8_t)(t*250.0), 255));
        if(RUNT.backgroundblock) rspq_block_run(RUNT.backgroundblock);
        else rdpq_sprite_blit(RUNT.background, 0,0, NULL);
        rdpq_detach_show();
        t -= display_get_delta_time();
    }
    rspq_wait();
    if(RUNT.backgroundblock) {rspq_block_free(RUNT.backgroundblock); RUNT.backgroundblock = NULL;}
    if(RUNT.background) {sprite_free(RUNT.background); RUNT.background = NULL;}

    if(gamestatus.state.game.backgroundfn[0] != 0){
        char fn[512]; sprintf(fn , "%s.sprite", gamestatus.state.game.backgroundfn);
        RUNT.background = sprite_load(filesystem_getfn(DIR_ROOT, fn).c_str());
    }
    t = 0;
    while(t < 1){
            audioutils_mixer_update();
        rdpq_attach(display_get(), NULL);
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
        rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
        rdpq_set_prim_color(RGBA32((uint8_t)(t*250.0),(uint8_t)(t*250.0),(uint8_t)(t*250.0), 255));
        if(!RUNT.backgroundblock){
            rspq_block_begin();
            if(RUNT.background) rdpq_sprite_blit(RUNT.background, 0,0, NULL);
            else rdpq_clear(RGBA32(0,0,0,0));
            RUNT.backgroundblock = rspq_block_end();
        } rspq_block_run(RUNT.backgroundblock);
        rdpq_detach_show();
        t += display_get_delta_time();
    }
}

// reload data, backgrounds, characters as necessary per game state
void resolve_dirty_flag(){
    rspq_wait();

    if(RUNT.charactersblock) {rspq_block_free(RUNT.charactersblock); RUNT.charactersblock = NULL;}
    // background
    if(strcmp(RUNT.curbackground, gamestatus.state.game.backgroundfn)){
        game_background_transition();
        strcpy(RUNT.curbackground, gamestatus.state.game.backgroundfn);
    }

    // dialogue
    if(gamestatus.state.game.dialogue.line != RUNT.dialogue.curline){
        RUNT.dialogue.textcurlen = 0;
        RUNT.dialogue.frame = 0;
        RUNT.dialogue.curline = gamestatus.state.game.dialogue.line;

        RUNT.dialogue.ptr = (cursequence[gamestatus.state.seq.currentlabel])
        [RUNT.dialogue.curline].parameters[gamestatus.state.game.dialogue.lineidx].c_str();
        char dial[1024]; sprintf(dial, RUNT.dialogue.ptr, gamestatus.state.game.playername);
        RUNT.dialogue.textlen = strlen(dial);
    }

    RUNT.entsactive = 0; // characters
    if(RUNT.charactersblock) {rspq_block_free(RUNT.charactersblock); RUNT.charactersblock = NULL;}
    for(auto& character : characters){
        int i = character.second.entityindex;
        RUNT.ents[i].active = gamestatus.state.game.entities[i].active;
        rspq_wait();
        if(RUNT.ents[i].active) RUNT.entsactive++;
        else {
            if(RUNT.ents[i].entsprite) {sprite_free(RUNT.ents[i].entsprite); RUNT.ents[i].entsprite = NULL;}
            if(RUNT.ents[i].entspritesurf.buffer) {surface_free(&RUNT.ents[i].entspritesurf); RUNT.ents[i].entspritesurf.buffer = NULL; memset(&RUNT.ents[i].entspritesurf, 0, sizeof(surface_t));}
        }
        if(strcmp(RUNT.ents[i].expressionfn, gamestatus.state.game.entities[i].expressionfn)){
            strcpy(RUNT.ents[i].expressionfn, gamestatus.state.game.entities[i].expressionfn);
            rspq_wait();
            if(RUNT.ents[i].entsprite) {sprite_free(RUNT.ents[i].entsprite); RUNT.ents[i].entsprite = NULL;}
            if(RUNT.ents[i].entspritesurf.buffer) {surface_free(&RUNT.ents[i].entspritesurf); RUNT.ents[i].entspritesurf.buffer = NULL; memset(&RUNT.ents[i].entspritesurf, 0, sizeof(surface_t));}
            
            // if there's a composite image for the expression then composite it, else check if there's a direct file there and load it, otherwise error
            if(RUNT.ents[i].expressionfn[0] != 0){
                if(character.second.composites.count(RUNT.ents[i].expressionfn))
                    RUNT.ents[i].entspritesurf = scripts_composite_image(character.second.imagesfolder, character.second.composites[gamestatus.state.game.entities[i].expressionfn]);
                else{
                    char fn[512]; sprintf(fn, "%s/%s", character.second.imagesfolder, RUNT.ents[i].expressionfn);
                    FILE *f = fopen(filesystem_getfn(DIR_IMAGE, fn).c_str(), "rb"); 
                    if(f){
                        fclose(f);
                        RUNT.ents[i].entsprite = sprite_load(filesystem_getfn(DIR_IMAGE, fn).c_str());
                    } else{
                        if(gamestatus.state.scripts.debug)
                            debugf("No composite rule with this name found %s for character %s\n", RUNT.ents[i].expressionfn, character.second.name); 
                    }
                }
            }
        }
    }

    gamestatus.state.game.dirtyflag = false;
}

void game_update(){
    if(RUNT.dialogue.textcurlen < RUNT.dialogue.textlen) {
        RUNT.dialogue.textcurlen++;
    }
    RUNT.dialogue.frame++;
    joypad_poll();
    audioutils_mixer_update();


    // dialogue handling
    joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);

    if(!gamestatus.state.game.overlay_active){
        if(RUNT.dialogue.textcurlen >= RUNT.dialogue.textlen && (gamestatus.state.game.dialogue.autoskip || pressed.a)){
            gamestatus.state.seq.lastresult = SEQ_CONTINUE;
        } else if (RUNT.dialogue.frame > 3 && pressed.a) RUNT.dialogue.textcurlen = RUNT.dialogue.textlen;
        if (pressed.c_up && held.b) gamestatus.state.scripts.debug = !gamestatus.state.scripts.debug;
        if (held.z || held.l || held.r) RUNT.dialogue.textcurlen = RUNT.dialogue.textlen;
        if (RUNT.dialogue.frame > 3 && (held.z || held.l || held.r)) gamestatus.state.seq.lastresult = SEQ_CONTINUE;
    }
    if(pressed.start) engine_game_pause_menu();
    overlays_update();
}

void game_render(){
    rdpq_attach(display_get(), NULL);
    rdpq_set_mode_copy(true);
    if(!RUNT.backgroundblock){
        rspq_block_begin();

        if(RUNT.background) rdpq_sprite_blit(RUNT.background, 0,0, NULL);
        else rdpq_clear(RGBA32(0,0,0,0));
        RUNT.backgroundblock = rspq_block_end();
    } rspq_block_run(RUNT.backgroundblock);

    if(!RUNT.charactersblock){
        rspq_block_begin();
        rdpq_set_mode_copy(true);

        if(RUNT.entsactive > 0){
            int width = RUNT.textbox->width;
            int start = display_get_width() / 2 - width / 2;
            int incr = width / (RUNT.entsactive + 1);
            start += incr;
            for(int i = 0; i < MAX_CHARACTERS_LIMIT; i++){
                if(RUNT.ents[i].active) {
                    if(RUNT.ents[i].entsprite){
                        rdpq_sprite_blit_anchor(RUNT.ents[i].entsprite, ALIGN_CENTER, VALIGN_BOTTOM, start, display_get_height() - RUNT.textbox->height, NULL);
                    }
                    else if(RUNT.ents[i].entspritesurf.buffer) {
                        rdpq_tex_blit_anchor(&RUNT.ents[i].entspritesurf, ALIGN_CENTER, VALIGN_BOTTOM, start, display_get_height() - RUNT.textbox->height, NULL);
                    }
                    start += incr;
                }
            }
        }

        if(RUNT.dialogue.ptr && RUNT.textbox) {
            rdpq_sprite_blit_anchor(RUNT.textbox, ALIGN_CENTER, VALIGN_BOTTOM, display_get_width() / 2, display_get_height(), NULL);
        }
        RUNT.charactersblock = rspq_block_end();
    } rspq_block_run(RUNT.charactersblock);

    overlays_draw();

    if(RUNT.dialogue.ptr && RUNT.textbox){
        rdpq_textparms_t textparms = {0};
        textparms.width = RUNT.textbox->width - 30;
        textparms.height = RUNT.textbox->height - 30;
        textparms.align = ALIGN_LEFT; textparms.valign = VALIGN_TOP;
        textparms.wrap = WRAP_WORD;
        //textparms.preserve_overlap = true; // could be bad for performance
        textparms.max_chars = RUNT.dialogue.textcurlen;
        textparms.style_id = gamestatus.fonts.mainfontstyle;

        rdpq_textparms_t textparms_title = {0}; textparms_title.style_id = gamestatus.fonts.titlefontstyle;

        if(strlen(gamestatus.state.game.dialogue.name)){
            rdpq_sprite_blit_anchor(RUNT.namebox, ALIGN_LEFT, VALIGN_BOTTOM, display_get_width() / 2 - (RUNT.textbox->width / 2) + 15, display_get_height() - RUNT.textbox->height, NULL);
            rdpq_text_printf(&textparms_title, gamestatus.fonts.titlefont, display_get_width() / 2 - (RUNT.textbox->width / 2) + 25, display_get_height() - RUNT.textbox->height - 5, "%s", !strcmp(gamestatus.state.game.dialogue.name, "player")? gamestatus.state.game.playername : gamestatus.state.game.dialogue.name);
        }
        rdpq_text_printf(&textparms, gamestatus.fonts.mainfont, display_get_width() / 2 - (RUNT.textbox->width / 2) + 15, display_get_height() - RUNT.textbox->height + 15, RUNT.dialogue.ptr, gamestatus.state.game.playername);
        if(RUNT.dialogue.textcurlen >= RUNT.dialogue.textlen && RUNT.button_a && !gamestatus.state.game.overlay_active){
            rdpq_set_mode_standard();
            rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
            rdpq_sprite_blit_anchor(RUNT.button_a, ALIGN_RIGHT, VALIGN_BOTTOM, display_get_width() / 2 + (RUNT.textbox->width / 2), display_get_height(), NULL);
        }

    }
    if(gamestatus.state.scripts.debug){
        heap_stats_t stats;
        sys_get_heap_stats(&stats);
        rdpq_text_printf(NULL, gamestatus.fonts.mainfont, 30 , 30, "FPS: %.2f\nMEM: %i total, %i used\nscript %s, label %s, line %lu", display_get_fps(),
            stats.total,  stats.used, 
            gamestatus.state.seq.currentfile, gamestatus.state.seq.currentlabel, gamestatus.state.seq.curline);
    }

    rdpq_detach_show();
}


void engine_loop(){
    RUNT = runtime_t();
    if(!RUNT.textbox) RUNT.textbox = sprite_load(filesystem_getfn(DIR_IMAGE, gamestatus.data.defaults.textbox_image).c_str());
    if(!RUNT.namebox) RUNT.namebox = sprite_load(filesystem_getfn(DIR_IMAGE,  gamestatus.data.defaults.namebox_image).c_str());
    if(!RUNT.button_a) RUNT.button_a = sprite_load(filesystem_getfn(DIR_IMAGE,  gamestatus.data.defaults.a_button_image).c_str());
    RUNT.seqline = gamestatus.state.seq.curline;
    while(true){
        // update game state based on the seqeunce
        while(gamestatus.state.seq.lastresult == SEQ_CONTINUE || gamestatus.state.seq.lastresult == SEQ_CONTINUE_LINEJMP || gamestatus.state.seq.lastresult == SEQ_FROM_SAVE){
            if(gamestatus.state.seq.lastresult == SEQ_CONTINUE_LINEJMP) 
                 RUNT.seqline = gamestatus.state.seq.curline;
            else gamestatus.state.seq.curline = RUNT.seqline;

            assertf(RUNT.seqline < (cursequence[gamestatus.state.seq.currentlabel]).size(), "sequence %s, label %s, reached the end of label without jump action, pc is %lu size is %u\nPlease reset your system.", 
                gamestatus.state.seq.currentfile, gamestatus.state.seq.currentlabel, RUNT.seqline, (cursequence[gamestatus.state.seq.currentlabel]).size());

            action_t* action = &(cursequence[gamestatus.state.seq.currentlabel])[RUNT.seqline];
            assertf(seqlibraryfuncs.count(action->functionname) > 0, "sequence %s, label %s, action %lu: invalid action %s not found", 
                gamestatus.state.seq.currentfile, gamestatus.state.seq.currentlabel, gamestatus.state.seq.curline + 1, action->functionname.c_str());
            
            if(gamestatus.state.scripts.debug) {
                debugf("Action %lu: %s parms:", RUNT.seqline, action->functionname.c_str());
                for(const auto& str : action->parameters){
                    debugf("| %s |", str.c_str());
                } debugf("\n");
            }
            // sequence function call
            gamestatus.state.seq.lastresult = seqlibraryfuncs[action->functionname](action->parameters);
            if(gamestatus.state.seq.lastresult != SEQ_CONTINUE_LINEJMP) 
                RUNT.seqline++;
        }        

        // we've collected all the persistent state changes for the current line, now it needs for player's input 
        // (like pressing next on dialogue, or making a choice)
        while(!(gamestatus.state.seq.lastresult == SEQ_CONTINUE || gamestatus.state.seq.lastresult == SEQ_CONTINUE_LINEJMP)){
            if(gamestatus.state.game.dirtyflag)
                resolve_dirty_flag();
            
            game_update();
            game_render();
            if(gamestatus.state.seq.lastresult == SEQ_RETURN_TO_MENU){engine_gameruntfree(); return;}
        }
    }
}

void engine_gameruntfree(){
    // clear sequence data
    cursequence.clear();
    characters.clear();
    cglist.clear();
    maxcharacters = 0;
    RUNT.seqline = 0;

    if(RUNT.background) {sprite_free(RUNT.background); } RUNT.background = NULL;
    if(RUNT.namebox) {sprite_free(RUNT.namebox); } RUNT.namebox = NULL;
    if(RUNT.textbox) {sprite_free(RUNT.textbox); } RUNT.textbox = NULL;
    if(RUNT.button_a) {sprite_free(RUNT.button_a); } RUNT.button_a = NULL;

    if(RUNT.backgroundblock) {rspq_block_free(RUNT.backgroundblock); } RUNT.backgroundblock = NULL;
    if(RUNT.charactersblock) {rspq_block_free(RUNT.charactersblock); } RUNT.charactersblock = NULL;
    RUNT.curbackground[0] = 0;

    for(int i = 0; i < MAX_CHARACTERS_LIMIT; i++){
        if(RUNT.ents[i].entsprite) {sprite_free(RUNT.ents[i].entsprite); } RUNT.ents[i].entsprite = NULL;
        if(RUNT.ents[i].entspritesurf.buffer) {surface_free(&RUNT.ents[i].entspritesurf); memset(&RUNT.ents[i].entspritesurf, 0, sizeof(RUNT.ents[i].entspritesurf));}

        RUNT.ents[i].expressionfn[0] = 0;
        RUNT.ents[i].active = false;
    }

    RUNT.dialogue.textcurlen = 0;
    RUNT.dialogue.textlen = 0;
    RUNT.dialogue.curline = 0;
    RUNT.dialogue.frame = 0;
    RUNT.dialogue.ptr = NULL;
}