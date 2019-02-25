CC = gcc
cflags := -g -O3
executables = oush
all: clean $(executables)
debug: cflags += -DDEBUG 
debug: all

clean:
	$(RM) $(executables)

oush:
	$(CC) oush.c $(cflags) -o oush
