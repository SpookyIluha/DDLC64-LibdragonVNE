#include <libdragon.h>
#include "engine_gamestatus.h"
#include "audioutils.h"
#include "engine_logic.h"
#include "engine_overlays.h"
#include "engine_filesystem.h"
#include "engine_library.h"
#include "engine_eeprom.h"

std::map<std::string, actionfunc_t> seqlibraryfuncs = std::map<std::string, actionfunc_t>();

void seqlibrary_library_load(){
    seqlibraryfuncs.clear();
    seqlibraryfuncs["play"]     = seqlibrary_play;
    seqlibraryfuncs["stop"]     = seqlibrary_stop;
    seqlibraryfuncs["scene"]    = seqlibrary_scene;
    seqlibraryfuncs["say"]      = seqlibrary_say;
    seqlibraryfuncs["show"]     = seqlibrary_show;
    seqlibraryfuncs["hide"]     = seqlibrary_hide;
    seqlibraryfuncs["with"]     = seqlibrary_with;
    seqlibraryfuncs["script"]   = seqlibrary_script;
    seqlibraryfuncs["label"]    = seqlibrary_label;
    seqlibraryfuncs["if"]       = seqlibrary_if;
    seqlibraryfuncs["switch"]   = seqlibrary_switch;
    seqlibraryfuncs["choice"]   = seqlibrary_choice;
    seqlibraryfuncs["variable"] = seqlibrary_variable;
    seqlibraryfuncs["autoskip"] = seqlibrary_autoskip;
    seqlibraryfuncs["autosave"] = seqlibrary_autosave;
    seqlibraryfuncs["mainmenu"] = seqlibrary_mainmenu;
    seqlibraryfuncs["chance"]   = seqlibrary_chance;
    seqlibraryfuncs["messagebox"] = seqlibrary_messagebox;
    seqlibraryfuncs["set_player_name"] = seqlibrary_set_player_name;
    
    engine_game_loadcustomseqlibrary();
}

actionfuncres_t seqlibrary_play(std::vector<std::string>& argv){
    assert(argv.size() >= 2);
    bool music = false; if(!strcmp(argv[0].c_str(), "music")) music = true;

    if(music) bgm_play(argv[1].c_str(), true, 1);
    else sound_play(argv[1].c_str(), false);

    return SEQ_CONTINUE;
}

actionfuncres_t seqlibrary_stop(std::vector<std::string>& argv){
    assert(argv.size() >= 1);
    bool music = false; if(!strcmp(argv[0].c_str(), "music")) music = true;

    if(music) bgm_stop(1);
    else sound_stop();
        
    return SEQ_CONTINUE;
}

actionfuncres_t seqlibrary_autosave(std::vector<std::string>& argv){
    engine_eeprom_save_autosave();
    return SEQ_CONTINUE;
}

// char* filename
actionfuncres_t seqlibrary_scene(std::vector<std::string>& argv){
    assert(argv.size() >= 1);

    gamestatus.state.game.dirtyflag = true;
    for(int i = 0; i <  MAX_CHARACTERS_LIMIT; i++){
        gamestatus.state.game.entities[i].active = false;
    }

    if(argv.size() == 1) {
        if(!strcmp(argv[0].c_str(), "none")) 
            memset(gamestatus.state.game.backgroundfn, 0, sizeof(gamestatus.state.game.backgroundfn));
        return SEQ_CONTINUE;
    }

    const char* folder = gamestatus.data.cgfolder;
    if(!strcmp("bg", argv[0].c_str())) folder = gamestatus.data.bgfolder;
    else scripts_cglist_set_viewed_flag(argv[1].c_str(), true);
    sprintf(gamestatus.state.game.backgroundfn, "%s/%s", folder, argv[1].c_str());
        
    return SEQ_CONTINUE;
}


actionfuncres_t seqlibrary_autoskip(std::vector<std::string>& argv){
    assert(argv.size() == 1);
    if(!strcmp(argv[0].c_str(), "true")) gamestatus.state.game.dialogue.autoskip = true;
    else gamestatus.state.game.dialogue.autoskip = false;
    return SEQ_CONTINUE;
}

// bogus for now
actionfuncres_t seqlibrary_with(std::vector<std::string>& argv){
    return SEQ_CONTINUE;
}

// script change
actionfuncres_t seqlibrary_script(std::vector<std::string>& argv){
    assert(argv.size() > 0);
    switch(argv.size()){
        case 1: {scripts_sequence_load_and_setlabel(argv[0].c_str(), "[Main]", 0); break;}
        case 2: {scripts_sequence_load_and_setlabel(argv[0].c_str(), argv[1].c_str(), 0); break;}
        case 3: {scripts_sequence_load_and_setlabel(argv[0].c_str(), argv[1].c_str(), atoi(argv[2].c_str())); break;}
        default: assertf(0, "sequence: script, wrong argument count");
    }
    return SEQ_CONTINUE_LINEJMP;
}

