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
    fs::path aPath{ "C:\\Users\\Piotr\\source\\repos\\Filesystem\\Text.txt" };

    cout << "Parent path: " << aPath.parent_path() << endl;
    cout << "Filename: " << aPath.filename() << endl;
    cout << "Extension: " << aPath.extension() << endl;
    //cout << ios::boolalpha << aPath.empty() << endl;

    const fs::path files{ "C:\\Users\\Piotr\\Desktop\\PROGRAMOWANIE\\C++\\FileSystem" };
    std::filesystem::create_directories(files / "dir1" / "dir2");
    std::filesystem::create_directories(files / "dir11" / "dir12"/"dir13");
    std::ofstream{ files / "file1.txt" };
    std::ofstream{ files / "file2.txt" };
    std::ofstream{ files / "dir11" / "file11.txt" };

    std::cout << "directory_iterator:\n";
    for (auto const& dir_entry : std::filesystem::directory_iterator{ files })
        cout << dir_entry.path() << '\n';

    std::cout << "recursive_directory_iterator:\n";
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ files })
    {
        fs::path subpath(dir_entry.path()); //sciezki podkatalogow lub plikow
        std::cout << subpath << '\n';           //wypisz sciezke podkatalogu lub pliku
        fs::file_status subpath_stat = fs::status(subpath);
        if(fs::is_directory(subpath_stat)) std::cout <<"Katalog"<< '\n';
        else 
        {
            std::cout << "Plik" << '\n'; counter++;
        }
    } 
    cout << "Jest " << counter << " plikow"  <<'\n';
    std::filesystem::remove_all(files);

	return 0;
}