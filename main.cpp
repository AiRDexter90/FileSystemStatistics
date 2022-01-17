#include <conio.h>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <gtest/gtest.h>
#include "thread_pool.h"

#define MY_TEST_PATH "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest"

using namespace std;
namespace fs = std::filesystem;

typedef unsigned int uint;
typedef unsigned long int ulint;
typedef unsigned short int usint;

//*************************************GLOBAL VARIABLES*******************************************

//a struct to keep all counters
struct counter
{
    //number of:
    ulint dirs;  //directories                   
    ulint files; //files
    ulint lines; //lines in file
    ulint full;  //full lines in file
    ulint empty; //empty lines in file
    ulint words; //words in file - interpreted as continous string of letters (range from a-z and A-Z)
    ulint letters; //letters in file - all chars from range a-z and A-Z
};

const usint max_nr_of_threads = thread::hardware_concurrency();                 //number of available threads to use
counter nr_of;                                                                  //instance of counter
mutex C;                                                                        //to guard shared data pieces


//***************************************FUNCTIONS************************************************
////sets up number of threads and guarantees limitation, if exceeded
usint set_nr_of_threads(usint requested_nr_of_threads)
{
    if (requested_nr_of_threads < max_nr_of_threads && requested_nr_of_threads > 0) return requested_nr_of_threads; //limit as user requested
    else if (requested_nr_of_threads <= 0) return 1;                                                                //threads must be positive
    else return max_nr_of_threads;                                                                                  //limit to max if exceed
}

///count up lines, words and letters
void counter_fun(const string & file_pathname)
{
    fstream  reader;
    string line_buffer;
    string word_buffer;         

    reader.open(file_pathname);
    while (!reader.eof())
    {
        line_buffer.clear();
        getline(reader, line_buffer, '\n');
        //THERE IS FULL LINE
        if (!line_buffer.empty())
        {
            size_t start = 0;
            size_t stop = 0;
            //COUNT WORDS
            do
            {
                //word is iterpreted as continous string of letters
                start = line_buffer.find_first_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM", stop);
                if (start == string::npos) break;
                stop = line_buffer.find_first_not_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM", start);
                word_buffer = line_buffer.substr(start, stop - start);
                C.lock();
                nr_of.words++; //count up words
                C.unlock();
                //COUNT LETTERS
                uint a = count_if(word_buffer.begin(), word_buffer.end(), [](char c) {
                    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) return true;   //ASCII codes
                    else return false;
                });

                C.lock();
                nr_of.letters += a; //count up letters
                C.unlock();

            } while (stop != string::npos);

            C.lock();
            nr_of.full++; //count up full lines
            C.unlock();
        }
        //EMPTY LINE
        else
        {
            C.lock();
            nr_of.empty++; //count up empty lines
            if (!nr_of.full) nr_of.empty = 0; //to avoid counting empty file as file with one empty line
            C.unlock();
        }
    }
}


//--------------------------------------------------------------------------------------------------------------------------
//analyze path and looks for files--------------------------------------------------------------------------
void analyze_path(const fs::path& parent, usint requested_nr_of_threads)
{
    //how many threads will be used
    usint used_nr_of_threads = set_nr_of_threads(requested_nr_of_threads);
    //construction of the thread pool
    thread_pool tp(used_nr_of_threads);

    //FIND COUNTABLE FILES AND COUNT OVERALL NUMBER OF FILES - USED RECURSIVE ITERATOR
    //use of filesystem library
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ parent })
    {
        //check if the node is a file or direcotctory
        fs::path subpath(dir_entry.path());
        fs::file_status subpath_stat = fs::status(subpath);
        if (fs::is_directory(subpath_stat)) nr_of.dirs++;     //count up directories  
        else
        {
            nr_of.files++;                                            //count up files
            //let enqueue an in file counting task 
            tp.enqueue([subpath] {
                cout << "start "<< endl;
                string pathname = subpath.string();
                counter_fun(pathname);                               //call counting of words,lines,letters
                cout << "stop" << endl;
            });
        }
    }
}

//TESTS ARE LOCATED UDER MAIN FUNCTION


