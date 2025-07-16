BUILD_DIR=build
T3D_INST=$(shell realpath ../..)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/workspace/tiny3d/t3d.mk

N64_CFLAGS += -std=gnu2x -O2

PROJECT_NAME=mightydoom64

src = $(wildcard *.c) 
assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_ttf = $(wildcard assets/*.ttf)
#assets_xm = $(wildcard assets/*.xm) # new code for music
assets_wav = $(wildcard assets/*.wav) # new code for music

assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm))) \
			  $(addprefix filesystem/,$(notdir $(assets_wav:%.wav=%.wav64))) # new code for music
# 			  $(addprefix filesystem/,$(notdir $(assets_xm))) # new code for music

all: $(PROJECT_NAME).z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

# Font 
filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -s 18 -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -w 256 -o filesystem $@

# new code for music
#filesystem/%.xm: assets/%.xm
#	@mkdir -p $(dir $@)
#	@echo "    [XM] $@"
#	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

filesystem/%.wav64: assets/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	@$(N64_AUDIOCONV) --wav-compress 1 --wav-resample 22050 -o filesystem "$<"
# new code for music

$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.c=$(BUILD_DIR)/%.o)
$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv) # new code for music

$(PROJECT_NAME).z64: N64_ROM_TITLE="Tiny3D - Animation"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

sc64: build_lib
	sc64deployer --remote 192.168.0.6:9064 upload --tv ntsc *.z64
	curl 192.168.0.6:9065/reset

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
