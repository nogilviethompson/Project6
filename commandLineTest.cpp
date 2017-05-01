#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "english_stem.h"
#include "meta.h"
#include "stemming.h"
#include "string_util.h"
#include "utilities.h"

using namespace std;

struct pairs{
	  unsigned short pathPointer;
	  int position;
  } locations;

void ProcessDirectory(string directory);
void ProcessFile(string file);
void ProcessEntity(struct dirent* entity);
bool hasEnding (string const &fullString, string const &ending);
int stringMatchCount(string file);
void build(string file, int point, map<string, vector<pairs> >& words);
void search(string word, map<string, vector<pairs> > words);


string path = "/home/skon/books/";
int fileCount = 0;
int matchCount = 0;
int fileMatchCount = 0;
long long wordCount = 0;
string delimiters = " ,.;:?'\"()[]";

int main()
{
  map <string, vector <pairs> > words;
  vector<string> filePaths;
  string word;
  string directory = "";
  ProcessDirectory(directory);
  
  cout << "Please enter a word to search for: ";
  cin >> word;
  transform(word.begin(), word.end(), word.begin(), ::toupper);
  stemmer(word);
  
  return 0;
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

void ProcessEntity(struct dirent* entity, string word)
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
      ProcessFile(string(entity->d_name));
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
  int point = filePaths.size() - 1;
  build(fullPath, point, words);
  }
}

void build(string fullPath, int point, map<string, pairs>& words){
	ifstream infile;
	string word, line;
	infile.open(fullPath.c_str());
	unsigned short pathPosition = point;
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
				transform(word.begin(), word.end(), word.begin(), ::toupper);
				stemmer(word);
				locations.pathPointer = pathPosition;
				locations.position = pos;
				words[word].push_back(locations);
				found2 = found + 1;
                found = line.find(" ", found2);
			}
		}
	}
}