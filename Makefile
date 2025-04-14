BUILD_DIR = build
SOURCE_DIR = src
ENGINE_DIR = engine
include $(N64_INST)/include/n64.mk

include $(SOURCE_DIR)/gamedef.mk

SRCS = $(shell find $(SOURCE_DIR)/$(ENGINE_DIR)/ -type f -name '*.c') \
	   $(shell find $(SOURCE_DIR)/$(ENGINE_DIR)/ -type f -name '*.cpp')

SRCS_DSO = $(shell find $(SOURCE_DIR)/$(NAME)/ -type f -name '*.c') \
	       $(shell find $(SOURCE_DIR)/$(NAME)/ -type f -name '*.cpp')

IMAGE_LIST = $(shell find $(ASSETS_DIR)/images/ -type f -name '*.png')
FONT_LIST  = $(shell find $(ASSETS_DIR)/font/ -type f -name '*.ttf')
SOUND_LIST  = $(shell find $(ASSETS_DIR)/sfx/ -type f -name '*.wav')
MUSIC_LIST  = $(shell find $(ASSETS_DIR)/bgm/ -type f -name '*.wav')
SCRIPTS_LIST = $(shell find $(ASSETS_DIR)/scripts/ -type f -name '*.*')
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(IMAGE_LIST:%.png=%.sprite))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(FONT_LIST:%.ttf=%.font64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(SOUND_LIST:%.wav=%.wav64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(MUSIC_LIST:%.wav=%.wav64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(SCRIPTS_LIST))

include $(SOURCE_DIR)/$(NAME)/assetflags.mk

NAME_DSO = $(NAME)_game.dso
MAIN_ELF_EXTERNS := $(BUILD_DIR)/engine.externs
DSO_MODULES = $(NAME_DSO)
DSO_LIST = $(addprefix $(FILESYSTEM_DIR)/, $(DSO_MODULES))

N64_C_AND_CXX_FLAGS += -I $(SOURCE_DIR) -I $(SOURCE_DIR)/engine -I $(SOURCE_DIR)/$(NAME) -Wno-error=write-strings -Wno-error=narrowing -Wno-narrowing -Wno-write-strings -ftrivial-auto-var-init=zero -DGAME_MOD_FOLDER=\"$(NAME)_mod_folder\" -DGAME_DSO_FNAME=\"$(NAME_DSO)\"

$(FILESYSTEM_DIR)/images/%.sprite: $(ASSETS_DIR)/images/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) --compress 1 --dither ORDERED -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/font/%.font64: $(ASSETS_DIR)/font/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/sfx/%.wav64: $(ASSETS_DIR)/sfx/%.wav
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) --wav-compress 1 -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/bgm/%.wav64: $(ASSETS_DIR)/bgm/%.wav
	@mkdir -p $(dir $@)
	@echo "    [MUSIC] $@"
	$(N64_AUDIOCONV) --wav-compress 3 --wav-resample 24000 $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/scripts/%: $(ASSETS_DIR)/scripts/%
	@mkdir -p $(dir $@)
	@echo "    [SCRIPT] $@"
	cp "$<" $@

all: $(NAME).z64

$(BUILD_DIR)/$(NAME).dfs: $(ASSETS_LIST) $(DSO_LIST)
$(BUILD_DIR)/$(NAME).elf: $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o) $(SRCS:$(SOURCE_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(MAIN_ELF_EXTERNS)
$(MAIN_ELF_EXTERNS): $(DSO_LIST)

$(NAME).z64: N64_ROM_TITLE=$(ROM_NAME)
$(NAME).z64: $(BUILD_DIR)/$(NAME).dfs $(BUILD_DIR)/$(NAME).msym
$(NAME).z64: N64_ROM_SAVETYPE = eeprom16k

$(FILESYSTEM_DIR)/$(NAME_DSO): $(SRCS_DSO:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o) $(SRCS_DSO:$(SOURCE_DIR)/%.cpp=$(BUILD_DIR)/%.o)
$(BUILD_DIR)/$(NAME).msym: $(BUILD_DIR)/$(NAME).elf

clean:
	rm -rf $(BUILD_DIR) $(NAME).z64 $(FILESYSTEM_DIR) $(DSO_LIST)

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
