OFLAGS += -g
CFLAGS += -I/opt/local/include -Wall
LDLIBS += -L/opt/local/lib
LDLIBS += -lgc -lm

PROGS = msstest
OBJS = msstest.o msgcs.o fatal.o

all : $(PROGS)

# Release build (opt)
opt : CFLAGS += -DNDEBUG -O3
opt : clean $(PROGS)

# System GC build
sys : CFLAGS += -DNDEBUG -O3 -DSYSGC=1
sys : clean $(PROGS)

# General build rule
$(PROGS): $(OBJS)
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ $(OBJS) $(LDLIBS)

# Object build rules
msstest.o: msstest.c msgcs.h fatal.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

msgcs.o: msgcs.c msgcs.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

# fatal.o のビルドルール
fatal.o: fatal.c fatal.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

# Test targets
compile : all .FORCE
	@echo "Compilation complete."

test : all .FORCE
	time ./msstest < ../test/test.txt

test-opt : opt .FORCE
	time ./msstest < ../test/test-opt.txt

test-big : all .FORCE
	time ./msstest -m 32M < ../test/test.txt

test-cycle : all .FORCE
	time ./msstest < ../test/test-cycle.txt

test-sys : sys .FORCE
	time ./msstest < ../test/test-sys.txt

clean : .FORCE
	rm -rf $(PROGS) *.o *.dSYM *~

.FORCE :

