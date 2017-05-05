#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "english_stem.h"
#include "meta.h"
#include "stemming.h"
#include "string_util.h"
#include "utilities.h"
#include "fifo.h"

using namespace std;

string path = "/home/skon/books/";
string delimiters = " ,.;:?'\"()[]";
struct pairs{
	  unsigned short pathPointer;
	  int position;
 };
vector<string> titles;
vector<string> filePaths;
map<string, vector<struct pairs> > words;
string receive_fifo = "messageRequest";
string send_fifo = "messageReply";


void ProcessDirectory(string directory);
void ProcessFile(string file);
void ProcessEntity(struct dirent* entity);
bool hasEnding (string const &fullString, string const &ending);
void build(string file, unsigned int point);
void stemmer(string& word);
bool commonWord(string word);
void writeFiles(map<string, vector<struct pairs> > mymap);
 
int main()
{
  string directory = "";
  string wordsPath = "/home/students/ogilviethompsonh/project6/bins/";
  string word, search, filename, book, title, display;
  struct pairs current;
  ifstream infile;
  Fifo recfifo(receive_fifo);
  Fifo sendfifo(send_fifo); 
  
  ProcessDirectory(directory);
  writeFiles(words);
  words.clear();
  
  while(1){
	recfifo.openread();
    search = recfifo.recv();
	recfifo.fifoclose();
	word = search;
	stemmer(search);
  
	if (commonWord(search) == true){
		sendfifo.openwrite();
		string message = "Sorry, but you cannot search for " + word;
		sendfifo.send(message);
		sendfifo.send("$END");
		sendfifo.fifoclose();
	}
	else{
		sendfifo.openwrite();
		filename = wordsPath + search + ".bin";
		ifstream myfile(filename.c_str(), ios::in | ios::binary);
		if (myfile.is_open()) {
			string message = "The following lines from the following books contain the word " + search + ":";
			sendfifo.send(message);
			while (!myfile.eof()) {
				myfile.read((char*)&current,sizeof(struct pairs));
				title = titles[current.pathPointer];
				book = filePaths[current.pathPointer];
				infile.open(book.c_str());
				if (infile.good()){
					infile.seekg (current.position, infile.beg);
					getline(infile, display);
					message = title + ": " + display;
					sendfifo.send(message);
				}
			}
			myfile.close();
			sendfifo.send("$END");
		}
		else {
			cout << "Unable to open bin" << endl;
		}
	}
  }
  return 0;
}

void stemmer(string& word) 
{
	transform(word.begin(), word.end(), word.begin(), ::tolower);
    
    stemming::english_stem<char, std::char_traits<char> > StemEnglish;    
    
    StemEnglish(word);
}

bool hasEnding (string const &fullString, string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
  } else {
    return false;
  }
}

