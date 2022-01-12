#include <conio.h>
#include <iostream>
#include <filesystem>
#include <fstream>

typedef unsigned int uint;
namespace fs = std::filesystem;

int counter = 0;

using namespace std;

int main()
{

    //there are created some directories and a few files for test iterating over given directory-------------------------------------------
    const fs::path files{ "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\FileSystem" };
    std::filesystem::create_directories(files / "dir1" / "dir2");
    std::filesystem::create_directories(files / "dir11" / "dir12"/"dir13");
    std::ofstream{ files / "file1.txt" };
    std::ofstream{ files / "file2.txt" };
    std::ofstream{ files / "dir11" / "file11.txt" };

    //recursive iterating over directory---------------------------------------------------------------------------------------------
    std::cout << "recursive_directory_iterator:\n";
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ files })
    {
        fs::path subpath(dir_entry.path());                                 
        std::cout << subpath << '\n';                               //print nodes names (directories and files)        
        fs::file_status subpath_stat = fs::status(subpath);         //status let me know, if node is file or directory
        if(fs::is_directory(subpath_stat)) std::cout <<"Directory"<< '\n';    //print info node is directory
        else                                                                  //print info node is file and increment file counter
        {
            std::cout << "File" << '\n'; counter++;
        }
    } 
    cout << "There are " << counter << " files"  <<'\n';                    //print the result
    std::filesystem::remove_all(files);                                     //remove directory created for test

	return 0;
}