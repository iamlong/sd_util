INCDIR = -I../include/cygwin
LDDIR = -L../ -L../external/cygwin 
LDMOULE = -lsdfs -lgtest

OBJS = testserialize.o

all: $(OBJS)

$(OBJS): %.o: %.cpp
	g++ -g -std=gnu++11 $< -o $@ $(INCDIR) $(LDDIR) $(LDMOULE) 
	#./$@

clean : 
	rm -f *.o
