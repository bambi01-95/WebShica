PORT=8000

# copy
DEST = ./shica-next-app/public/shikada/js
FILES = vm.js vm.wasm

# コンパイラ
CC = emcc

# ソースとターゲット
SRC = vm.c ./gc/msgc/msgc.c
OUT = vm.js

# コンパイルフラグ
CFLAGS = -O3 -I./gc -s WASM=1 -s INITIAL_MEMORY=64MB -s ENVIRONMENT=web -s EXPORTED_FUNCTIONS='["_memory_init","_compileWebCode","_initRunWeb","_runWeb","_initWebTimerPtr","_initWebClickSTTPtr","_initAnAgnetDataPtr"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32"]' --no-entry -o $(OUT)

crun:
	leg -o vm.c vm.leg
# ./legstruct_type_inserter
	gcc vm.c ./gc/msgc/msgc.c -o vm
	
# build vm.js
web: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS)



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
