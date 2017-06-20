include Makefile.bas

SVR_NAME = shield
OBJS = shield.o
all: libs $(SVR_NAME)

LIBS = -L$(ROOTPATH)/frame -lframe \
       -L$(ROOTPATH)/middle -lmiddle \
       -L$(ROOTPATH)/core -lcore \
       -L$(ROOTPATH)/utils -lutils \
       -L$(ROOTPATH)/db -ldb \
       -L$(ROOTPATH)/libs -lsqlite3 \
       -lpthread

libs:
	@make -C frame
	@make -C middle
	@make -C core
	@make -C utils
	@make -C db

$(SVR_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(SVR_NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

clean:
	rm -f $(OBJS) $(SVR_NAME)
	@make -C frame  clean
	@make -C middle clean
	@make -C core   clean
	@make -C utils  clean
	@make -C db  clean
