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
#include "engine_logic.h"
#include "engine_filesystem.h"

std::map<std::string, std::string> dictionary;
std::map<std::string, std::string> languages;

const char* modfolder = GAME_MOD_FOLDER;

void engine_load_dictionary(){
    rspq_wait();
    dictionary.clear();

    tortellini::ini ini;
	std::ifstream in(filesystem_getfn(DIR_SCRIPT_LANG, "dictionary.ini").c_str());
	in >> ini;
    // iterate over all Dictionary section
    tortellini::ini::section section = ini["Dictionary"];
    // for each entry add it to map of strings
    for(auto& pair : section._mapref){
        dictionary[pair.first] = unquote(pair.second);
    }
}

const char* dictstr(const char* name){
    if(dictionary.count(name))
        return dictionary[name].c_str();
    else return name;
}

void engine_load_languages(){
    languages.clear();

    tortellini::ini ini;
	std::ifstream in(filesystem_getfn(DIR_SCRIPT, "languages.ini").c_str());
	in >> ini;
    // iterate over all Dictionary section
    tortellini::ini::section section = ini["Languages"];
    // for each entry add it to map of strings
    for(auto& pair : section._mapref)
        languages[pair.first] = pair.second;
}

void engine_set_language(const char* lang){
    if(gamestatus.state.scripts.debug)
    debugf("Set language: %s\n", lang);
    strcpy(gamestatus.state_persistent.curlang, lang);
}

char* engine_get_language(){
    if(gamestatus.state_persistent.curlang[0]) return gamestatus.state_persistent.curlang;
    else return NULL;
}

filesystem_info_t filesysteminfo;

bool filesystem_ismodded(){
    return gamestatus.state_persistent.modded;
}

int filesystem_init_callback(const char *fn, dir_t *dir, void *data){
    debugf("Found mod directory %s\n", fn);
    strcpy(filesysteminfo.rootdir, fn);
    gamestatus.state_persistent.modded = true;
    return DIR_WALK_ABORT;
}

void filesystem_init(){
    if(debug_init_sdfs("sd:/", -1)){
        debugf("SD card initialized\n");
    } else debugf("SD card not found\n");
    dir_glob((std::string("**/") + modfolder).c_str(), "sd:/", filesystem_init_callback, NULL);
    if(!gamestatus.state_persistent.modded) 
        debugf("The game is not modded with %s directory\n", modfolder);
    strcpy(filesysteminfo.romrootdir, "rom:/");
}

bool filesystem_chkexist(const char* fn){
    FILE *f = fopen(fn, "r"); 
    if(f){
        fclose(f);
        if(gamestatus.state.scripts.debug)
            debugf("File: %s is found\n", fn);
        return true;}
    if(gamestatus.state.scripts.debug)
        debugf("File: %s does not exist\n", fn);
    return false;
}

std::string filesystem_getfolder(assetdir_t dir, bool modded, bool en_lang){
    std::string romfn = std::string(filesysteminfo.romrootdir);
    std::string sdfn = std::string(filesysteminfo.rootdir);

    std::string romfn_en = std::string(filesysteminfo.romrootdir);
    std::string sdfn_en = std::string(filesysteminfo.rootdir);

    switch(dir){
        case DIR_ROOT:
            romfn += "/" ;
            sdfn += "/" ;
            break;
        case DIR_SCRIPT:
            romfn += std::string("/") + gamestatus.data.scriptsfolder + "/" ;
            sdfn += std::string("/") + gamestatus.data.scriptsfolder + "/" ;
            break;
        case DIR_SCRIPT_LANG:
            romfn += std::string("/") + gamestatus.data.scriptsfolder + "/" + gamestatus.state_persistent.curlang + "/" ;
            sdfn += std::string("/") + gamestatus.data.scriptsfolder + "/" + gamestatus.state_persistent.curlang + "/" ;
            romfn_en += std::string("/") + gamestatus.data.scriptsfolder + "/en/" ;
            sdfn_en += std::string("/") + gamestatus.data.scriptsfolder + "/en/" ;
            break;
        case DIR_IMAGE:
            romfn += std::string("/") + gamestatus.data.imagesfolder + "/";
            sdfn += std::string("/") + gamestatus.data.imagesfolder + "/";
            break;
        case DIR_CG:
            romfn += std::string("/") + gamestatus.data.cgfolder + "/";
            sdfn += std::string("/") + gamestatus.data.cgfolder + "/";
            break;
        case DIR_BG:
            romfn += std::string("/") + gamestatus.data.bgfolder + "/";
            sdfn += std::string("/") + gamestatus.data.bgfolder + "/";
            break;
        case DIR_FONT:
            romfn += std::string("/") + gamestatus.data.fontfolder + "/";
            sdfn += std::string("/") + gamestatus.data.fontfolder + "/";
            break;
        case DIR_MUSIC:
            romfn += std::string("/") + gamestatus.data.bgmfolder + "/";
            sdfn += std::string("/") + gamestatus.data.bgmfolder + "/";
            break;
        case DIR_SOUND:
            romfn += std::string("/") + gamestatus.data.sfxfolder + "/";
            sdfn += std::string("/") + gamestatus.data.sfxfolder + "/";
            break;
    }
    if(en_lang && dir == DIR_SCRIPT_LANG) {
        if(modded) return sdfn_en;
        else return romfn_en;
    }
    if(modded) return sdfn;
    else return romfn;
}

