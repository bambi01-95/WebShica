# <!-- Please write the following content in English -->
# Shica makefile
# Option Flags:
# <GC>
#  - MSGC:
#. - MSGCS:
#. <DEBUG>
#  - DEBUG:



# copy
DEST = ../shica-next-app/public/shikada/js
APPDEST = ../../shica-app/public/ide/js
FILES = ./Platform/WebShica/shica.js \
        ./Platform/WebShica/shica.wasm \
		./Platform/WebShica/shica-lib.js

# コンパイラ
WASMCC = emcc

# ソースとターゲット
COMMON_SRC = ./GC/msgcs/msgcs.c  \
	  		 ./Error/error.c  \
			 ./Opcode/opcode.c \
			 ./File/file.c \
			 ./Tool/tool.c
COMPILER_SRC = ./Node/node.c\
			   ./Compiler/compiler.c \
			   ./Parser/parser.c 
EXECUTOR_SRC = ./Executor/executor.c \
               ./Object/object.c 
# インクルードディレクトリ
COMMON_INCLUDE_DIRS = -I . \
                      -I ./GC \
                      -I ./GC/msgcs \
                      -I ./Error \
                      -I ./Opcode \
                      -I ./File \
                      -I ./Tool

COMPILER_INCLUDE_DIRS = $(COMMON_INCLUDE_DIRS) \
                        -I ./Node \
                        -I ./Compiler \
                        -I ./Parser

EXECUTOR_INCLUDE_DIRS = $(COMMON_INCLUDE_DIRS) \
                        -I ./Executor \
                        -I ./Object \
                        -I ./Platform/Linux/Library

# 共通ソース
SRC = ./GC/msgcs/msgcs.c  \
	  ./Error/error.c   \
	  ./Opcode/opcode.c \
	  ./Node/node.c \
	  ./Compiler/compiler.c \
	  ./Object/object.c \
	  ./Parser/parser.c \
	  ./Executor/executor.c \
	  ./File/file.c

INCLUDE_DIRS = -I ./GC \
			   -I ./GC/msgcs \
           	   -I ./Error \
			   -I ./Opcode \
			   -I ./Node \
			   -I ./Compiler \
			   -I ./Object \
			   -I ./Parser \
			   -I ./Executor \
			   -I ./File
# shica.js
WASMOUT = ./Platform/WebShica/shica.js
# shica-native
NATIVEOUT = shica

FLAGS = -DMSGCS -DNDEBUG -Os -DRPI
#-DMSGC    マーク＆スイープGCを有効化
#-DMSGCS   マーク＆スイープGC（分割実行）を有効化
#-DDEBUG
#-DNDEBUG   assert()を無効化
#-DSHICACOMP     コンパイラ用コードを有効化
#-DSHICAEXEC     実行用コードを有効化
#-DWEBSHICA WebShica用コードを有効化
#-DRPI Raspberry Pi用コードを有効化



leg:
	leg -o ./Parser/parser.c ./Parser/parser.leg

cc: leg
	gcc ./main.c $(SRC) ./Platform/Linux/Library/library.c -o $(NATIVEOUT) $(FLAGS) $(INCLUDE_DIRS) -I ./Platform/Linux/Library/

inputc: cc
	./$(NATIVEOUT) < ./input.txt -c
input: cc
	./$(NATIVEOUT) < ./input.txt

compiler: leg
	gcc -o ./Shica-compiler ./Shica-compiler.c \
	 $(FLAGS) -DSHICACOMP \
	 $(COMMON_SRC) $(COMPILER_SRC) ./Platform/Linux/Library/library.c  \
	 $(COMPILER_INCLUDE_DIRS) -I ./Platform/Linux/Library/

executor: 
	gcc -o ./Shica-executor ./Shica-executor.c \
	 $(FLAGS) -DSHICAEXEC \
	 $(COMMON_SRC) $(EXECUTOR_SRC) ./Platform/Linux/Library/library.c  \
	 $(EXECUTOR_INCLUDE_DIRS) -I ./Platform/Linux/Library/

WEB_SHICA_FUNC = "_memory_init","_initWebCodes","_compileWebCode","_addWebCode","_deleteWebCode","_initWebAgents","_executeWebCodes","_stopWebCodes","_initWebTimerPtr","_initWebClickSTTPtr","_initALLAgentDataPtr", "_getAnAgentDataPtr", "_getCompiledWebCode", "_reinitAllAgentData"
WEB_SHICA_LIB_FUNC = "__web_rtc_broadcast_receive_"
WEB_SHICA_ERROR_FUNC = "_getNumOfErrorMsg", "_getErrorMsg"

# コンパイルフラグ
WEB_SHICA_FLAGS = -O3 \
         $(INCLUDE_DIRS) \
		-I ./Platform/WebShica/ \
		 $(FLAGS) \
		 -DWEBSHICA \
         -s WASM=1 \
         -s INITIAL_MEMORY=128MB \
         -s ENVIRONMENT=web \
         -s EXPORTED_FUNCTIONS='[$(WEB_SHICA_FUNC), $(WEB_SHICA_LIB_FUNC), $(WEB_SHICA_ERROR_FUNC)]' \
		 -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32","HEAPU8","FS"]' \
         --no-entry \
		 --js-library ./Platform/WebShica/shica-lib.js \
         -o $(WASMOUT)

# build vm.js
wasm: $(WASMOUT)

$(WASMOUT): $(SRC)
	$(WASMCC) $(SRC) ./Platform/WebShica/Library/library.c $(WEB_SHICA_FLAGS) 

move: wasm
	mkdir -p $(DEST)
	cp $(FILES) $(DEST)
appmove: wasm
	mkdir -p $(APPDEST)
	cp $(FILES) $(APPDEST)

clean:
	rm -f shica.c shica shica.js shica.wasm
	rm -rf $(DEST)/shica.js $(DEST)/shica.wasm
	rm -rf $(FILES)


.PHONY: all clean executor
