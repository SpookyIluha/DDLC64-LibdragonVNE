#include <libdragon.h>
#include "intro.h"
#include "engine_gamestatus.h"
#include "engine_logic.h"
#include "engine_eeprom.h"
#include "engine_filesystem.h"
#include "engine_setup.h"

/// Visual novel engine for Libdragon SDK, made by SpookyIluha

void systems_init(){
    debug_init_isviewer();
    debug_init_usblog();

    dfs_init(DFS_DEFAULT_LOCATION);
    asset_init_compression(3);
    wav64_init_compression(3);

    audio_init(48000, 6);
    mixer_init(8);

    rdpq_init();
    timer_init();
    joypad_init();
    timesys_init();
    srand(getentropy32());
    register_VI_handler((void(*)())rand);

    engine_eeprom_init();
    engine_eeprom_load_persistent();
    filesystem_init();
}

int main(void)
{
    systems_init();
    libdragon_logo();
    check_language_config();
    check_memory_expanded();
    engine_load_gamedso();
    engine_game_start();
}