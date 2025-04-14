#ifndef ENGINE_GAMESTATUS_H
#define ENGINE_GAMESTATUS_H
/// Visual novel engine for Libdragon SDK, made by SpookyIluha
/// The global gamestatus variables that can be used at runtime and EEPROM saving

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

#define MAX_FONTS 32
#define MAX_CHARACTERS_LIMIT 8
#define SHORTSTR_LENGTH 32
#define LONGSTR_LENGTH 64
#define CHARACTER_MAX_VARS 8

#define STATE_MAGIC_NUMBER 0xBEEF
#define STATE_PERSISTENT_MAGIC_NUMBER 0xAEEC

typedef struct gamestate_s{
    unsigned short magicnumber;
    struct{
        bool bgmusic_playing;
        bool sound_playing;
        char bgmusic_name[SHORTSTR_LENGTH];
        char sound_name[SHORTSTR_LENGTH];
        float bgmusic_vol;
        float sound_vol;

        int transitionstate;
        float transitiontime;
        float transitiontimemax;
        bool loopingmusic;
    } audio;

    struct{
        char currentfile[LONGSTR_LENGTH];
        char currentlabel[SHORTSTR_LENGTH];
        uint32_t curline;
        actionfuncres_t lastresult;
    } seq;

    struct{
        char backgroundfn[LONGSTR_LENGTH];
        struct{
            int line, lineidx;
            char   name[SHORTSTR_LENGTH];
            bool   autoskip;
        } dialogue;

        struct {
            bool active;
            short variable[CHARACTER_MAX_VARS];
            char expressionfn[SHORTSTR_LENGTH];
        } entities[MAX_CHARACTERS_LIMIT];

        bool dirtyflag;
        bool overlay_active;
        char   playername[SHORTSTR_LENGTH];
    } game;

    struct{
        bool debug;
    } scripts;

} gamestate_t;

typedef enum{
    SAVE_NONE = 0,
    SAVE_AUTOSAVE,
    SAVE_MANUALSAVE
} saveenum_t;

typedef struct{
    unsigned short        magicnumber;
    uint64_t     cg_unlocked_bitfield; // unlocked cg's
    uint64_t     global_game_state; // game incomplete, complete, broken etc.
    saveenum_t   lastsavetype; // true - manual, false - autosave
    bool         autosaved;
    bool         manualsaved;
    char         curlang[SHORTSTR_LENGTH]; // "en" - english, etc.
    bool         modded;
} gamestate_persistent_t;

typedef struct gamestatus_s{
    double currenttime;
    double realtime;
    double deltatime;
    double deltarealtime;
    double fixeddeltatime;
    double fixedtime;
    double fixedframerate;

    double gamespeed;
    bool   paused;

    struct {
        char imagesfolder[SHORTSTR_LENGTH];
        char bgfolder[SHORTSTR_LENGTH];
        char cgfolder[SHORTSTR_LENGTH];
        char fontfolder[SHORTSTR_LENGTH];
        char bgmfolder[SHORTSTR_LENGTH];
        char sfxfolder[SHORTSTR_LENGTH];
        char scriptsfolder[SHORTSTR_LENGTH];
        char startupscript[LONGSTR_LENGTH];
        char startuplabel[SHORTSTR_LENGTH];

        struct{
            char selectsound[LONGSTR_LENGTH];
            char a_button_image[LONGSTR_LENGTH];
            char b_button_image[LONGSTR_LENGTH];
            char start_button_image[LONGSTR_LENGTH];
            char textbox_image[LONGSTR_LENGTH];
            char namebox_image[LONGSTR_LENGTH];
            char overlay_frame[LONGSTR_LENGTH];
            char button_idle[LONGSTR_LENGTH];
            char button_select[LONGSTR_LENGTH];
        } defaults;
    } data;

    struct{
        int mainfont;
        int mainfontstyle;
        int titlefont;
        int titlefontstyle;

        rdpq_font_t* fonts[MAX_FONTS];
        int fontcount;
    } fonts;

    gamestate_t state;
    gamestate_persistent_t state_persistent;

    float statetime;
} gamestatus_t;

/// @brief Global game state, includes a persistent state, a state for the game, state of the engine's datapoints
extern gamestatus_t gamestatus;

#define CURRENT_TIME            gamestatus.currenttime
#define CURRENT_TIME_REAL       gamestatus.realtime

#define GAMESPEED               gamestatus.gamespeed
#define GAME_PAUSED             gamestatus.paused

#define DELTA_TIME              gamestatus.deltatime
#define DELTA_TIME_REAL         gamestatus.deltarealtime

#define DELTA_TIME_FIXED        gamestatus.fixeddeltatime
#define CURRENT_TIME_FIXED      gamestatus.fixedtime

/// Init the global game state to 0
void timesys_init();

#ifdef __cplusplus
}
#endif

#endif