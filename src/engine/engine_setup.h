#ifndef ENGINE_SETUP_H
#define ENGINE_SETUP_H

#include <libdragon.h>
#include "intro.h"
/// Visual novel engine for Libdragon SDK, made by SpookyIluha
/// Low level setup code for languages and expansion pak

/// @brief Check if the system has an expansion pak installed and show a screen if it doesn't
void check_memory_expanded();

/// @brief Check the game's current set language and show a screen to set it up if it doesn't
void check_language_config();

#endif