#include <Windows.h>
#include <iostream>

#include <regex>
#include <string>
#include <vector>
#include <thread>


/*
    for FileShare startup FolderNavigatorComponent 
    is required to search its dir
    for SharedFolder and UserDataFile.

    if either is missing, it should be created on spot

    also UserDataFileComponent is ought to retreive UserName
    for SELF or Stage::Acquaintance or smth is gonna be triggered
*/

using String = std::string;
using Regex = std::basic_regex<TCHAR>;

int main() {
    TCHAR tmodulePath[MAX_PATH]{};
    GetModuleFileName(NULL, tmodulePath, MAX_PATH);

    String directPath = std::regex_replace(tmodulePath, std::regex("([^\\\\]+\\.exe)"), "");
        
    TCHAR tdirectPathShort[MAX_PATH]{};
    GetShortPathName(directPath.c_str(), tdirectPathShort, MAX_PATH);

    String directPathShort(tdirectPathShort);

    std::cout << tmodulePath << std::endl;
    std::cout << directPath << std::endl;
    std::cout << directPathShort << std::endl;

    directPath = directPathShort;
    
    // find files in dir
    WIN32_FIND_DATA findData{};
    std::vector<std::string> foundFiles;
    HANDLE fileIter = FindFirstFile((directPath+"*.*").c_str(), &findData);
    if (fileIter != INVALID_HANDLE_VALUE) {
        do {
            foundFiles.push_back(std::string(findData.cFileName));
        } while (FindNextFile(fileIter, &findData));
    }
    else
        std::cout << "handle error" << std::endl;
    
    // check shared folder existance
    std::string sharedFolderName("SharedFolder");
    bool sharedFolderExists = std::find(foundFiles.begin(), foundFiles.end(), sharedFolderName) != foundFiles.end();

    std::for_each(foundFiles.begin(), foundFiles.end(), 
        [](std::string& s) {
            std::cout << s << std::endl;
        }
    );

    std::cout << sharedFolderName << " " << (sharedFolderExists ? "exists" : "doesn't exist") << std::endl;
    if (!sharedFolderExists) {
        std::cout << "creating " << sharedFolderName << std::endl;
        CreateDirectory((directPath + sharedFolderName).c_str(), NULL);
    }

    // try opening some files for editing
    std::string fileName =  
        //"writeme.txt";
        "paintme.bmp";
        //"smoke.mp3";
    //std::cin >> fileName;

    std::string filePath = /*"\"" + */directPath + sharedFolderName + "\\" + fileName/* + "\""*/;

    std::cout << filePath << std::endl;

    if(std::regex_match(filePath, std::regex(".+[.](txt|text|readme)")))
        system(("notepad " + filePath + "&").c_str());
    
    if (std::regex_match(filePath, std::regex(".+[.](bmp|jpeg|gif|png)"))) {
        std::thread th([=]() {system(("start mspaint " + filePath).c_str());});
        th.detach();        
    }
    if (std::regex_match(filePath, std::regex(".+[.](mp3)")))
        system(("start wmplayer.exe -p " + filePath).c_str());

    
    //ShellExecute(NULL, "edit", filePath.c_str(), NULL, NULL, SW_SHOW);

    String macreation = "macreation.txt";
    HANDLE mafile = CreateFile(macreation.c_str(), GENERIC_READ | GENERIC_WRITE, 7, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(mafile);
    system("start notepad macreation.txt");

    system("pause");
    DeleteFile(macreation.c_str());

    return system("pause");
}
