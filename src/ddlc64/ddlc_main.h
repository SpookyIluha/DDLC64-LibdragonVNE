#ifndef DDLC_MAIN_H
#define DDLC_MAIN_H

#ifdef __cplusplus
extern "C"{
#endif

/// @brief Main game entrypoint after engine is initialized
extern void game_start();
/// @brief Game's main menu
extern void game_menu();
/// @brief Pause menu
extern void game_pause_menu();
/// @brief Team Salvato Logo
extern void game_intro_logo();
/// @brief Consent screen
extern void game_intro_consent();
/// @brief A function to load all the custom sequence library commands into the engine, required by the engine to run
extern void game_seqlibrary_customs_load();

#ifdef __cplusplus
}
#endif

#endif