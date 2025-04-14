#ifndef ENGINE_OVERLAYS_H
#define ENGINE_OVERLAYS_H
/// Visual novel engine for Libdragon SDK, made by SpookyIluha
/// Logic code to handle various overlays an engine has (choice, messagebox, setting a player's name)

#ifdef __cplusplus
extern "C"{
#endif

#include <vector>
#include <string>

typedef struct{
    struct {
        bool enabled;
        int  selectedchoice;
        std::vector<std::string> choices_labels;
        int  choicescount;
        int  choiceframe;
    } choiceoverlay;
    
} overlays_t;

extern overlays_t overlaystate;

// vector has to interleave names with script [labels], i.e: choice1 [label1] choice2 [label2]...
void overlays_activate_choice(std::vector<std::string>& choices_labels);

/// @brief update the state of the overlay
void overlays_update();
/// draw the overlay
void overlays_draw();

/// @brief display a blocking message onto the screen with A press to continue
/// @param msg message to show
void overlays_message(const char* msg);

/// @brief display a blocking message onto the screen with A press to continue and the player has to set their name
/// @param msg message to show when setting the name
void overlays_set_player_name(const char* msg);

#ifdef __cplusplus
}
#endif

#endif