//***********************************************MAIN FUNCTION*****************************************************************
int main()
{
    //................EXAMPLE PART OF PROGRAM TO SHOW IMPACT OF THE NUMBER OF USED THREADS.....................................
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    fs::path files{MY_TEST_PATH}; //PLACE WHERE YOU CAN ENTER YOUR OWN PATH INSTEAD OF MY_TEST_PATH
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    cout << "Maximum number of threads to use: " << max_nr_of_threads<<endl;
    cout << "Tested directory: \"DirectoriesTest\"" << endl;
    
    //sweep through different number of threads (from one to max) and time of all tasks execution 
    for (usint i = 1; i <= max_nr_of_threads; i++)
    {
        nr_of = { 0,0,0,0,0,0,0};//reset counters
        cout << "Performance results for " << i << " threads:" << endl;
        auto start = chrono::steady_clock::now();
        analyze_path(files, i);
        auto stop = chrono::steady_clock::now();
        chrono::duration<double> elapsed_time = stop - start;
        cout << "Required time: " << elapsed_time.count() << endl;
    }
    
    //results in counters
    nr_of.lines = nr_of.full + nr_of.empty;
    cout << "There are: " << endl;
    cout << nr_of.files << " files" << '\n';                    //print the nr_of
    cout << nr_of.dirs << " directories" << '\n';
    cout << nr_of.lines << " lines" << '\n';
    cout << nr_of.full << " full lines" << '\n';
    cout << nr_of.empty << " empty lines" << '\n';
    cout << nr_of.words << " words" << '\n';
    cout << nr_of.letters << " letters" << '\n';
    

//........................................TESTING PART OF PROGRAM.............................................
    testing::InitGoogleTest();
    RUN_ALL_TESTS();
    return 0;
}

//------------------------------------------EXAMPLE OF TESTS-----------------------------------------------------
//test for safe limiting the number of threads,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
TEST(ThreadLimits, ThreadLimitExceeded)
{
    ASSERT_TRUE(set_nr_of_threads(-23) <= max_nr_of_threads);
    ASSERT_TRUE(set_nr_of_threads(0) <= max_nr_of_threads);
    ASSERT_TRUE(set_nr_of_threads(2) <= max_nr_of_threads);
    ASSERT_TRUE(set_nr_of_threads(765753) <= max_nr_of_threads);
}

//counting of dirs and files tests,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
TEST(CountingFilesTest, EmptyDirectoryTest)
{
    nr_of = { 0,0,0,0,0,0,0 };//everything should remain zero for empty dir
    analyze_path("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\Empty", 4);
    EXPECT_EQ(nr_of.dirs, 0);
    EXPECT_EQ(nr_of.files, 0);
}

TEST(CountingFilesTest, TestRecursiveDirectory)
{
    nr_of = { 0,0,0,0,0,0,0 };
    analyze_path("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest", 4);
    EXPECT_EQ(nr_of.dirs, 12);
    EXPECT_EQ(nr_of.files, 11);
}

//counting in files,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
TEST(CountingInFileTest, EmptyFileTest) //everything should be zero for empty file
{
    nr_of = { 0,0,0,0,0,0,0 };
    counter_fun("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\TestFiles\\Empty.txt");
    EXPECT_EQ(nr_of.empty, 0);
    EXPECT_EQ(nr_of.full, 0);
    EXPECT_EQ(nr_of.letters, 0);
    EXPECT_EQ(nr_of.words, 0);
}

TEST(CountingInFileTest, BigFileOnlyWords)//large amount of text
{
    nr_of = { 0,0,0,0,0,0,0 }; 
    counter_fun("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\TestFiles\\LoremIpsum10K_Words.txt");
    EXPECT_EQ(nr_of.empty, 113);
    EXPECT_EQ(nr_of.full, 114);
    EXPECT_EQ(nr_of.letters, 55683);
    EXPECT_EQ(nr_of.words, 10000);
}

TEST(CountingInFileTest, MixedNumbersWords)//not direct words
{
    nr_of = { 0,0,0,0,0,0,0 };
    counter_fun("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\TestFiles\\MixedWordsAndNumbers.txt");
    EXPECT_EQ(nr_of.empty, 1);
    EXPECT_EQ(nr_of.full, 4);
    EXPECT_EQ(nr_of.letters, 74);
    EXPECT_EQ(nr_of.words, 18);
}