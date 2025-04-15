# Libdragon Visual Novel Engine Features:
- Automatic translations support for any game with a powerful translation system built into the engine itself
- Automatic mod support for any game with a mirrored filesystem in a game_mod_folder directory on flashcarts and emulators with SD card support
- Ready-made saving mechanics onto an EEPROM, including autosaves, manual saves and persistent saves
- Powerful scripting engine and .script files for narrative tropes, no coding required
- Customizable backgrounds, configs, cg images, composited character expressions and much more
- Extensive API for all the game features including audio and music, filesystem, scripts and images
- Multilanguage dictionary support for all the game's GUI elements
- Makefile script that automatically builds a .DSO game that can also be used as a mod for other Libdragon VNE games
- .INI configuration files support with C++ tortellini.hh library and other text files in the scripts folder

# How to make your own game or start a port:
- Create a directory with your game's name inside the src/ folder
- Edit the gamedef.mk and change the target game to compile to the name of your game
- Make sure that inside your game's folder there's an implementation of entrypoints void game_start(), void game_pause_menu() and void game_seqlibrary_customs_load() for the engine, there is an example of all of them in the example-game directory, use engine.h include for all the engine features to be usable
- game_start() will be called after the logo and language setup and should typically do a display_init() and display a main menu and intros if you have those, once the menu options has been selected, either call engine_newgame() or load a save and call engine_continue(), those functions will returns once the player selects to go to the main menu
- game_pause_menu() will be called inside the gameplay loop and should typically be implemented as a pause screen with some options like saving the game, continuing, or returning to the menu
- game_seqlibrary_customs_load() is a function to load all the custom sequence commands you game can use that can then be utilized in .script files like showing credits, minigames, screens, maps etc.
- Add all the necessary content to your game in the assets folder, such as narrative, music, screens, sounds effects, configs, translations etc. You can follow the standard folder structure that is of an example-game for ease of use, but you can change those in the config.ini and makefile file
- All the engine functions have descriptions in their header files

# Makefile
- the makefile will automatically scan the "assets" folder with all the image files, sounds and music, text and script files etc.
- you can adjust where it finds all files by modifing the makefile script

# Game assets folder structure

A typical folder structure is in the "assets" directory, and consists of the following:
- music "bgm" folder with .wav music files (will be compressed into Opus for the engine)
- sound effects "sfx" folder with .wav sound files (will be compressed into VADPCM for the engine)
- fonts folder that typically has subfolders for different translations such as en/de/fr/ru/jp etc. and have .ttf fonts inside (To control the glyph range, edit the assetflags.mk that includes the compile flags
  
- images folder which can have subfolders for backgrounds, characters, cg's, gui and menus (most should be in RGBA16 format with few exceptions):
    - intro folder is mandatory for the libdragon logo and cannot be changed
    - bg is the typical folder for the backgrounds (which are 640x480 RGBA16 images, or the same resolution your game will use)
    - cg folder is the same as bg, but for specific cg backgrounds that are meant to act as "rewards" for correct choices, and those will be tracked by the engine
    - gui is the folder that contains engine default images such as controller buttons, textboxes, choices, namebox etc.)
    - any other folder inside can contain any other .png that your game can reference in code through filesystem_getfn(folder, filename)
  
- scripts folder that has languages.ini and subfolders for defined languages such as en/de/fr/ru/jp etc.:
    - inside the language subfolders there should be:
        - config.ini,
        - dictionary.ini,
        - characters subfolder with all character .ini files
        - all the .script narrative files
        - any other text file that you may want to include and use in your game

# Text file definitions
### languages.ini:
  - inside Languages section is a list of languages as {country_code = "Language name"} the country code should correspond to a subfolder in the scripts directory
  - These language folders will be used by the engine to find all the subsequent text files inside language folders such as en/config.ini en/act0.script. etc.
### dictionary.ini:
  - inside Dictionary section is a list of key value pairs that can be used either for GUI text elements through the dictstr() function or general-purpose configurations
### config.ini:
  - a typical config.ini will look like this:
