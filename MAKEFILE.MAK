MEM = c
TC = c:\tc
C0x = $(TC)\lib\c0$(MEM)
Cx = $(TC)\lib\c$(MEM)

TARGET = junk.exe

OBJS = 			\
	cgbuf.obj	\
	lgbuf.obj	\
	junk.obj

HEADERS =       \
	cgbuf.h     \
	lgbuf.h

LIBS =

$(TARGET): $(OBJS) $(HEADERS)
	tlink /c $(LDFLAGS) $(C0x) $(OBJS), $*, $*, $(LIBS) $(Cx)

.c.obj:
	tcc -c -m$(MEM) $(CFLAGS) $*

clean:
	- del $(TARGET)
	- del *.map
	- del *.obj