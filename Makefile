include Makefile.bas

SVR_NAME = shield
OBJS = shield.o

all: libs $(SVR_NAME)

LIBRARY_PATH = -L$(ROOTPATH)/frame -lframe \
       -L$(ROOTPATH)/middle -lmiddle \
       -L$(ROOTPATH)/core -lcore \
       -L$(ROOTPATH)/utils -lutils \
       -L$(ROOTPATH)/db -ldb \
       -L$(ROOTPATH)/libs -lsqlite3 \
       -L$(ROOTPATH)/libs -lparson \
       -L$(ROOTPATH)/libs -lzlog \
       -ldl \
       -lpthread

.PHONY: libs clean

libs:
	@make -C frame
	@make -C middle
	@make -C core
	@make -C utils
	@make -C db

$(SVR_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBRARY_PATH) -o $(SVR_NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

clean:
	rm -f $(OBJS) $(SVR_NAME)
	@make -C frame  clean
	@make -C middle clean
	@make -C core   clean
	@make -C utils  clean
	@make -C db     clean
