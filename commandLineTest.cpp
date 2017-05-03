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
map <string, vector <struct pairs> > words;
vector<string> filePaths;

void ProcessDirectory(string directory);
void ProcessFile(string file, vector<string>& filePaths);
void ProcessEntity(struct dirent* entity);
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
  ProcessDirectory(directory);
  
  cout << "Please enter a word to search for: ";
  cin >> word;
  transform(word.begin(), word.end(), word.begin(), ::toupper);
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

void ProcessDirectory(string directory)
{
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

void ProcessEntity(struct dirent* entity)
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
      ProcessDirectory(string(entity->d_name));
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      ProcessFile(string(entity->d_name), filePaths);
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string file, vector<string>& filePaths)
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
				transform(word.begin(), word.end(), word.begin(), ::toupper);
				if (!commonWord(word)){
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
	if (word == "A"){
		common = true;
	}else if (word == "THE"){
		common = true;
	}else if (word == "IS"){
		common = true;
	}else if (word == "AM"){
		common = true;
	}else if (word == "ARE"){
		common = true;
	}else if (word == "HE"){
		common = true;
	}else if (word == "SHE"){
		common = true;
	}else if (word == "I"){
		common = true;
	}else if (word == "IT"){
		common = true;
	}else if (word == "WHO"){
		common = true;
	}else if (word == "AT"){
		common = true;
	}else if (word == "AND"){
		common = true;
	}else if (word == "OR"){
		common = true;
	}else if (word == "BUT"){
		common = true;
	}else if (word == "IF"){
		common = true;
	}else if (word == "ELSE"){
		common = true;
	}else if (word == "THEY"){
		common = true;
	}else if (word == "YOU"){
		common = true;
	}else if (word == "WE"){
		common = true;
	}else if (word == "US"){
		common = true;
	}else if (word == "HIM"){
		common = true;
	}else if (word == "HER"){
		common = true;
	}else if (word == "THEM"){
		common = true;
	}else if (word == "THEIR"){
		common = true;
	}else if (word == "THERE"){
		common = true;
	}else if (word == "HERE"){
		common = true;
	}else if (word == "AS"){
		common = true;
	}else if (word == "BY"){
		common = true;
	}else if (word == "ELSE"){
		common = true;
	}else if (word == "FOR"){
		common = true;
	}else if (word == "DO"){
		common = true;
	}else if (word == "GO"){
		common = true;
	}else if (word == "HIS"){
		common = true;
	}else if (word == "HERS"){
		common = true;
	}else if (word == "THEIRS"){
		common = true;
	}else if (word == "ITS"){
		common = true;
	}else if (word == "IN"){
		common = true;
	}else if (word == "ON"){
		common = true;
	}else if (word == "JUST"){
		common = true;
	}else if (word == "MR"){
		common = true;
	}else if (word == "MS"){
		common = true;
	}else if (word == "MRS"){
		common = true;
	}else if (word == "MISTER"){
		common = true;
	}else if (word == "MY"){
		common = true;
	}else if (word == "YES"){
		common = true;
	}else if (word == "NO"){
		common = true;
	}else if (word == "NOR"){
		common = true;
	}else if (word == "OF"){
		common = true;
	}else if (word == "OFF"){
		common = true;
	}else if (word == "OH"){
		common = true;
	}else if (word == "OUR"){
		common = true;
	}else if (word == "PER"){
		common = true;
	}else if (word == "SAY"){
		common = true;
	}
	
	return common;
}