void ProcessDirectory(string directory){
  string dirToOpen = path + directory;
  DIR *dir;
  dir = opendir(dirToOpen.c_str());

  //set the new path for the content of the directory
  path = dirToOpen + "/";

  //  cout << "Process directory: " << dirToOpen.c_str() << endl;

  if(NULL == dir)
    {
      cout << "could not open directory: " << dirToOpen.c_str() << endl;
      return;
    }
  struct dirent *entity;
  entity = readdir(dir);

  while(entity != NULL)
    {
      ProcessEntity(entity);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

void ProcessEntity(struct dirent* entity){
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an direcotry
      //don't process the  '..' and the '.' directories
      if(entity->d_name[0] == '.')
	{
	  return;
	}

      //it's an directory so process it
      ProcessDirectory(string(entity->d_name));
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      ProcessFile(string(entity->d_name));
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string file){
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
	string fullPath = path+file;
	filePaths.push_back(fullPath);
	unsigned int point = filePaths.size() - 1;
	build(fullPath, point);
  }
}

void build(string fullPath, unsigned int point){
	ifstream infile;
	string word, line, title, filename;
	int lineNumber, marker;
	infile.open(fullPath.c_str());
	unsigned short pathPosition = point;
	struct pairs locations;
	
	lineNumber = 0;
	marker = 0;
	
	if (infile.good())
    {
        while(!infile.eof())
        {
//Find the position of the line in the file then get the line
			int pos = infile.tellg();
            getline(infile, line);
			size_t found = line.find("Title: ");
			if (lineNumber < 20){
				if (found != string::npos && marker == 0){
					found = found + 7;
					title = line.substr(found);
					titles.push_back(title);
					marker = 1;
				}
			}
			
			if (lineNumber > 20 && marker == 0){
				title = "Unknown";
				titles.push_back(title);
			}
            size_t found2 = 0;
            found = line.find(" ");
            while (found != string::npos)
            {
				word = line.substr(found2, found-found2);
				stemmer(word);
				transform(word.begin(), word.end(), word.begin(), ::tolower);
				if (commonWord(word) == false){
					locations.pathPointer = pathPosition;
					locations.position = pos;
					words[word].push_back(locations);
				}
				found2 = found + 1;
				found = line.find(" ", found2);
			}
			lineNumber=lineNumber+1;
		}
		cout << pathPosition << endl;
	}
	else{
		;
	}

	if (words.size() > 500000){
		writeFiles(words);
		words.clear();
		cout << "Written" << endl;
	}
}

void writeFiles(map<string, vector<struct pairs> > mymap){
	for (map<string, vector<struct pairs> >::iterator it=mymap.begin(); it!=mymap.end(); it++){
		string word = it->first;
		vector<struct pairs> current = mymap[word];
		string filename = "/home/students/ogilviethompsonh/project6/bins/" + word +".bin";
		ofstream myfile(filename.c_str(), ios::out | ios::binary | ios:: app);
		if (myfile.is_open()) {
			for (unsigned int i = 0; i < current.size(); i++){
				struct pairs value = current[i];
				myfile.write((char*)&value, sizeof(struct pairs));
			}
			myfile.close();
		}
		else {
			cout << "Unable to open file" << endl;
		}
	}
}

bool commonWord(string word){
	bool common = false;
	size_t found = word.find_first_not_of("abcdefghijklmnopqrstuvwxyz ");
	
	if (found != string::npos){
		common = true;
	}
	else if (word.length() <= 3){
		common = true;
	}
	else if (word == "about"){
		common = true;
	}
	else if (word == "above"){
		common = true;
	}
	else if (word == "after"){
		common = true;
	}
	else if (word == "again"){
		common = true;
	}
	else if (word == "against"){
		common = true;
	}
	else if (word == "aren't"){
		common = true;
	}
	else if (word == "because"){
		common = true;
	}
	else if (word == "been"){
		common = true;
	}
	else if (word == "before"){
		common = true;
	}
	else if (word == "being"){
		common = true;
	}
	else if (word == "below"){
		common = true;
	}
	else if (word == "between"){
		common = true;
	}
	else if (word == "both"){
		common = true;
	}
	else if (word == "can't"){
		common = true;
	}
	else if (word == "cannot"){
		common = true;
	}
	else if (word == "could"){
		common = true;
	}
	else if (word == "couldn't"){
		common = true;
	}
	else if (word == "didn't"){
		common = true;
	}
	else if (word == "does"){
		common = true;
	}
	else if (word == "doesn't"){
		common = true;
	}
	else if (word == "doing"){
		common = true;
	}
	else if (word == "don't"){
		common = true;
	}
	else if (word == "down"){
		common = true;
	}
	else if (word == "during"){
		common = true;
	}
	else if (word == "each"){
		common = true;
	}
	else if (word == "from"){
		common = true;
	}
	else if (word == "further"){
		common = true;
	}
	else if (word == "hadn't"){
		common = true;
	}
	else if (word == "hasn't"){
		common = true;
	}
	else if (word == "have"){
		common = true;
	}
	else if (word == "haven't"){
		common = true;
	}
	else if (word == "having"){
		common = true;
	}
	else if (word == "he'd"){
		common = true;
	}
	else if (word == "he'll"){
		common = true;
	}
	else if (word == "he's"){
		common = true;
	}
	else if (word == "here"){
		common = true;
	}
	else if (word == "here's"){
		common = true;
	}
	else if (word == "hers"){
		common = true;
	}
	else if (word == "herself"){
		common = true;
	}
	else if (word == "himself"){
		common = true;
	}
	else if (word == "how's"){
		common = true;
	}
	else if (word == "i'll"){
		common = true;
	}
	else if (word == "i've"){
		common = true;
	}
	else if (word == "into"){
		common = true;
	}
	else if (word == "isn't"){
		common = true;
	}
	else if (word == "it's"){
		common = true;
	}
	else if (word == "itself"){
		common = true;
	}
	else if (word == "let's"){
		common = true;
	}
	else if (word == "more"){
		common = true;
	}
	else if (word == "most"){
		common = true;
	}
	else if (word == "must"){
		common = true;
	}
	else if (word == "mustn't"){
		common = true;
	}
	else if (word == "myself"){
		common = true;
	}
	else if (word == "once"){
		common = true;
	}
	else if (word == "only"){
		common = true;
	}
	else if (word == "other"){
		common = true;
	}
	else if (word == "ought"){
		common = true;
	}
	else if (word == "ours"){
		common = true;
	}
	else if (word == "ourselves"){
		common = true;
	}
	else if (word == "over"){
		common = true;
	}
	else if (word == "same"){
		common = true;
	}
	else if (word == "shall"){
		common = true;
	}
	else if (word == "shan't"){
		common = true;
	}
	else if (word == "she'd"){
		common = true;
	}
	else if (word == "she'll"){
		common = true;
	}
	else if (word == "she's"){
		common = true;
	}
	else if (word == "should"){
		common = true;
	}
	else if (word == "shouldn't"){
		common = true;
	}
	else if (word == "some"){
		common = true;
	}
	else if (word == "such"){
		common = true;
	}
	else if (word == "than"){
		common = true;
	}
	else if (word == "that"){
		common = true;
	}
	else if (word == "that's"){
		common = true;
	}
	else if (word == "their"){
		common = true;
	}
	else if (word == "theirs"){
		common = true;
	}
	else if (word == "them"){
		common = true;
	}
	else if (word == "themselves"){
		common = true;
	}
	else if (word == "then"){
		common = true;
	}
	else if (word == "there"){
		common = true;
	}
	else if (word == "there's"){
		common = true;
	}
	else if (word == "these"){
		common = true;
	}
	else if (word == "they"){
		common = true;
	}
	else if (word == "they'd"){
		common = true;
	}
	else if (word == "they'll"){
		common = true;
	}
	else if (word == "they're"){
		common = true;
	}
	else if (word == "they've"){
		common = true;
	}
	else if (word == "this"){
		common = true;
	}
	else if (word == "those"){
		common = true;
	}
	else if (word == "through"){
		common = true;
	}
	else if (word == "too"){
		common = true;
	}
	else if (word == "under"){
		common = true;
	}
	else if (word == "until"){
		common = true;
	}
	else if (word == "very"){
		common = true;
	}
	else if (word == "wasn't"){
		common = true;
	}
	else if (word == "we'd"){
		common = true;
	}
	else if (word == "we'll"){
		common = true;
	}
	else if (word == "we're"){
		common = true;
	}
	else if (word == "we've"){
		common = true;
	}
	else if (word == "were"){
		common = true;
	}
	else if (word == "weren't"){
		common = true;
	}
	else if (word == "what"){
		common = true;
	}
	else if (word == "what's"){
		common = true;
	}
	else if (word == "when"){
		common = true;
	}
	else if (word == "when's"){
		common = true;
	}
	else if (word == "where"){
		common = true;
	}
	else if (word == "where's"){
		common = true;
	}
	else if (word == "which"){
		common = true;
	}
	else if (word == "while"){
		common = true;
	}
	else if (word == "who's"){
		common = true;
	}
	else if (word == "whom"){
		common = true;
	}
	else if (word == "why's"){
		common = true;
	}
	else if (word == "won't"){
		common = true;
	}
	else if (word == "would"){
		common = true;
	}
	else if (word == "wouldn't"){
		common = true;
	}
	else if (word == "you'd"){
		common = true;
	}
	else if (word == "you'll"){
		common = true;
	}
	else if (word == "you're"){
		common = true;
	}
	else if (word == "you've"){
		common = true;
	}
	else if (word == "your"){
		common = true;
	}
	else if (word == "yours"){
		common = true;
	}
	else if (word == "yourself"){
		common = true;
	}
	else if (word == "yourselves"){
		common = true;
	}
	
	return common;
}