# For CSC3004 Software Development

# Put your user name below:

USER= ogilviethompsonh

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

all: gutenburgServer gutenburgTest gutenburgAjax PutCGI PutHTML
#all: server commandLineTest 

commandLineTest.o: commandLineTest.cpp fifo.h english_stem.h meta.h stemming.h string_util.h utilities.h
	$(CC) -c $(CFLAGS) commandLineTest.cpp

server.o: server.cpp fifo.h english_stem.h meta.h stemming.h string_util.h utilities.h
	$(CC) -c $(CFLAGS) server.cpp

gutenburgAjax.o: gutenburgAjax.cpp fifo.h
	$(CC) -c $(CFLAGS) gutenburgAjax.cpp

gutenburgTest: commandLineTest.o fifo.o
	$(CC) commandLineTest.o fifo.o -o gutenburgTest

gutenburgServer: server.o fifo.o
	$(CC) server.o  fifo.o -o server
	
fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp

gutenburgAjax: gutenburgAjax.o  fifo.h
	$(CC) gutenburgAjax.o  fifo.o -o gutenburgAjax -L/usr/local/lib -lcgicc

PutCGI: gutenburgAjax
	chmod 757 gutenburgAjax
	cp gutenburgAjax /usr/lib/cgi-bin/$(USER)_gutenburgAjax.cgi 

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp Gutenburg.html /var/www/html/class/softdev/$(USER)
	cp gutenburg.js /var/www/html/class/softdev/$(USER)
	cp gutenburg.css /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o gutenburg server commandLineTest