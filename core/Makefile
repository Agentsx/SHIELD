include ../Makefile.bas

LIB_TARG = libcore.a

LIB_OBJS = core.o login.o addvol.o bizover.o command.o cutvol.o logout.o ping.o qry.o db_handler.o

$(LIB_TARG): $(LIB_OBJS)
	$(AR) $(ARPARAM) $@ $(LIB_OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.PHONY:clean
clean:
	rm -f $(LIB_OBJS) $(LIB_TARG)
