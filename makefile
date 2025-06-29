crun:
	leg -o vm.c ./vm.leg
	gcc -o vm vm.c
	
build:
	python3 -m http.server 8080
	


wrun:
	emcc vm.c -o vm.js -s MODULARIZE=1 -s EXPORT_NAME="'vm'" -s "EXPORTED_FUNCTIONS=['_store', '_web_compile', '_web_compile_size', '_web_vm_execute', '_web_vm_execute_size', '_malloc', '_free']" -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'allocateUTF8', 'HEAP32']" -s ENVIRONMENT='web' -O0

clean:
	rm -f vm.c vm.js vm.wasm vm
