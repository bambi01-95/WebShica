# <!-- Please write the following content in English -->
# Shica makefile
# Option Flags:
# <GC>
#  - MSGC:
#. - MSGCS:
#. <DEBUG>
#  - DEBUG:


PORT=8000

# copy
DEST = ../shica-next-app/public/shikada/js
FILES = ./WebShica/shica.js ./WebShica/shica.wasm

# コンパイラ
WASMCC = emcc

# ソースとターゲット
SRC = main.c \
      ./GC/msgc/msgc.c  \
	  ./Error/error.c   \
	  ./Opcode/opcode.c \
	  ./Object/object.c \
	  ./Parser/parser.c 

INCLUDE_DIRS = -I ./GC \
           	   -I ./Error \
			   -I ./Opcode \
			   -I ./Object \
			   -I ./Parser 
# shica.js
WASMOUT = ./WebShica/shica.js
# shica-native
NATIVEOUT = shica

FLAGS = -DMSGC

# コンパイルフラグ
CFLAGS = -O3 \
         $(INCLUDE_DIRS) \
		 $(FLAGS) \
         -s WASM=1 \
         -s INITIAL_MEMORY=64MB \
         -s ENVIRONMENT=web \
         -s EXPORTED_FUNCTIONS='["_memory_init","_compileWebCode","_initRunWeb","_runWeb","_initWebTimerPtr","_initWebClickSTTPtr","_initAnAgnetDataPtr"]' \
         -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32"]' \
         --no-entry \
         -o $(WASMOUT)

crun:
	leg -o ./Parser/parser.c ./Parser/parser.leg
# ./legstruct_type_inserter
	gcc $(SRC) -o $(NATIVEOUT) $(FLAGS) $(INCLUDE_DIRS)

# build vm.js
web: $(WASMOUT)

$(WASMOUT): $(SRC)
	$(WASMCC) $(SRC) $(CFLAGS)
	mkdir -p $(DEST)
	cp $(FILES) $(DEST)


clean:
	rm -f shica.c shica shica.js shica.wasm


.PHONY: all clean
