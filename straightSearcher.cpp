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

using namespace std;

string path = "/home/skon/books/";
string delimiters = " ,.;:?'\"()[]";
struct pairs{
	  unsigned short pathPointer;
	  int position;
 };
vector<string> names;
vector<string> filePaths;
long wordCount;

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
  string word, search, filename, book, display, title, output;
  struct pairs current;
  ifstream infile;
  
  wordCount = 0;
  
  ProcessDirectory(directory);

  while(1){
	  
  cout << "Please enter the word you'd like to search for: ";
  cin >> search;
  word = search;
  stemmer(search);
  
  if (commonWord(search) == true){
	  cout << "Sorry, but you cannot search for that word" << endl;
  }
  else{
	  filename = wordsPath + search + ".bin";
	  ifstream myfile(filename.c_str(), ios::in | ios::binary);
	  if (myfile.is_open()) {
		cout << "The following lines from the following books contain the word " << search << ":" << endl;
		while (!myfile.eof()) {
			myfile.read((char*)&current,sizeof(struct pairs));
			book = filePaths[current.pathPointer];
			infile.open(book.c_str());
			//title;
			//title = names[current.pathPointer];
			if (infile.good()){
				infile.seekg (current.position, infile.beg);
				getline(infile, display);
				cout << display << /*"(from " << title << ")" << */endl;
				infile.close();
			}
			
			if(myfile.eof()){
				break;
			}
		}
		myfile.close();
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
	int lineNumber;
	bool named = false;
	infile.open(fullPath.c_str());
	unsigned short pathPosition = point;
	
	lineNumber = 0;
	
	if (infile.good())
    {
        while(!infile.eof())
        {
//Find the position of the line in the file then get the line
			int pos = infile.tellg();
            getline(infile, line);
			size_t found = line.find("Title: ");
			if (lineNumber <= 20){
				if (found != string::npos && named == false){
					found = found + 7;
					title = line.substr(found);
					names.push_back(title);
					named = true;
					break;
				}
			}
			
			if (lineNumber > 20 && named == false){
				title = "Unknown";
				names.push_back(title);
				named = true;
				break;
			}
			lineNumber++;
		}
	}
	else{
		;
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