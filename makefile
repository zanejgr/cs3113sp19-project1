CC = gcc
cflags := -g -O3
executables = oush
all: $(executables)
debug: cflags += -DDEBUG 
debug: all

clean:
	$(RM) $(executables)

oush: oush.c
	$(CC) oush.c $(cflags) -o oush
