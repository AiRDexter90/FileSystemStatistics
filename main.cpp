#include <conio.h>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <string>
#include <chrono>
#include "thread_pool.h"

#define NR_THREADS 4
#define PATH_1 "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\FileSystem"
#define PATH_2 "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\OdczytZapisPlikow"
#define PATH_3 "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\BibliotekaGraficznaAllegro\\allegro"
#define PATH_4 "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\OdczytZapisPlikow\\New2"



using namespace std;
namespace fs = std::filesystem;

typedef unsigned int uint;
typedef unsigned long int ulint;
typedef unsigned short int usint;

//string s_parent_path;
//*************************************GLOBAL VARIABLES*******************************************
const usint max_nr_of_threads = thread::hardware_concurrency(); //number of available threads to use

ulint count_directories;
ulint count_files;
ulint count_lines;
ulint count_full_lines;
ulint count_empty_lines;
ulint count_words;
ulint count_letters;

mutex C;

//***************************************FUNCTIONS************************************************

//-----------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------------
//MAIN FUNCTION-------------------------------------------------------------------------------------------------------------
void analyze_path(const fs::path& parent,usint requested_nr_of_threads)
{
    //LIMITATION OF THREADS
    usint used_nr_of_threads = max_nr_of_threads;          
    if (requested_nr_of_threads < max_nr_of_threads && !requested_nr_of_threads)
    {
        used_nr_of_threads = requested_nr_of_threads; //limit as user requested
        cout << "Used number of threads: " << used_nr_of_threads;
    }
    else
    {
        cout << "Used number of threads limited to max: " << used_nr_of_threads;
    }
                                        
    //CONSTRUCTION OF THREAD POOL
    thread_pool tp(used_nr_of_threads); 
    
    //tell the analyzed path
    {
        string s_parent_path = parent.string();
        cout << "Results for path: " << s_parent_path << endl;
    }

    //SET OF COUNTERS
    

    //FIND COUNTABLE (LINES,WORDS,LETTERS) FILES AND COUNT OVERALL NUMBER OF FILES - USED RECURSIVE ITERATOR
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ parent })
    {
        //CHECK IF NODE IS DIRECTORY OR FILE
        fs::path subpath(dir_entry.path());
        fs::file_status subpath_stat = fs::status(subpath);                     
        if (fs::is_directory(subpath_stat)) count_directories++;     //count up directories  
        else                                                                  
        {
            count_files++;                                            //count up files
 
            //ENQUEUE TASK TO ANALYZE A FILE 
            tp.enqueue([subpath] {

                //ANALYZE OF A FILE
                string pathname = subpath.string();
                fstream  reader;
                string buffer;			//to analyze line by line

                //READ LINES AND COUNT
                reader.open(pathname);
                while (!reader.eof())
                {
                    buffer.clear();
                    getline(reader, buffer, '\n');
                    C.lock();
                    if (!buffer.empty()) count_full_lines++;
                    else count_empty_lines++;
                    C.unlock();
                }
            });
        }
    }
    
}

int main()
{
   // cout << "Enter the path of analyzed directory: ";
    //cin >> s_parent_path;
    

    fs::path files{PATH_4};
    auto start = chrono::steady_clock::now();
    analyze_path(files,4);
    auto stop = chrono::steady_clock::now();

    count_lines = count_full_lines + count_empty_lines;
    cout << "There are: " << endl;
    cout << count_files << " files" << '\n';                    //print the result
    cout << count_directories << " directories" << '\n';
    cout << count_lines << " lines" << '\n';
    cout << count_full_lines << "full lines" << '\n';
    cout << count_empty_lines << "empty lines" << '\n';

    cout << "Number of used threads: " << NR_THREADS << endl;
    chrono::duration<double> elapsed_time = stop - start;
    cout << "Required time: " << elapsed_time.count() << endl;
    return 0;
}