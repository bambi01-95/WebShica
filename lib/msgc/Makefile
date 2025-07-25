OFLAGS += -g
CFLAGS += -I/opt/local/include -Wall
LDLIBS += -L/opt/local/lib
LDLIBS += -lgc -lm

PROGS = mstest
OBJS = mstest.o msgc.o fatal.o

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
mstest.o: mstest.c msgc.h fatal.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

msgc.o: msgc.c msgc.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

fatal.o: fatal.c fatal.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

# LEG to C conversion
%.c : %.leg
	leg -o $@ $<

# Test targets
test : all .FORCE
	time ./mstest < ../test/test.txt

test-opt : opt .FORCE
	time ./mstest < ../test/test-opt.txt

test-big : all .FORCE
	time ./mstest -m 32M < ../test/test.txt

test-cycle : all .FORCE
	time ./mstest < ../test/test-cycle.txt

test-sys : sys .FORCE
	time ./mstest < ../test/test-sys.txt

clean : .FORCE
	rm -rf $(PROGS) *.o *.dSYM *~

.FORCE :

