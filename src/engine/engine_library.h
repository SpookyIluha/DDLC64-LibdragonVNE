#ifndef ENGINE_LIBRARY_H
#define ENGINE_LIBRARY_H
/// Visual novel engine for Libdragon SDK, made by SpookyIluha
/// The default .script sequence function library of the engine. Contains all the default sequence functions

#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <libdragon.h>
#include "engine_logic.h"

/// @brief This is a list of custom functions that the .script files can access as a part of narrative, 
/// like saying, playing music, changing scenes, showing characters etc. 
/// the engine_library.h has a standard set of functions that can be used in .script files
/// .script files can use them by adding a line of [func_name] [argv0] [argv1]...
extern std::map<std::string, actionfunc_t> seqlibraryfuncs;
extern void seqlibrary_library_load();

// play, music/sound, fname
actionfuncres_t seqlibrary_play(std::vector<std::string>& argv);

// stop, music/sound
actionfuncres_t seqlibrary_stop(std::vector<std::string>& argv);

// autosave 0
actionfuncres_t seqlibrary_autosave(std::vector<std::string>& argv);

// char* filename
actionfuncres_t seqlibrary_scene(std::vector<std::string>& argv);

// const char* character, const char* expressionfn , const char* fn
actionfuncres_t seqlibrary_say(std::vector<std::string>& argv);

// const char* character, const char* expressionfn
actionfuncres_t seqlibrary_show(std::vector<std::string>& argv);

// const char* character
actionfuncres_t seqlibrary_hide(std::vector<std::string>& argv);

// bogus funcion
actionfuncres_t seqlibrary_with(std::vector<std::string>& argv);

// script change
actionfuncres_t seqlibrary_script(std::vector<std::string>& argv);

// script change
actionfuncres_t seqlibrary_label(std::vector<std::string>& argv);

// if statement
actionfuncres_t seqlibrary_if(std::vector<std::string>& argv);

// switch statement
actionfuncres_t seqlibrary_switch(std::vector<std::string>& argv);

// label change, name [label] name [label]...
actionfuncres_t seqlibrary_choice(std::vector<std::string>& argv);

// messagebox, "message"
actionfuncres_t seqlibrary_messagebox(std::vector<std::string>& argv);

// set the player's name, "message"
actionfuncres_t seqlibrary_set_player_name(std::vector<std::string>& argv);

// character + index + action + value
actionfuncres_t seqlibrary_variable(std::vector<std::string>& argv);

// autoskip 0
actionfuncres_t seqlibrary_autoskip(std::vector<std::string>& argv);

// percentage [label]
actionfuncres_t seqlibrary_chance(std::vector<std::string>& argv);

// return to main menu
actionfuncres_t seqlibrary_mainmenu(std::vector<std::string>& argv);

#endif