std::string filesystem_getfn(assetdir_t dir, const char* name){
    std::string romfn = std::string(filesysteminfo.romrootdir);
    std::string sdfn = std::string(filesysteminfo.rootdir);

    std::string romfn_en = std::string(filesysteminfo.romrootdir);
    std::string sdfn_en = std::string(filesysteminfo.rootdir);

    switch(dir){
        case DIR_ROOT:
            romfn += std::string("/") + name;
            sdfn += std::string("/") + name;
            break;
        case DIR_SCRIPT:
            romfn += std::string("/") + gamestatus.data.scriptsfolder + "/" + name;
            sdfn += std::string("/") + gamestatus.data.scriptsfolder + "/" + name;
            break;
        case DIR_SCRIPT_LANG:
            romfn += std::string("/") + gamestatus.data.scriptsfolder + "/" + gamestatus.state_persistent.curlang + "/" + name;
            sdfn += std::string("/") + gamestatus.data.scriptsfolder + "/" + gamestatus.state_persistent.curlang + "/" + name;
            romfn_en += std::string("/") + gamestatus.data.scriptsfolder + "/" + "en" + "/" ;
            sdfn_en += std::string("/") + gamestatus.data.scriptsfolder + "/" + "en" + "/" ;
            break;
        case DIR_IMAGE:
            romfn += std::string("/") + gamestatus.data.imagesfolder + "/" + name + ".sprite";
            sdfn += std::string("/") + gamestatus.data.imagesfolder + "/" + name + ".sprite";
            break;
        case DIR_BG:
            romfn += std::string("/") + gamestatus.data.bgfolder + "/" + name + ".sprite";
            sdfn += std::string("/") + gamestatus.data.bgfolder + "/" + name + ".sprite";
            break;
        case DIR_CG:
            romfn += std::string("/") + gamestatus.data.cgfolder + "/" + name + ".sprite";
            sdfn += std::string("/") + gamestatus.data.cgfolder + "/" + name + ".sprite";
            break;
        case DIR_FONT:
            romfn += std::string("/") + gamestatus.data.fontfolder + "/" + name + ".font64";
            sdfn += std::string("/") + gamestatus.data.fontfolder + "/" + name + ".font64";
            break;
        case DIR_MUSIC:
            romfn += std::string("/") + gamestatus.data.bgmfolder + "/" + name + ".wav64";
            sdfn += std::string("/") + gamestatus.data.bgmfolder + "/" + name + ".wav64";
            break;
        case DIR_SOUND:
            romfn += std::string("/") + gamestatus.data.sfxfolder + "/" + name + ".wav64";
            sdfn += std::string("/") + gamestatus.data.sfxfolder + "/" + name + ".wav64";
            break;
    }

    if(filesystem_chkexist(sdfn.c_str())) return sdfn;
    if(filesystem_chkexist(sdfn_en.c_str())) return sdfn_en;
    if(filesystem_chkexist(romfn.c_str())) return romfn;
    if(filesystem_chkexist(romfn_en.c_str())) return romfn_en;
    
    if(gamestatus.state.scripts.debug)
        debugf("File %s does not exist in SD nor in ROM!\n", romfn.c_str());
    return romfn;
}

static void *game_ovl;

void (*game_start)() = NULL;
void (*game_pause_menu)() = NULL;
void (*e_seqlibrary_customs_load)() = NULL;

void engine_load_gamedso(){
    if(game_ovl){dlclose(game_ovl); game_ovl = NULL;}
    game_start = NULL;
    game_pause_menu = NULL;
    e_seqlibrary_customs_load = NULL;
    game_ovl = dlopen(filesystem_getfn(DIR_ROOT, GAME_DSO_FNAME).c_str(), RTLD_GLOBAL);

    game_start = (void (*)())dlsym(game_ovl, "game_start");
    game_pause_menu = (void (*)())dlsym(game_ovl, "game_pause_menu");
    e_seqlibrary_customs_load = (void (*)())dlsym(game_ovl, "game_seqlibrary_customs_load");
}

void engine_game_start(){
    assertf(game_start, "Game start function not found in game DSO");
    game_start();
}
void engine_game_pause_menu(){
    assertf(game_pause_menu, "Game menu function not found in game DSO");
    game_pause_menu();
}
void engine_game_loadcustomseqlibrary(){
    assertf(e_seqlibrary_customs_load, "Custom sequence library load function not found in game DSO");
    e_seqlibrary_customs_load();
}