```
# everything regarding where the asset folders are located

[Data]

images        = "images" # default image folder
backgrounds   = "images/bg" # default backgrounds folder
cgbackgrounds = "images/cg" # default cg backgrounds folder
fonts         = "font/en" # default .font64 folder
music         = "bgm" # default music folder
sounds        = "sfx" # default sound effects folder

[Defaults]

# entrypoint .script of a new game
startupscript = "act0" # the .script file that will be played on the engine_newgame() call
startuplabel  = "[Main]" # the label inside the startupscript that will be played on the engine_newgame() call

# various default images for the engine/game
selectsound =           "select"                      # default select sound file
a_button_image =        "gui/button_a.rgba32"         # default A button image file
b_button_image =        "gui/button_b.rgba32"         # default B button image file
start_button_image =    "gui/button_start.rgba32"     # default Start button image file
textbox_image =         "gui/textbox"                 # default textbox image file that is displayed underneath dialogue
namebox_image =         "gui/namebox"                 # default textbox image file that is displayed underneath character name
overlay_frame =         "gui/frame"                   # default frame image file that is displayed whenever there's a messagebox appearance
button_idle =           "gui/button_idle"             # default button image file that is used in gameplay whenever there's a choice on screen
button_select =         "gui/button_select"           # default button image file that is used in gameplay whenever there's a choice on screen and is selected

[Scripts]

#show debug info on screen
debug = true

[Fonts]

#fonts and styles that will be used in GUI
mainfont = 1            # default font number that will be used on all dialogue, text and screens
mainfontstyle = 0       # default font style number that will be used on all dialogue, text and screens
titlefont = 2           # default font number that will be used on in title screens, character names
titlefontstyle = 2      # default font style number that will be used on in title screens, character names

#font data to load and assign 
# fonts should start with index 1, there should at least be 1 font and the font numbers must be continuous i.e font1 font2 font3 etc.
# font style colors should start with index 0, there should at least be 1 font style per font and the style numbers must be continuous i.e style0 style1 style2
# styles contain main color and outline color

font1 = "en_Nunito_main"
font1_color0text     = 0xFFFFFFFF
font1_color0outline  = 0x000000FF
```

### character .ini files:
  - a typical chatacter .ini will look like this: brew.ini
```
# character data
[Character]

name = "Brew"         # verbose character name that will appear in dialogue
shortname = "b"       # short name 1 that can be used to reference that chatacter in .script files or .cpp code
shortname2 = "brew"   # short name 2 that can be used to reference that chatacter in .script files or .cpp code
# there can only be 2 short names

# folders data
[Data]

images = "character_brew" # character folder relative to images folder that will contain all the character's image files

# image compositing from sources
[Composite]

1a = "l1 r1 base" # a list of filenames that are used to composite the expression inside the chatacter image folder, they should all be the same size
2a = "l2 r1 base"
3a = "l1 r2 base"
4a = "l2 r2 base"
5a = "l3 r1 base"
6a = "l3 r2 base"
# notice that we don't have 'dragon' compositing rule, as single images can also be just loaded within the engine
```
- there must also be a "player" character that represents the one playing the game or the protagonist of the story (can have a custom adjustable name)
must look like this:
```
# character data
[Character]

name = "player" # will be substituted with actual player's name
shortname = "mc"
```

# .script narrative language
.script file is pretty similar to a .rpy script, but does not contain any Python code at all, instead it is structed like so:
- for an example of .scripts files and their usage in games, see assets/scripts/en/ .script files
- .script files are made with .rpy porting in mind and can contain various logic elements, variable storing, calculation and referencing, and gamestate changes
```
[Main_Code_Block_Label]

# Comments are written like this, only from line start
    functionname1 argv0 argv1 argv2...
    functionname2 argv0 argv1 argv2...
    "Narrative"
    character_shortname "Character saying something"
    character_shortname expression_name "Character saying something with certain expression"
    ...
    label [Code_Block_Label2]

[Code_Block_Label2]
    functionname1 argv0 argv1 argv2...
    functionname2 argv0 argv1 argv2...
    ...
    label [Label]
```
- the function names and their arguments can be refernced from the engine_library.h that included standard narrative functions such as saying, showing characters, scenes etc.
- by default the functionname is "say" which is used for dialogue and narrative, so that its simpler and shorter to write

# Mod support
By default all VNE games will have mod support by simply mirroring the filesystem folder onto the $(GAME)_mod_folder inside the flashcard SD card
- The engine will scan the SD card on game bootup to see if it is modded, and if it is, the filesystem_getfn() will also try to search any files needed inside the mod folder
- The mod folder name is derived from the name of the game in the gamedef.mk
- Once you build the game, you can use the "filesystem" folder in the game directory as a mod for any other VNE game with the same engine version and settings
- the game itself is also compiled as a .DSO file that can be opened from the flashcart, it is optional to put the .DSO onto the flashcart
