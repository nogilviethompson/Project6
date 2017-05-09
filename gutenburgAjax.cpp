#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

//Stuff for pipes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "fifo.h"

using namespace std;

string receive_fifo = "messageReply";
string send_fifo = "messageRequest";

int main() {
	cgicc::Cgicc cgi;    // Ajax object
	char *cstr;
	string results = "hi";
  
	// Create AJAX objects to recieve information from web page.
	cgicc::form_iterator ts = cgi.getElement("search");

	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
  
	string search = **ts;

	// Call server to get results
	sendfifo.openwrite();
	sendfifo.send(search);
  
	cout << "Content-Type: text/plain\n\n";
	/* Get a message from a server and then cout it*/
	/* Repeat until the $END signal is received */
	recfifo.openread();
	string reply = recfifo.recv();
	while(reply.find("$END") == -1){
		reply = "<p>" + reply + "</p>";
		cout << reply;
		reply = recfifo.recv();
		if (reply.find("$END") != -1){
			string out = reply.substr(0,reply.find("$END"));
			out = "<p>" + out + "</p>";
			cout << out;
		}
	}
  
  sendfifo.fifoclose();
  
  return 0;
  }