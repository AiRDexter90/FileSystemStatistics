#include <conio.h>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include "thread_pool.h"

using namespace std;
typedef unsigned int uint;
typedef unsigned long int ulint;
namespace fs = std::filesystem;

//structure represent directory and its content
struct dir
{
    fs::path raw_path;              //path to directory
    string path;                    //path as the string
    vector <dir> subdirs;           //vector of direct subdirectory
    uint total_files;               //total number of files
    uint total_lines;               //total number of lines
    ulint total_words;               //total number of words
    ulint total_letters;             //total number of letters
    //counting totals - sum over vector
};

//vector<function<void()>>;
uint counter = 0;

fs::path files{ "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\FileSystem" };

//like constructor---------------------------------------------------------------
void make_test_directory(const fs::path& parent)
{
    std::filesystem::create_directories(files / "dir1" / "dir2");
    std::filesystem::create_directories(files / "dir11" / "dir12" / "dir13");
    std::ofstream{ files / "file1.txt" };
    std::ofstream{ files / "file2.txt" };
    std::ofstream{ files / "dir11" / "file11.txt" };
}

//like destructor----------------------------------------------------------------
void destroy_test_directory(const fs::path& parent)
{
    cout << "Remove" << endl;
    std::filesystem::remove_all(files);                                     //remove directory created for test
}


//main function, that will analyze the given path--------------------------------------
void analyze_path(const fs::path& parent)
{
    //tell which path to analyze
    {
        string s_parent_path = parent.string();
        cout << "Wyniki analizy katalogu: " << s_parent_path << endl;
    }

    std::cout << "recursive_directory_iterator:\n";
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ files })
    {
        fs::path subpath(dir_entry.path());
        std::cout << subpath << '\n';                               //print nodes names (directories and files)        
        fs::file_status subpath_stat = fs::status(subpath);         //status let me know, if node is file or directory
        if (fs::is_directory(subpath_stat)) std::cout << "Directory" << '\n';    //print info node is directory
        else                                                                  //print info node is file and increment file counter
        {
            std::cout << "File" << '\n'; counter++;
        }
    }
    cout << "There are " << counter << " files" << '\n';                    //print the result

}

int main()
{
    
    //there are created some directories and a few files for test iterating over given directory-------------------------------------------
    make_test_directory(files);
    analyze_path(files);
    destroy_test_directory(files);

    return 0;
}