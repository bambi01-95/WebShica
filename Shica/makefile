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
FILES = ./Platform/WebShica/shica.js \
        ./Platform/WebShica/shica.wasm \
		./Platform/WebShica/shica-lib.js

# コンパイラ
WASMCC = emcc

# ソースとターゲット
SRC = main.c \
      ./GC/msgcs/msgcs.c  \
	  ./Error/error.c   \
	  ./Opcode/opcode.c \
	  ./Node/node.c \
	  ./Object/object.c \
	  ./Parser/parser.c 

INCLUDE_DIRS = -I ./GC \
           	   -I ./Error \
			   -I ./Opcode \
			   -I ./Node \
			   -I ./Object \
			   -I ./Parser 
# shica.js
WASMOUT = ./Platform/WebShica/shica.js
# shica-native
NATIVEOUT = shica

FLAGS = -DMSGCS -DDEBUG

WEB_SHICA_FUNC = "_memory_init","_initWebCodes","_compileWebCode","_addWebCode","_deleteWebCode","_initWebAgents","_executeWebCodes","_stopWebCodes","_initWebTimerPtr","_initWebClickSTTPtr","_initALLAgentDataPtr", "_getAnAgentDataPtr", "_getCompiledWebCode"
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
         -s EXPORTED_FUNCTIONS='[$(WEB_SHICA_FUNC), $(WEB_SHICA_ERROR_FUNC)]' \
		 -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32","HEAPU8","FS"]' \
         --no-entry \
		 --js-library ./Platform/WebShica/shica-lib.js \
         -o $(WASMOUT)



cc:
	leg -o ./Parser/parser.c ./Parser/parser.leg
# ./legstruct_type_inserter
	gcc $(SRC) ./Platform/Linux/Library/library.c -o $(NATIVEOUT) $(FLAGS) $(INCLUDE_DIRS) -I ./Platform/Linux/Library/

input:
	./$(NATIVEOUT) < ./input.txt -c

# build vm.js
wasm: $(WASMOUT)

$(WASMOUT): $(SRC)
	$(WASMCC) $(SRC) ./Platform/WebShica/Library/library.c $(WEB_SHICA_FLAGS) 

move: wasm
	mkdir -p $(DEST)
	cp $(FILES) $(DEST)


clean:
	rm -f shica.c shica shica.js shica.wasm
	rm -rf $(DEST)/shica.js $(DEST)/shica.wasm
	rm -rf $(FILES)


.PHONY: all clean
