# An example game using LibdragonVNE
This simple game shows how to use the Libdragon visual novel engine through a simple demo showing its various features.

# Features:
- General purpose modding support via mirrored filesystem ddlc64_mod_folder on a flashcart
- Full 480i graphics at high framerates and full CD stereo quality soundtrack
- Includes English and Russian translations, other translations are possible via mods

![boxart](https://github.com/user-attachments/assets/3e6f661e-c4ad-41dd-9c77-cb86f17e80be)

# Libdragon Visual Novel Engine Features:
- Automatic translations support for any game with a powerful translation system built into the engine itself
- Automatic mod support for any game with a mirrored filesystem in a game_mod_folder directory on flashcarts and emulators with SD card support
- Ready-made saving mechanics onto an EEPROM, including autosaves, manual saves and persistent saves
- Powerful scripting engine and .script files for narrative tropes, no coding required
- Customizable backgrounds, configs, cg images, composited character expressions and much more
- Extensive API for all the game features including audio and music, filesystem, scripts and images
- Multilanguage dictionary support for all the game's GUI elements
- Makefile script that automatically builds a .DSO game that can also be used as a mod for other Libdragon VNE games

# Build the game
- Install Libdragon SDK (unstable)
- Clone this repository
- Build it with make command

# Add mods
-  Build the game with any changes you want
-  Copy the filesystem folder to the flashcart, rename it to example_game_mod_folder (or some other game name if you've changed it)
-  Launch the game from flashcart, you should see content from that mod folder and the main menu should say "Modded"

# Screenshots
![example_game 2025-04-15 00-17-22](https://github.com/user-attachments/assets/d39ad3f3-25d6-42b0-9da8-527da5b15f4e)
![example_game 2025-04-15 00-18-53](https://github.com/user-attachments/assets/472a3902-2f61-43ea-bc9d-c66ed18a0219)
![example_game 2025-04-15 00-18-19](https://github.com/user-attachments/assets/522ab27c-4557-4281-b6eb-d93d1012d191)
![example_game 2025-04-15 00-17-54](https://github.com/user-attachments/assets/ef563c3d-516e-4837-82f6-a8a65cb91b14)
![example_game 2025-04-15 00-17-44](https://github.com/user-attachments/assets/450008df-acb3-4a7a-ba3b-442cc9be3ff4)

