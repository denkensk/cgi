CC=g++
CPPFLAGS=-Wall -g
BIN=main
OBJS=main.o cgi_conn.o

$(BIN):$(OBJS)
		 $(CC) $(CPPFLAGS) $^ -o $@

main.o:cgi_conn.h processpool.hpp
cgi_conn.o:cgi_conn.h

.PHONY:clean
clean:
	-rm -f *.o $(BIN)
