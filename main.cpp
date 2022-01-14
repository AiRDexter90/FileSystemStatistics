#include <conio.h>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <string>
#include <chrono>
#include "thread_pool.h"

using namespace std;
typedef unsigned int uint;
typedef unsigned long int ulint;
namespace fs = std::filesystem;


#define NR_THREADS 4
#define PATH_1 "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\FileSystem"
#define PATH_2 "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\OdczytZapisPlikow"

uint counter = 0;

//fun will be used in thread
void inspect_file(const fs::path & file)
{
    string pathname = file.string();
    fstream  reader;
    string buffer;									//a kind of container for txt lines
    uint full_lines = 0, empty_lines = 0;

    reader.open(pathname);

        /*get consecutive lines of file count, and analyze them in the mean of words and letters number*/
        while (!reader.eof())
        {
            buffer.clear();
            getline(reader, buffer, '\n');
            if (!buffer.empty()) full_lines++;
            else empty_lines++;
        }
        cout << "Full line: " << full_lines << endl;
        cout << "Empty line: " << empty_lines << endl;
}


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

//main function, that will analyze the given path--------------------------------------
void analyze_path(const fs::path& parent)
{
    thread_pool tp(NR_THREADS);
    //tell which path to analyze
    {
        string s_parent_path = parent.string();
        cout << "Wyniki analizy katalogu: " << s_parent_path << endl;
    }

    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ parent })
    {
        fs::path subpath(dir_entry.path());
        std::cout << subpath << '\n';                               //print nodes names (directories and files)        
        fs::file_status subpath_stat = fs::status(subpath);         //status let me know, if node is file or directory
        if (fs::is_directory(subpath_stat)) std::cout << "Directory" << '\n';    //print info node is directory
        else                                                                  //print info node is file and increment file counter
        {
            std::cout << "File" << '\n'; counter++;
            //inspect_file(subpath);
            
            tp.enqueue([subpath] {
                int max = 1000000000;
                for (int i = 0; i < max; i++)
                {
                    //empty
                }
                string pathname = subpath.string();
                fstream  reader;
                string buffer;									//a kind of container for txt lines
                uint full_lines = 0, empty_lines = 0;

                reader.open(pathname);

                /*get consecutive lines of file count, and analyze them in the mean of words and letters number*/
                while (!reader.eof())
                {
                    buffer.clear();
                    getline(reader, buffer, '\n');
                    if (!buffer.empty()) full_lines++;
                    else empty_lines++;
                }
                cout << "Full line: " << full_lines << endl;
                cout << "Empty line: " << empty_lines << endl; });
        }
    }
    cout << "There are " << counter << " files" << '\n';                    //print the result

}

int main()
{
    fs::path files{PATH_2};
    auto start_4 = chrono::steady_clock::now();
    analyze_path(files);
    auto stop_4 = chrono::steady_clock::now();

    cout << "Number of used threads: " << NR_THREADS << endl;
    chrono::duration<double> elapsed_time_4 = stop_4 - start_4;
    cout << "Required time: " << elapsed_time_4.count() << endl;
    return 0;
}