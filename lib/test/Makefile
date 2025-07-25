OFLAGS += -g
CFLAGS += -I/opt/local/include -Wall
LDLIBS += -L/opt/local/lib
LDLIBS += -lgc -lm

PROGS = ../msgc/mstest
OBJS = ../msgc/mstest.o ../msgc/msgc.o ../msgc/fatal.o

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
mstest.o: ../msgc/mstest.c ../msgc/msgc.h ../msgc/fatal.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

msgc.o: ../msgc/msgc.c ../msgc/msgc.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

fatal.o: ../msgc/fatal.c ../msgc/fatal.h
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

# Test targets
test : all .FORCE
	time ../msgc/mstest < test.txt

test-opt : opt .FORCE
	time ../msgc/mstest < test-opt.txt

test-big : all .FORCE
	time ../msgc/mstest -m 32M < test.txt

test-cycle : all .FORCE
	time ../msgc/mstest < test-cycle.txt

test-sys : sys .FORCE
	time ../msgc/mstest < test-sys.txt

clean : .FORCE
	rm -rf $(PROGS) *.o *.dSYM *~
	rm -rf ../msgc/*.o ../msgc/*.dSYM ../msgc/*~

.FORCE :

