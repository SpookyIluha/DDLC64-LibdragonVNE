# DDLC64-LibdragonVNE
An unofficial fan-port of the Doki Doki Literature Club to the Nintendo 64 using Libdragon SDK and a custom-made general purpose visual novel engine in C/C++. It is not affiliated with Team Salvato.

![boxart](https://github.com/user-attachments/assets/22796741-3315-45b4-b331-faed6675e823)

### It is suggested to play the official version first! Support the original developers at Team Salvato!

# Features:
- General purpose modding support via mirrored filesystem ddlc64_mod_folder on a flashcart
- Full 480i graphics at high framerates and full CD stereo quality soundtrack
- Includes English and Russian translations, other translations are possible via mods

# Disclaimer:
- This fan port to N64 is not affiliated with Team Salvato.
- It is suggested to play the official version first! Support the original developers!
- You can download official Doki Doki Literature Club at http://www.ddlc.moe
- This port comes with absolutely NO WARRANTY WHATSOEVER.

### This game is not suitable for children. And those who are easily disturbed.
### Individuals suffering from anxiety or depression may not have a safe experience playing this game. For content warnings, please visit http://www.ddlc.moe/warning.html
### By playing Doki Doki Literature Club, you agree that you are at least 13 years of age, and you consent to your exposure of highly disturbing content.

# Libdragon Visual Novel Engine Features:
- Automatic translations support for any game with a powerful translation system built into the engine itself
- Automatic mod support for any game with a mirrored filesystem in a game_mod_folder directory on flashcarts and emulators with SD card support
- Ready-made saving mechanics onto an EEPROM, including autosaves, manual saves and persistent saves
- Powerful scripting engine and .script files for narrative tropes, no coding required
- Customizable backgrounds, configs, cg images, composited character expressions and much more
- Extensive API for all the game features including audio and music, filesystem, scripts and images
- Multilanguage dictionary support for all the game's GUI elements
- Makefile script that automatically builds a .DSO game that can also be used as a mod for other Libdragon VNE games

# Specifics and characteristics of the N64 fan-port:
- the fan-port doesn't contain or ever intends to have any DDLC Plus content or features. Just the original game that is free to download.
- the fan-port only contains (assets and story-wise) the "First Run" part of the game (sayori scene included) as a self-contained story + a couple first scenes and scares from the next phase of the game as an epilogue.
- the game and a github page will contain all the precaution texts the original game has + a disclaimer about the port not being affiliated with Team Salvato and that it is suggested to play the original game first with links as per IP Guidelines. The disclaimer is the first screen after a logo the player sees and has to agree on upon each game boot.
- the fan-port properly credits the original team, shows the Team Salvato logo after the disclaimers.
- the game is made as a clean-room reimplementation, using C++ and Libdragon SDK without using any proprietary libraries or restrictive licenses.
Libdragon SDK (github page https://github.com/DragonMinded/libdragon) is a free clean open-source (Unlicense license) SDK for development on the N64.

- the github page of an DDLC N64 fan-port will not contain any assets (including text, narrative, sound/music, images/backgrounds etc.) of the original game and anyone that wants to build the game themselves would need to download the game and extract its assets.
- the release itself of the game is a .z64 ROM image that should only be played on an N64 system itself. Any assets that the game is built with are converted to Libdragon-specific formats, compressed and reduced in resolution/bitrate and will use different formatting, and are baked into the ROM image itself, meaning they are only used for their intended purpose of playing the game.

# Build the game
- Install Libdragon SDK (unstable)
- Clone this repository
- Add your DDLC assets from the Release to the "assets" or "filesystem" folder
- Build it with make command

# Add mods
-  Build the game with any changes you want
-  Copy the filesystem folder to the flashcart, rename it to ddlc64_mod_folder (or some other game name if you've changed it)
-  Launch the game from flashcart, you should see content from that mod folder and the main menu should say "Modded"

# Screenshots
![ddlc64 2025-04-10 00-36-17](https://github.com/user-attachments/assets/9d47028c-d9eb-49ae-a1ef-15bb4e481dab)
![ddlc64 2025-04-09 23-12-57](https://github.com/user-attachments/assets/b3d751de-e56f-4738-8be2-67a52d7d6061)
![ddlc64 2025-04-05 18-26-11](https://github.com/user-attachments/assets/218dd677-caf0-4516-9383-866dd3439d25)
![ddlc64 2025-04-05 18-18-27](https://github.com/user-attachments/assets/07b1717e-0c88-4f00-a7c2-24613a079db9)
![ddlc64 2025-04-05 15-17-46](https://github.com/user-attachments/assets/1c97be4c-7b61-4175-8735-e2fff32ce1aa)
![ddlc64 2025-04-05 16-13-17](https://github.com/user-attachments/assets/e946bc5f-6022-43ea-850c-02eb00837fec)
![ddlc64 2025-03-24 18-30-21](https://github.com/user-attachments/assets/886985d8-4d52-4b06-be10-4ce1ab46e8f4)
![ddlc64 2025-04-13 00-32-16](https://github.com/user-attachments/assets/9fc4db57-05eb-4f82-ad21-7411e8b4e835)

