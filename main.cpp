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

#define NR_THREADS 4
#define MY_TEST_PATH "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest"


using namespace std;
namespace fs = std::filesystem;

typedef unsigned int uint;
typedef unsigned long int ulint;
typedef unsigned short int usint;

//string s_parent_path;
//*************************************GLOBAL VARIABLES*******************************************
const usint max_nr_of_threads = thread::hardware_concurrency(); //number of available threads to use

struct counter
{
    ulint dirs;
    ulint files;
    ulint lines;
    ulint full;
    ulint empty;
    ulint words;
    ulint letters;
};

counter nr_of;
mutex C;

//***************************************FUNCTIONS************************************************
void counter_fun(const string & file_pathname)
{
    fstream  reader;
    string line_buffer;			//to analyze line by line
    string word_buffer;
    reader.open(file_pathname);
    while (!reader.eof())
    {
        line_buffer.clear();
        getline(reader, line_buffer, '\n');
        
        if (!line_buffer.empty())
        {
            size_t start = 0;
            size_t stop = 0;
            do
            {
                start = line_buffer.find_first_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM", stop);
                if (start == string::npos) break;
                stop = line_buffer.find_first_not_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM", start);
                word_buffer = line_buffer.substr(start, stop - start);
                C.lock();
                nr_of.words++;
                C.unlock();
                uint a = count_if(word_buffer.begin(), word_buffer.end(), [](char c) {
                    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) return true;
                    else return false;
                });

                C.lock();
                nr_of.letters += a;
                C.unlock();

            } while (stop != string::npos);
            C.lock();
            nr_of.full++;
            C.unlock();
        }
        else
        {
            C.lock();
            nr_of.empty++;
            if (!nr_of.full) nr_of.empty = 0;
            C.unlock();
        }
    }
}
//-----------------------------------------------------------------------------------------------
usint set_nr_of_threads(usint requested_nr_of_threads)
{
    //LIMITATION OF THREADS
    if (requested_nr_of_threads < max_nr_of_threads && requested_nr_of_threads > 0) return requested_nr_of_threads; //limit as user requested
    else if(requested_nr_of_threads <= 0) return 1;
    else return max_nr_of_threads;
}

//--------------------------------------------------------------------------------------------------------------------------
//MAIN FUNCTION-------------------------------------------------------------------------------------------------------------
void analyze_path(const fs::path& parent, usint requested_nr_of_threads)
{
    usint used_nr_of_threads = set_nr_of_threads(requested_nr_of_threads);
    //CONSTRUCTION OF THREAD POOL
    thread_pool tp(used_nr_of_threads);

    //SET OF COUNTERS


    //FIND COUNTABLE (LINES,WORDS,LETTERS) FILES AND COUNT OVERALL NUMBER OF FILES - USED RECURSIVE ITERATOR
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ parent })
    {
        //CHECK IF NODE IS DIRECTORY OR FILE
        fs::path subpath(dir_entry.path());
        fs::file_status subpath_stat = fs::status(subpath);
        if (fs::is_directory(subpath_stat)) nr_of.dirs++;     //count up directories  
        else
        {
            nr_of.files++;                                            //count up files

            //ENQUEUE TASK TO ANALYZE A FILE 
            tp.enqueue([subpath] {

                //ANALYZE OF A FILE
                string pathname = subpath.string();
                //READ LINES AND COUNT
                counter_fun(pathname);
            });
        }
    }

}

//------------------------------------------TESTS-----------------------------------------------------
TEST(ThreadLimits, ThreadLimitExceeded)
{
    ASSERT_TRUE(set_nr_of_threads(-23) <= max_nr_of_threads);
    ASSERT_TRUE(set_nr_of_threads(0) <= max_nr_of_threads);
    ASSERT_TRUE(set_nr_of_threads(2) <= max_nr_of_threads);
    ASSERT_TRUE(set_nr_of_threads(765753) <= max_nr_of_threads);
}
TEST(CountingFilesTest,EmptyDirectoryTest)
{
    nr_of = { 0,0,0,0,0,0,0 };
    analyze_path("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\Empty",4);
    EXPECT_EQ(nr_of.dirs, 0);
    EXPECT_EQ(nr_of.files, 0);
}

TEST(CountingFilesTest,TestRecursiveDirectory)
{
    nr_of = { 0,0,0,0,0,0,0 };
    analyze_path("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest", 4);
    EXPECT_EQ(nr_of.dirs, 12);
    EXPECT_EQ(nr_of.files, 11);
}

TEST(CountingInFileTest, EmptyFileTest)
{
    nr_of = { 0,0,0,0,0,0,0 };
    counter_fun("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\TestFiles\\Empty.txt");
    EXPECT_EQ(nr_of.empty,0);
    EXPECT_EQ(nr_of.full,0);
    EXPECT_EQ(nr_of.letters,0);
    EXPECT_EQ(nr_of.words,0);
}

TEST(CountingInFileTest,BigFileOnlyWords)
{
    nr_of = { 0,0,0,0,0,0,0 };
    counter_fun("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\TestFiles\\LoremIpsum10K_Words.txt");
    EXPECT_EQ(nr_of.empty, 113);
    EXPECT_EQ(nr_of.full, 114);
    EXPECT_EQ(nr_of.letters, 55683);
    EXPECT_EQ(nr_of.words, 10000);
}

TEST(CountingInFileTest, MixedNumbersWords)
{
    nr_of = { 0,0,0,0,0,0,0 };
    counter_fun("C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\DirectoriesTest\\TestFiles\\MixedWordsAndNumbers.txt");
    EXPECT_EQ(nr_of.empty, 1);
    EXPECT_EQ(nr_of.full, 4);
    EXPECT_EQ(nr_of.letters, 74);
    EXPECT_EQ(nr_of.words, 18);
}

int main()
{
    //................EXAMPLE PART OF PROGRAM TO SHOW IMACT OF THE NUMBER OF USED THREADS.....................................

  /*  fs::path files{MY_TEST_PATH};
    
    cout << "Maximum numberof threads to use: " << max_nr_of_threads<<endl;
    cout << "Test on directory \"DirectoriesTest\"" << endl;
    for (usint i = 1; i <= max_nr_of_threads; i++)
    {
        nr_of = { 0,0,0,0,0,0,0};
        cout << "Performance results for " << i << " threads:" << endl;
        auto start = chrono::steady_clock::now();
        analyze_path(files, i);
        auto stop = chrono::steady_clock::now();
        chrono::duration<double> elapsed_time = stop - start;
        cout << "Required time: " << elapsed_time.count() << endl;
    }
    
    nr_of.lines = nr_of.full + nr_of.empty;
    cout << "There are: " << endl;
    cout << nr_of.files << " files" << '\n';                    //print the nr_of
    cout << nr_of.dirs << " directories" << '\n';
    cout << nr_of.lines << " lines" << '\n';
    cout << nr_of.full << " full lines" << '\n';
    cout << nr_of.empty << " empty lines" << '\n';
    cout << nr_of.words << " words" << '\n';
    cout << nr_of.letters << " letters" << '\n';
    */

    //................TESTING PART OF PROGRAM.....................................
    testing::InitGoogleTest();
    RUN_ALL_TESTS();

    return 0;
}