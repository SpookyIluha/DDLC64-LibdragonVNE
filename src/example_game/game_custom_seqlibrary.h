#ifndef DDLC_CUSTOM_SEQLIBRARY_H
#define DDLC_CUSTOM_SEQLIBRARY_H

#include <libdragon.h>
#include "engine.h"


#ifdef __cplusplus
extern "C"{
#endif

/// @brief A function to load all the custom sequence library commands into the engine, required by the engine to run
void game_seqlibrary_customs_load();

#ifdef __cplusplus
}
#endif

/// @brief Show credits screen with all CG's that are unlocked
/// @param argv 0
/// @return asserts
actionfuncres_t customseq_credits(std::vector<std::string>& argv);

#endif