// if [char index] [>] [char index] then [label] else [label]
// char index can be replaced with [value number]
actionfuncres_t seqlibrary_if(std::vector<std::string>& argv){
    assert(argv.size() == 7 || argv.size() == 9);

    typedef enum{
        EQUAL,
        NOT_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
    } ifact_t;

    std::string& characterA = argv[0];
    int         variableindexA = atoi(argv[1].c_str());
    short       variableA = 0;

    std::string& characterB = argv[3];
    int         variableindexB = atoi(argv[4].c_str());
    short       variableB = 0;

    ifact_t     ifact = EQUAL;
    const char* ifactstr = argv[2].c_str();
    if      (!strcmp(ifactstr, "=="))  ifact = EQUAL;
    else if (!strcmp(ifactstr, "!="))  ifact = NOT_EQUAL;
    else if (!strcmp(ifactstr, ">"))   ifact = GREATER;
    else if (!strcmp(ifactstr, ">="))  ifact = GREATER_EQUAL;
    else if (!strcmp(ifactstr, "<"))   ifact = LESS;
    else if (!strcmp(ifactstr, "<="))  ifact = LESS_EQUAL;

    const char* labelTrue = argv[6].c_str();
    const char* labelFalse = NULL;
    if(argv.size() == 9) labelFalse = argv[8].c_str();

    character_t* chrA = NULL;
    if(strcmp(characterA.c_str(), "value")) {chrA = scripts_find_character(characterA);}
    if(chrA) {variableA = gamestatus.state.game.entities[chrA->entityindex].variable[variableindexA];}
    else {variableA = variableindexA;}

    character_t* chrB = NULL;
    if(strcmp(characterB.c_str(), "value")) {chrB = scripts_find_character(characterB);}
    if(chrB) {variableB = gamestatus.state.game.entities[chrB->entityindex].variable[variableindexB];}
    else {variableB = variableindexB;}

    bool exprTrue = false;

    switch(ifact){
        case EQUAL:         {exprTrue = (variableA == variableB); break;}
        case NOT_EQUAL:     {exprTrue = (variableA != variableB); break;}
        case GREATER:       {exprTrue = (variableA >  variableB); break;}
        case GREATER_EQUAL: {exprTrue = (variableA >= variableB); break;}
        case LESS:          {exprTrue = (variableA <= variableB); break;}
        case LESS_EQUAL:    {exprTrue = (variableA <= variableB); break;}
    }

    if(exprTrue){
        scripts_sequence_setlabel(labelTrue);
        return SEQ_CONTINUE_LINEJMP;
    } else if(labelFalse){
        scripts_sequence_setlabel(labelFalse);
        return SEQ_CONTINUE_LINEJMP;
    } else return SEQ_CONTINUE;
}

// switch [char index] [number] [label] [number] [label] ...
actionfuncres_t seqlibrary_switch(std::vector<std::string>& argv){
    assert(argv.size() > 0);

    character_t* chrA = scripts_find_character(argv[0]);
    int          variableindexA = atoi(argv[1].c_str());
    short        variableA = gamestatus.state.game.entities[chrA->entityindex].variable[variableindexA];

    size_t i = 2;
    while(i < argv.size()){
        short variableB = atoi(argv[i].c_str());
        const char* labelB = argv[i+1].c_str();
        if(variableA == variableB) {
            scripts_sequence_setlabel(labelB);
            return SEQ_CONTINUE_LINEJMP;
        }
        i+=2;
    }

    return SEQ_CONTINUE;
}

// label change
// label or label + line
actionfuncres_t seqlibrary_label(std::vector<std::string>& argv){
    assert(argv.size() > 0);
    switch(argv.size()){
        case 1: {scripts_sequence_setlabel(argv[0].c_str()); scripts_sequence_line(0); break;}
        case 2: {scripts_sequence_setlabel(argv[0].c_str()); scripts_sequence_line(atoi(argv[1].c_str())); break;}
        default: assertf(0, "sequence: label, wrong argument count");
    }
    return SEQ_CONTINUE_LINEJMP;
}

