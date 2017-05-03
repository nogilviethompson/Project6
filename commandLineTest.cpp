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
int fileCount = 0;
int matchCount = 0;
int fileMatchCount = 0;
long long wordCount = 0;
string delimiters = " ,.;:?'\"()[]";
struct pairs{
	  unsigned short pathPointer;
	  int position;
 };

void ProcessDirectory(string directory, vector<string>& filePaths, map<string, vector<struct pairs> >& words);
void ProcessFile(string file, vector<string>& filePaths, map<string, vector<struct pairs> >& words);
void ProcessEntity(struct dirent* entity, vector<string>& filePaths, map<string, vector<struct pairs> >& words);
bool hasEnding (string const &fullString, string const &ending);
void build(string file, unsigned int point, map<string, vector<struct pairs> >& words);
vector<struct pairs> search(string word, map<string, vector<struct pairs> > words);
void stemmer(string& word);
bool commonWord(string word); 
 
int main()
{
  struct pairs locations, specific;
  vector<struct pairs> refs;
  string word;
  string directory = "";
  string display;
  ifstream infile;
  map <string, vector <struct pairs> > words;
  vector<string> filePaths;
  
  ProcessDirectory(directory, filePaths, words);
  
  cout << "Please enter a word to search for: ";
  cin >> word;
  transform(word.begin(), word.end(), word.begin(), ::tolower);
  stemmer(word);
  refs = search(word, words);
  
  for (unsigned int i = 0; i < refs.size(); i++){
	  specific = refs[i];
	  unsigned short currentPathIndex = specific.pathPointer;
	  string currentPath = filePaths[currentPathIndex];
	  infile.open(currentPath.c_str());
	  int pointer = specific.position;
      infile.seekg (pointer, infile.beg);
      getline(infile, display);
      cout << display << endl;
  }
  
  return 0;
}

void stemmer(string& word) 
{
	transform(word.begin(), word.end(), word.begin(), ::toupper);
    
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

void ProcessDirectory(string directory, vector<string>& filePaths, map<string, vector<struct pairs> >& words){
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
      ProcessEntity(entity, filePaths, words);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

void ProcessEntity(struct dirent* entity, vector<string>& filePaths, map<string, vector<struct pairs> >& words)
{
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an direcotry
      //don't process the  '..' and the '.' directories
      if(entity->d_name[0] == '.')
	{
	  return;
	}

      //it's an directory so process it
      ProcessDirectory(string(entity->d_name), filePaths, words);
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      ProcessFile(string(entity->d_name), filePaths, words);
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string file, vector<string>& filePaths, map<string, vector<struct pairs> >& words)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
	string fullPath = path+file;
	filePaths.push_back(fullPath);
	unsigned int point = filePaths.size() - 1;
	build(fullPath, point, words);
  }
}

void build(string fullPath, unsigned int point, map<string, vector<struct pairs> >& words){
	ifstream infile;
	string word, line;
	infile.open(fullPath.c_str());
	unsigned short pathPosition = point;
	struct pairs locations;
	if (infile.good())
    {
        while(!infile.eof())
        {
//Find the position of the line in the file then get the line
			int pos = infile.tellg();
            getline(infile, line);
            size_t found2 = 0;
            size_t found = line.find(" ");
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
		}
		cout << pathPosition << endl;
	}
	else{
		;
	}
}

vector<struct pairs> search(string word, map<string, vector<struct pairs> > words){
	map<string, vector<struct pairs> >::iterator it;
	vector<struct pairs> blank;
	transform(word.begin(), word.end(), word.begin(), ::toupper);
	it = words.find(word);
	if (it == words.end())
    {
        cout << "Sorry, there are no Project Gutenberg books that contain that word" << endl;
		return (blank);
    } 
	else{
		return (words[word]);
	}
}

