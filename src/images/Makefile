bin2c_OBJECT = bin2c.o
EXEEXT = 
LIBS = 
bin2c_LDFLAGS = 
srcdir = .
IMAGES = goal.png halfstone_1.png halfstone_2.png halfstone_3.png halfstone_4.png man.png object.png saveman.png stone_1.png stone_2.png stone_3.png stone_4.png stone_5.png stone_6.png treasure.png starfield.png

CLEANFILES = data.c

all: data.c

bin2c$(EXEEXT): $(bin2c_OBJECT)
	@rm -f bin2c$(EXEEXT)
	gcc $(bin2c_OBJECT) $(bin2c_LDFLAGS) $(LIBS) -o bin2c$(EXEEXT)

$(bin2c_OBJECT): 
	gcc -c bin2c.c
	
data.c: $(IMAGES) bin2c$(EXEEXT)
	list=""; for i in $(IMAGES); do list="$$list $(srcdir)/$$i"; done; \
	./bin2c$(EXEEXT) "" $$list

clean:
	rm -f data.c bin2c$(EXEEXT) $(bin2c_OBJECT)

