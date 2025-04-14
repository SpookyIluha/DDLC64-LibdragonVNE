#ifndef DDLC_CUSTOM_SEQLIBRARY_H
#define DDLC_CUSTOM_SEQLIBRARY_H

#include <libdragon.h>
#include "engine.h"
#include "ddlc_poem_game.h"


#ifdef __cplusplus
extern "C"{
#endif

/// @brief A function to load all the custom sequence library commands into the engine, required by the engine to run
void game_seqlibrary_customs_load();

#ifdef __cplusplus
}
#endif

/// @brief Activate a poem minigame
/// @param argv [0] minigame state (0 - show tutorial)
/// @return CONTINUE
actionfuncres_t ddlcseq_poem_minigame(std::vector<std::string>& argv);

/// @brief Show a character's poem
/// @param argv [0] filename of the poem
/// @return CONTINUE
actionfuncres_t ddlcseq_showpoem(std::vector<std::string>& argv);

/// @brief Show credits screen with all CG's that are unlocked
/// @param argv 0
/// @return asserts
actionfuncres_t ddlcseq_credits(std::vector<std::string>& argv);

/// @brief Sayori scene #1 (spoilers)
/// @param argv 0
/// @return CONTINUE
actionfuncres_t ddlcseq_sayori_d(std::vector<std::string>& argv);

/// @brief Sayori scene #2 (spoilers)
/// @param argv 0
/// @return CONTINUE
actionfuncres_t ddlcseq_sayori_d2(std::vector<std::string>& argv);

#endif