bool commonWord(string word){
	bool common = false;
	if (word == "a"){
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
	else if (word == "all"){
		common = true;
	}
	else if (word == "am"){
		common = true;
	}
	else if (word == "an"){
		common = true;
	}
	else if (word == "and"){
		common = true;
	}
	else if (word == "any"){
		common = true;
	}
	else if (word == "are"){
		common = true;
	}
	else if (word == "aren't"){
		common = true;
	}
	else if (word == "as"){
		common = true;
	}
	else if (word == "at"){
		common = true;
	}
	else if (word == "be"){
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
	else if (word == "but"){
		common = true;
	}
	else if (word == "by"){
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
	else if (word == "did"){
		common = true;
	}
	else if (word == "didn't"){
		common = true;
	}
	else if (word == "do"){
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
	else if (word == "few"){
		common = true;
	}
	else if (word == "for"){
		common = true;
	}
	else if (word == "from"){
		common = true;
	}
	else if (word == "further"){
		common = true;
	}
	else if (word == "had"){
		common = true;
	}
	else if (word == "hadn't"){
		common = true;
	}
	else if (word == "has"){
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
	else if (word == "he"){
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
	else if (word == "her"){
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
	else if (word == "him"){
		common = true;
	}
	else if (word == "himself"){
		common = true;
	}
	else if (word == "his"){
		common = true;
	}
	else if (word == "how"){
		common = true;
	}
	else if (word == "how's"){
		common = true;
	}
	else if (word == "i"){
		common = true;
	}
	else if (word == "i'd"){
		common = true;
	}
	else if (word == "i'll"){
		common = true;
	}
	else if (word == "i'm"){
		common = true;
	}
	else if (word == "i've"){
		common = true;
	}
	else if (word == "if"){
		common = true;
	}
	else if (word == "in"){
		common = true;
	}
	else if (word == "into"){
		common = true;
	}
	else if (word == "is"){
		common = true;
	}
	else if (word == "isn't"){
		common = true;
	}
	else if (word == "it"){
		common = true;
	}
	else if (word == "it's"){
		common = true;
	}
	else if (word == "its"){
		common = true;
	}
	else if (word == "itself"){
		common = true;
	}
	else if (word == "let's"){
		common = true;
	}
	else if (word == "me"){
		common = true;
	}
	else if (word == "more"){
		common = true;
	}
	else if (word == "most"){
		common = true;
	}
	else if (word == "mr"){
		common = true;
	}
	else if (word == "mrs"){
		common = true;
	}
	else if (word == "ms"){
		common = true;
	}
	else if (word == "must"){
		common = true;
	}
	else if (word == "mustn't"){
		common = true;
	}
	else if (word == "my"){
		common = true;
	}
	else if (word == "myself"){
		common = true;
	}
	else if (word == "no"){
		common = true;
	}
	else if (word == "nor"){
		common = true;
	}
	else if (word == "not"){
		common = true;
	}
	else if (word == "of"){
		common = true;
	}
	else if (word == "off"){
		common = true;
	}
	else if (word == "on"){
		common = true;
	}
	else if (word == "once"){
		common = true;
	}
	else if (word == "only"){
		common = true;
	}
	else if (word == "or"){
		common = true;
	}
	else if (word == "other"){
		common = true;
	}
	else if (word == "ought"){
		common = true;
	}
	else if (word == "our"){
		common = true;
	}
	else if (word == "ours"){
		common = true;
	}
	else if (word == "ourselves"){
		common = true;
	}
	else if (word == "out"){
		common = true;
	}
	else if (word == "over"){
		common = true;
	}
	else if (word == "own"){
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
	else if (word == "she"){
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
	else if (word == "so"){
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
	else if (word == "the"){
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
	else if (word == "to"){
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
	else if (word == "up"){
		common = true;
	}
	else if (word == "very"){
		common = true;
	}
	else if (word == "was"){
		common = true;
	}
	else if (word == "wasn't"){
		common = true;
	}
	else if (word == "we"){
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
	else if (word == "who"){
		common = true;
	}
	else if (word == "who's"){
		common = true;
	}
	else if (word == "whom"){
		common = true;
	}
	else if (word == "why"){
		common = true;
	}
	else if (word == "why's"){
		common = true;
	}
	else if (word == "with"){
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
	else if (word == "you"){
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