// label change: "dialogue" name [label] name [label]...
actionfuncres_t seqlibrary_choice(std::vector<std::string>& argv){
    assert(argv.size() > 0);
    gamestatus.state.game.dialogue.line = gamestatus.state.seq.curline;
    strcpy(gamestatus.state.game.dialogue.name, "");
    gamestatus.state.game.dialogue.lineidx = 0;
    overlays_activate_choice(argv);
    gamestatus.state.game.dirtyflag = true;
    return SEQ_ACTIONREQ;
}

actionfuncres_t seqlibrary_messagebox(std::vector<std::string>& argv){
    assert(argv.size() == 1);
    overlays_message(argv[0].c_str());
    return SEQ_CONTINUE;
}

actionfuncres_t seqlibrary_set_player_name(std::vector<std::string>& argv){
    assert(argv.size() == 1);
    overlays_set_player_name(argv[0].c_str());
    return SEQ_CONTINUE;
}

// return to main menu
actionfuncres_t seqlibrary_mainmenu(std::vector<std::string>& argv){
    return SEQ_RETURN_TO_MENU;
}

// percentage [label]
actionfuncres_t seqlibrary_chance(std::vector<std::string>& argv){
    assert(argv.size() == 2);
    int percentage = atoi(argv[0].c_str());
    if(rand() % 100 <= percentage){
        scripts_sequence_setlabel(argv[1].c_str()); 
        scripts_sequence_line(0);
        return SEQ_CONTINUE_LINEJMP;
    } else 
    return SEQ_CONTINUE;
}

// character + index + action + value
actionfuncres_t seqlibrary_variable(std::vector<std::string>& argv){
    typedef enum{
        SET,
        PLUS,
        MINUS,
        INVALID,
    } varact_t;

    assert(argv.size() == 4);
    
    const std::string& charname = argv[0];
    int index = atoi(argv[1].c_str());
    const char* action = argv[2].c_str();
    int value = atoi(argv[3].c_str());

    character_t* chr = scripts_find_character(charname);

    int chrindex = chr->entityindex;
    varact_t act = INVALID; 
    if(!strcmp(action, "set")) act = SET;
    else if(!strcmp(action, "incr")) act = PLUS;
    else if(!strcmp(action, "decr")) act = MINUS;

    switch(act){
        case SET: gamestatus.state.game.entities[chrindex].variable[index] = value; break;
        case PLUS: gamestatus.state.game.entities[chrindex].variable[index] += value; break;
        case MINUS: gamestatus.state.game.entities[chrindex].variable[index] -= value; break;
        default: assertf(0, "invalid variable action %i", act);
    }

    return SEQ_CONTINUE;
}

// const char* character, const char* expressionfn , const char* fn
actionfuncres_t seqlibrary_say(std::vector<std::string>& argv){
    assert(argv.size() >= 1);
    gamestatus.state.game.dialogue.line = gamestatus.state.seq.curline;
    switch(argv.size()){
        // narrative
        case 1:{
            strcpy(gamestatus.state.game.dialogue.name, "");
            gamestatus.state.game.dialogue.lineidx = 0;
            break;
        }
        // ch + narrative
        case 2:{
            strcpy(gamestatus.state.game.dialogue.name, characters[argv[0]].name);
            gamestatus.state.game.dialogue.lineidx = 1;
            break;
        }
        // ch + expr + narrative
        case 3:{
            strcpy(gamestatus.state.game.entities[scripts_find_character(argv[0])->entityindex].expressionfn, argv[1].c_str());
            strcpy(gamestatus.state.game.dialogue.name, characters[argv[0]].name);
            gamestatus.state.game.dialogue.lineidx = 2;
            break;
        }
    }
    gamestatus.state.game.dirtyflag = true;
        
    return SEQ_ACTIONREQ;
}   

// const char* character, const char* expressionfn
actionfuncres_t seqlibrary_show(std::vector<std::string>& argv){
    assert(argv.size() >= 2);

    character_t* chr = scripts_find_character(argv[0]);

    int id = chr->entityindex;
    gamestatus.state.game.entities[id].active = true;  
    if(strcmp(argv[1].c_str(), "at") && strcmp(argv[1].c_str(), "zorder") && strcmp(argv[1].c_str(), "behind"))
        strcpy(gamestatus.state.game.entities[id].expressionfn, argv[1].c_str());

    gamestatus.state.game.dirtyflag = true;
        
    return SEQ_CONTINUE;
}

// const char* character
actionfuncres_t seqlibrary_hide(std::vector<std::string>& argv){
    assert(argv.size() == 1);

    character_t* chr = scripts_find_character(argv[0]);

    int id = chr->entityindex;
    gamestatus.state.game.entities[id].active = false;

    gamestatus.state.game.dirtyflag = true;
        
    return SEQ_CONTINUE;
}