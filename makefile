PORT=8080
# コンパイラ
CC = emcc

# ソースとターゲット
SRC = vm.c
OUT = vm.js

# コンパイルフラグ
CFLAGS = -O3 -s WASM=1 -s INITIAL_MEMORY=64MB -s EXPORTED_FUNCTIONS='["_memory_init","_compileWebCode","_initRunWeb","_runWeb","_initWebTimerPtr","_initWebClickSTTPtr","_initAnAgnetDataPtr"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP32"]' --no-entry -o $(OUT)

crun:
	leg -o vm.c ./vm.leg
	gcc -o vm vm.c
	
# デフォルトターゲット
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
	@python3 -m http.server $(PORT)


# Makefile for building vm.c into vm.js using Emscripten




clean:
	rm -f $(OUT) $(OUT).wasm


.PHONY: all clean
