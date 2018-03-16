#pragma once
#include "BasicSharedFolderNavigator.h"

namespace FileShare {

    class SharedFolderNavigatorSelf :
        protected BasicSharedFolderNavigator
    {
    public:
        SharedFolderNavigatorSelf(); // find out if exists, create if doesnt

        FileVector GetFileList();

        /**
            since function treats provided name as suggestion
            and doesnt guarantee much conformity with it
            newly created file name is returned
        */
        String FileCreate(const String& fileName);
        Bool   FileDelete(const String& fileName);
        /*
            User can open some files from shared folder
            Files are opened via system(start...) in separate threads
            current possibilities:
            text files - notepad
            pictures - mspaint
            music/video - wmplayer
        */
        Bool   FileOpen  (const String& fileName); // text - notepad, picture - paint, music/video - wmplayer
        Bool   FileExists(const String& fileName);
        Bool   FileRename(const String& nameOld, const String& nameNew);

    protected:
        /**
            to protect ourselves from files with same names replacing others etc
            we create new names for files with names already existing
        */
        String FileCreateUniqueName(const String&);

        Bool SharedFolderExists();
        Bool CreateSharedFolder();

        /**
            madule directory path is a static string
            that is searched for once if empty:
            we get directory our program instance is runnig from
            also path is converted to dos-path to extinguish whitespaces
        */
        const String& ModuleDirectoryPath();
        const String& SharedFolderPath();

        /**
            should does skip hidden files
        */
        FileVector FilesInDirectory(const String&);

        const String sharedFolderName;

        friend class SharedFolderNavigatorComponent;
    };
}
