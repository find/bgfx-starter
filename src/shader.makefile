SHADERC=../.build/win64_vs2017/bin/shadercRelease
INCLUDEDIR=../3rdparty/bgfx/src ../3rdparty/bgfx/examples/common

SC=$(wildcard *.sc)
HEADERS=$(filter-out varying.def.bin.h, $(SC:.sc=.bin.h))
INTERMEDIATE_DIR=.intermediate

# INCLUDE_FLAGS=$(subst %,-i%,$(INCLUDEDIR))
INCLUDE_FLAGS=$(INCLUDEDIR:%=-i %)

%.bin.h: $(INTERMEDIATE_DIR)/%.windows.sm3.h $(INTERMEDIATE_DIR)/%.windows.sm4.h $(INTERMEDIATE_DIR)/%.glsl.h $(INTERMEDIATE_DIR)/%.spv.h
	cat $^ > $@

$(INTERMEDIATE_DIR):
	mkdir $(INTERMEDIATE_DIR)

$(INTERMEDIATE_DIR)/vs_%.windows.sm3.h : vs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type vertex --platform windows -p vs_3_0 -O 3 -f $< -o $@ --bin2c $(basename $<)_vs3

$(INTERMEDIATE_DIR)/vs_%.windows.sm4.h : vs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type vertex --platform windows -p vs_4_0 -O 3 -f $< -o $@ --bin2c $(basename $<)_vs4

$(INTERMEDIATE_DIR)/fs_%.windows.sm3.h : fs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type fragment --platform windows -p ps_3_0 -O 3 -f $< -o $@ --bin2c $(basename $<)_ps3

$(INTERMEDIATE_DIR)/fs_%.windows.sm4.h : fs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type fragment --platform windows -p ps_4_0 -O 3 -f $< -o $@ --bin2c $(basename $<)_ps4

$(INTERMEDIATE_DIR)/vs_%.glsl.h : vs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type vertex --platform linux -f $< -o $@ --bin2c $(basename $<)_glsl

$(INTERMEDIATE_DIR)/fs_%.glsl.h : fs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type fragment --platform linux -f $< -o $@ --bin2c $(basename $<)_glsl

$(INTERMEDIATE_DIR)/vs_%.spv.h : vs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type vertex --platform linux -p spirv -f $< -o $@ --bin2c $(basename $<)_spv

$(INTERMEDIATE_DIR)/fs_%.spv.h : fs_%.sc
	$(SHADERC) $(INCLUDE_FLAGS) --type fragment --platform linux -p spirv -f $< -o $@ --bin2c $(basename $<)_spv

all: $(INTERMEDIATE_DIR) $(HEADERS)

headers: $(HEADERS)

vars:
	echo $(SC)
	echo $(HEADERS)
	echo $(INCLUDE_FLAGS)
	echo $(HEADERS)


