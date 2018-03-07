#include <Windows.h>
#include <thread>
#include <string>
#include <vector>
#include <regex>

#include "SharedFolderNavigatorComponent.h"

using namespace FileShare;
using Vector = std::vector<std::string>;

SharedFolderNavigatorSelf::SharedFolderNavigatorSelf():
    sharedFolderName("SharedFolder\\")
{
    if (!SharedFolderExists())
        CreateSharedFolder();
}

FileVector SharedFolderNavigatorSelf::GetFileList(){
    return FilesInDirectory(SharedFolderPath());   
}

/**
since function treats provided name as suggestion
and doesnt guarantee much conformity with it
newly created file name is returned
*/
String SharedFolderNavigatorSelf::FileCreate(const String& fileName)
{
    String newName = FileCreateUniqueName(fileName);
    String filePath = SharedFolderPath() + newName;
    HANDLE mafile = CreateFile(
        filePath.c_str(), 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    CloseHandle(mafile);
    return newName;
}

BOOL SharedFolderNavigatorSelf::FileRename(const String& fileNameOld, const String& fileNameNew)
{
    if (!FileExists(fileNameOld))
        return false;

    String newName = FileCreateUniqueName(fileNameNew);

    String filePathOld = SharedFolderPath() + fileNameOld;
    String filePathNew = SharedFolderPath() + newName;

    return MoveFile(filePathOld.c_str(), filePathNew.c_str());
}

BOOL SharedFolderNavigatorSelf::FileDelete(const String& fileName)
{
    if (FileExists(fileName)) {
        String filePath = SharedFolderPath() + fileName;
        return DeleteFile(filePath.c_str());
    }

    return FALSE;
}

BOOL SharedFolderNavigatorSelf::FileOpen(const String& fileName)
/*
    User can open some files from shared folder
    Files are opened via system(start...) in separate threads
    current possibilities:
        text files - notepad
        pictures - mspaint
        music/video - wmplayer
*/
{
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

        return TRUE;
    }
    return FALSE;
}

/**
    does skip hidden files
*/
FileVector FileShare::SharedFolderNavigatorSelf::FilesInDirectory(const String& dir)
{
    WIN32_FIND_DATA findData;
    Vector foundFiles;
    HANDLE fileIter = FindFirstFile((dir + "*.*").c_str(), &findData);
    if (fileIter != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                continue;
            else
                foundFiles.push_back(String(findData.cFileName));
        }
        while (FindNextFile(fileIter, &findData));
    }
    return foundFiles;
}

/**
helper function that doesnt rly need to be part of the class
*/
BOOL NameInVector(const Vector& v, const String& s) 
{
    return std::find(v.begin(), v.end(), s) != v.end();
}

/**
to protect ourselves from files with same names replacing others etc
we create new names for files with names already existing
*/
String FileShare::SharedFolderNavigatorSelf::FileCreateUniqueName(const String& fileName)
{
    String newName = fileName;
    Vector names = GetFileList();
    int i = 1;
    while (NameInVector(names, newName)) {
        newName = std::regex_replace(fileName, std::regex("[.]"), "("+std::to_string(i)+").");
        ++i;
    }
    return newName;
}

BOOL SharedFolderNavigatorSelf::FileExists(const String& fileName)
{
    Vector names = GetFileList();
    return NameInVector(names, fileName);
}

BOOL SharedFolderNavigatorSelf::SharedFolderExists()
{
    Vector foundFiles = FilesInDirectory(ModuleDirectoryPath());
    return NameInVector(foundFiles, sharedFolderName);
}

BOOL SharedFolderNavigatorSelf::CreateSharedFolder()
{
    return CreateDirectory((SharedFolderPath()).c_str(), NULL);
}

/**
madule directory path is a static string
that is searched for once if empty:
we get directory our program instance is runnig from
also path is converted to dos-path to extinguish whitespaces
*/
const String& SharedFolderNavigatorSelf::ModuleDirectoryPath()
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
