#ifndef ENGINE_LOGIC_H
#define ENGINE_LOGIC_H
/// Visual novel engine for Libdragon SDK, made by SpookyIluha
/// Main logic code for the visual novel engine that includes setup and rendering

#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <libdragon.h>
#include "types.h"

typedef struct{
    char name[64];
    char shortname[64];
    char shortname2[64];

    char imagesfolder[64];
    int  entityindex;

    std::map<std::string, std::vector<std::string>> composites;
} character_t;

typedef actionfuncres_t(*actionfunc_t)(std::vector<std::string>&);

typedef class{
public:
    std::string functionname;
    std::vector<std::string> parameters;
    std::vector<const char*> parameters_cstr = std::vector<const char*>();
} action_t;

typedef std::vector<action_t> actionblock_t;

typedef std::map<std::string, actionblock_t> sequencefile_t;

// current sequence data
extern sequencefile_t cursequence;
// global character data
extern std::map<std::string, character_t> characters;
// max number of characters in the game
extern int maxcharacters;
// cg data
extern std::vector<std::string> cglist;

// get the amount of cg backgrounds in a game
size_t scripts_cglist_get_count();

// get the filename of a cg background at an index
std::string& scripts_cglist_get_filename_at(int index);

// get whether a cg background at index has been viewed by the player
bool scripts_cglist_get_viewed_flag(int index);

// get an index of the cg background by its filename "cg_bg1" for instance
int scripts_cglist_getindex(const char* cgfn);

/// @brief set whether a cg background has been viewed by the player
/// @param cgfn filename of the cg background
/// @param value true if viewed
void scripts_cglist_set_viewed_flag(const char* cgfn, bool value);

/// @brief Composite multiple same-size RGBA16 images into one surface
/// @param basefolder basefolder of the images to find
/// @param imagesfn an array of filenames "expr1 expr2 expr3 etc."
/// @return a surface of the composited image
surface_t scripts_composite_image(const char* basefolder, const std::vector<std::string>& imagesfn);

/// @brief Find a character by either its shortname or shortname2
/// @param charname shortname/shortname2 of the character
/// @return a chatacter reference or NULL if not found
character_t* scripts_find_character(const std::string& charname);

/// @brief Load a .script sequence file and set the program counter to it
/// @param fn filename of the .script
/// @param label the label code-block to set the PC at (def. [Main])
/// @param line the line of the code-block to set the PC at (def. 0)
void scripts_sequence_load_and_setlabel(const char *fn, const char *label, uint32_t line);

/// @brief Jump to a set label code-block within a current .script sequence
/// @param label the label code-block to set the PC at
void scripts_sequence_setlabel(const char *label);

/// the line of the code-block to set the PC at (def. 0)
void scripts_sequence_line(uint32_t line);

/// resolve a runtime state based by the gamestatus state (i.e after processing .script lines)
void resolve_dirty_flag();

/// start a new game, by loading a startup .script sequence and setting up a runtime environment, rendering a visual novel sequence
/// will return after calling a SEQ_RETURN_TO_MENU sequence library function
void engine_newgame();

/// continue the game with the current gamestatus, by loading a .script sequence and setting up a runtime environment, rendering a visual novel sequence
/// will return after calling a SEQ_RETURN_TO_MENU sequence library function
void engine_continuegame();

/// main engine loop
void engine_loop();

/// load a config.ini file into the gamestatus
void scripts_config_load();

/// free the runtime environment
void engine_gameruntfree();

#endif