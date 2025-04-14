#ifndef TYPES_H
#define TYPES_H

#include <unistd.h>
#include <time.h>
#include <libdragon.h>

// Various misc types for the engine

#ifdef __cplusplus
extern "C"{
#endif

typedef enum buffers_s{
    DOUBLE_BUFFERED = 2,
    TRIPLE_BUFFERED = 3,
    QUAD_BUFFERED = 4,
} buffers_t;

typedef struct{
    float x,y;
} pos2d;

/// @brief Sequence .script function return values
typedef enum{
    SEQ_CONTINUE, // continue the sequence without blocking (i.e setting a variable, changing backgrounds etc.)
    SEQ_CONTINUE_LINEJMP, // continue the sequence, but the PC has been changed (i.e after loading another .script), runtime has to update the state
    SEQ_ACTIONREQ, // action is required to progress (i.e a choice, or a dialogue box)
    SEQ_FROM_SAVE, // the game has just been started up from a save point, runtime has to update the state
    SEQ_RETURN_TO_MENU, // the whole game has been ended and its time to return to the main menu
} actionfuncres_t;

#ifdef __cplusplus
}
#endif

#endif