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
DEST = ./shica-next-app/public/shikada/js
FILES = vm.js vm.wasm

# コンパイラ
WASMCC = emcc

# ソースとターゲット
SRC = vm.c \
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

WASMOUT = vm.js
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
	leg -o vm.c vm.leg
	leg -o ./Parser/parser.c ./Parser/parser.leg
# ./legstruct_type_inserter
	gcc $(SRC) -o vm $(FLAGS) $(INCLUDE_DIRS)

# build vm.js
web: $(WASMOUT)

$(WASMOUT): $(SRC)
	$(WASMCC) $(SRC) $(CFLAGS)


build:
	@echo "Checking if port $(PORT) is in use..."
	@PID=$$(lsof -t -i:$(PORT)); \
	if [ -n "$$PID" ]; then \
		echo "Port $(PORT) is in use by PID $$PID. Killing..."; \
		kill -9 $$PID; \
	else \
		echo "Port $(PORT) is free."; \
	fi
	@echo "Starting server on port $(PORT)..."
	@nohup python3 -m http.server $(PORT) >/dev/null 2>&1 &

stop:
	@echo "Checking if port $(PORT) is in use..."
	@PID=$$(lsof -t -i:$(PORT)); \
	if [ -n "$$PID" ]; then \
		echo "Stopping server (PID: $$PID)..."; \
		kill -9 $$PID; \
	else \
		echo "No server is running on port $(PORT)."; \
	fi
# Makefile for building vm.c into vm.js using Emscripten

copy:
	mkdir -p $(DEST)
	cp $(FILES) $(DEST)


clean:
	rm -f vm.c vm vm.js vm.wasm 


.PHONY: all clean
