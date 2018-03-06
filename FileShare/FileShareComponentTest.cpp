#include <Windows.h>
#include <iostream>
#include <string>

#include "SharedFolderNavigatorComponent.h"

#ifdef _DEBUG
#define TEST main
void Cerr(const std::string& message);
#define IN_RED Cerr
#define TO_STR(s) std::string(#s)


#define TEST_SHARED_FOLDER_NAVIGATOR_SELF   1
#define TEST_USER_DATA_FILE_COMPONENT       2

#ifndef CURRENT_TEST
#define CURRENT_TEST TEST_USER_DATA_FILE_COMPONENT
#endif

#if defined CURRENT_TEST
#if defined TEST_USER_DATA_FILE_COMPONENT
#if CURRENT_TEST == TEST_USER_DATA_FILE_COMPONENT

int TEST() {
    std::cout << TO_STR(##CURRENT_TEST) << std::endl;



    return system("pause");
}
#endif
#endif
#endif

#if defined CURRENT_TEST
#if defined TEST_SHARED_FOLDER_NAVIGATOR_SELF 
#if CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_SELF

using namespace FileShare;
int TEST() {
    //for (int i = 0; i < 256; ++i) {
    //    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
    //    std::cout << "i = " << i << std::endl;
    //}


    std::cout << "hello, ";
    IN_RED("world\n");
    
    SharedFolderNavigatorSelf self;
    IN_RED("instance created");

    String paintmeName = self.FileCreate("paintme.bmp");
    if (paintmeName != "paintme.bmp") {
        IN_RED("paintme.bmp already existed.");
        IN_RED("new paintme is created as " + paintmeName);
        IN_RED("gonna rename " + paintmeName + " somehow");
        self.FileRename(paintmeName, "new_paintme_name_with_blackjack_and_hookers.bmp");
    }
    else
        IN_RED("paintme.bmp created");
    {
        FileVector fv = self.GetFileList();
        if (fv.empty())
            IN_RED("vector empty, smth wrong");

        for (const String& fileName : fv)
            std::cout << fileName << std::endl;
    }

    String fileName;
    std::cout << "file name pls" << std::endl;
    std::cin >> fileName;

    IN_RED("opening " + fileName);
    self.FileOpen(fileName);

    IN_RED("opened file s gonna be deleted afterwards");
    system("pause");

    self.FileDelete(fileName);

    {
        FileVector fv = self.GetFileList();
        for (const String& fileName : fv)
            std::cout << fileName << std::endl;
    }   
    return system("pause");
}
#endif
#endif
#endif


////////
void Cerr(const std::string& message) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

    std::cerr << message << std::endl;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
#endif