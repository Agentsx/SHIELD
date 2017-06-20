include ../Makefile.bas

LIB_TARG = libframe.a

LIB_OBJS = frame.o handler.o net.o protocol.o thread.o

$(LIB_TARG): $(LIB_OBJS)
	$(AR) $(ARPARAM) $@ $(LIB_OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.PHONY: clean
clean:
	rm -f $(LIB_OBJS) $(LIB_TARG)
