/*thi code is to test file processing, for example counting lines, words, letters*/

#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

typedef unsigned int uint;

fstream  file;
string buffer;									//a kind of container for txt lines
uint full_lines,empty_lines;

int main()
{
	
	file.open("plik1.txt");
	full_lines = empty_lines = 0;				//full and empty lines counter
	
	/*get consecutive lines of file count, and analyze them in the mean of words and letters number*/
	while (!file.eof())
	{
		buffer.clear();
		getline(file,buffer,'\n');
		if (!buffer.empty()) full_lines++;
		else empty_lines++;
	}
	cout << "Full line: " << full_lines << endl;
	cout << "Empty line: " << empty_lines << endl;

	return 0;
}