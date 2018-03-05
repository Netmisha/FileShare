#include "SharedFolderNavigatorComponent.h"

#include <Windows.h>
#include <thread>
#include <string>
#include <vector>
#include <regex>

using namespace FileShare;
using String = std::string;
using Vector = std::vector<std::string>;


FileShare::SharedFolderNavigatorSelf::SharedFolderNavigatorSelf():
    sharedFolderName("SharedFolder\\")
{
    if (!SharedFolderExists())
        CreateSharedFolder();
}

FileVector SharedFolderNavigatorSelf::GetFileList(){
    return FilesInDirectory(SharedFolderPath());   
}

void SharedFolderNavigatorSelf::FileCreate(const String& fileNameUnique)
{
    String fileName = FileCreateName(fileNameUnique);
    String filePath = SharedFolderPath() + fileName;
    HANDLE mafile = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, 7/*1+2+4=all*/, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(mafile);
}

bool SharedFolderNavigatorSelf::FileRename(const String& fileNameOld, const String& fileNameNew)
{
    if (!FileExists(fileNameOld))
        return;

    String newName = FileCreateName(fileNameNew);

    String filePathOld = SharedFolderPath() + fileNameOld;
    String filePathNew = SharedFolderPath() + newName;

    return MoveFile(filePathOld.c_str(), filePathNew.c_str());
}

bool SharedFolderNavigatorSelf::FileDelete(const String& fileName)
{
    bool rat = false;

    if (FileExists(fileName)) {
        String filePath = SharedFolderPath() + fileName;
        rat = DeleteFile(filePath.c_str());
    }

    return rat;
}

bool SharedFolderNavigatorSelf::FileOpen(const String& fileName)
/*
    User can open some files from shared folder
    Files are opened via system(start...) in separate threads
    current possibilities:
        text files - notepad
        pictures - mspaint
        music/video - wmplayer
*/
{
    bool rat = false;

    if (FileExists(fileName)) {
        String filePath = SharedFolderPath() + fileName;
        String executor;

        if (std::regex_match(filePath, std::regex(".+[.](txt|text|readme)")))
            executor = "notepad";

        if (std::regex_match(filePath, std::regex(".+[.](bmp|jpeg|gif|png)")))
            executor = "mspaint";

        if (std::regex_match(filePath, std::regex(".+[.](mp3|avi)")))
            executor = "wmplayer.exe -p";

        String command = "start " + executor + " " + filePath;

        std::thread th([=]() {system(command.c_str());});
        th.detach();

        rat = true;
    }

    return rat;
}

FileVector FileShare::SharedFolderNavigatorSelf::FilesInDirectory(const String& dir)
{
    WIN32_FIND_DATA findData;
    Vector foundFiles;
    HANDLE fileIter = FindFirstFile((dir + "*.*").c_str(), &findData);
    if (fileIter != INVALID_HANDLE_VALUE) {
        do
            foundFiles.push_back(String(findData.cFileName));
        while (FindNextFile(fileIter, &findData));
    }
    return foundFiles;
}

String FileShare::SharedFolderNavigatorSelf::FileCreateName(const String& fileName)
{
    String newName = fileName;
    Vector names = GetFileList();
    int i = 0;
    while (FileExists(newName)) {
        newName = std::regex_replace(fileName, std::regex("[.]"), "("+std::to_string(i)+")");
    }
    return newName;
}

bool NameInVector(const Vector& v, const String& s) 
/*
    helper function that doesnt rly need to be part of the class
*/
{
    return std::find(v.begin(), v.end(), s) != v.end();
}

bool SharedFolderNavigatorSelf::FileExists(const String& fileName)
{
    Vector names = GetFileList();
    return NameInVector(names, fileName);
}

bool SharedFolderNavigatorSelf::SharedFolderExists()
{
    Vector foundFiles = FilesInDirectory(ModuleDirectoryPath());

    return NameInVector(foundFiles, sharedFolderName);
}

bool SharedFolderNavigatorSelf::CreateSharedFolder()
{
    return CreateDirectory((SharedFolderPath()).c_str(), NULL);
}

const String& SharedFolderNavigatorSelf::ModuleDirectoryPath()
/*
    madule directory path is a static string 
    that is searched for once if empty:
        we get directory out program instance is runnig from
        cut *.exe off
*/
{
    static String directoryPathShort;

    if (directoryPathShort.empty()) 
    {
        TCHAR tmodulePath[MAX_PATH]{};
        GetModuleFileName(NULL, tmodulePath, MAX_PATH);

        String directoryPath = std::regex_replace(tmodulePath, std::regex("([^\\\\]+[.]exe)"), "");

        TCHAR tdirectPathShort[MAX_PATH]{};
        GetShortPathName(directoryPath.c_str(), tdirectPathShort, MAX_PATH);

        directoryPathShort = String(tdirectPathShort);
    }

    return directoryPathShort;
}

const String& SharedFolderNavigatorSelf::SharedFolderPath(){
    static String folderPath;

    if (folderPath.empty())
        folderPath = ModuleDirectoryPath() + sharedFolderName;

    return folderPath;
}
