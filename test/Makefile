INCDIR = -I../googletestbin/include/cygwin -I../include/external -I../
LDDIR = -L../ -L../googletestbin/lib/cygwin  -L../lib
LDMOULE = -lsdutil -lgtest

OBJS = testserialize.o

all: $(OBJS)

$(OBJS): %.o: %.cpp
	g++ -g -std=gnu++1y $< -o $@ $(INCDIR) $(LDDIR) $(LDMOULE) 
	#./$@

clean : 
	rm -f *.o
