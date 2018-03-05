#pragma once
#include <Windows.h>
#include <string>
#include <vector>



namespace FileShare {
    using String = std::string;
    using FileVector = std::vector<std::string>;

    class BasicSharedFolderNavigator {
        /*
            donno if i actualy need this base yet
        */
    };

    class SharedFolderNavigatorSelf:
        protected BasicSharedFolderNavigator
    {
    public:
        SharedFolderNavigatorSelf(); // find out if exists, create if doesnt

        FileVector GetFileList(); 

        void FileCreate(const String&);
        bool FileRename(const String&, const String&);
        bool FileDelete(const String&);
        bool FileOpen(const String&); // text - notepad, picture - paint, music/video - wmplayer

    protected:
        String FileCreateName(const String&);

        bool FileExists(const String&);
        bool SharedFolderExists();
        bool CreateSharedFolder();

        const String& ModuleDirectoryPath();
        const String& SharedFolderPath();

        FileVector FilesInDirectory(const String&);

        const String sharedFolderName;

        friend class SharedFolderNavigatorComponent;
    };

    class SharedFolderNavigatorOther:
    protected BasicSharedFolderNavigator
    {

    };


    class SharedFolderNavigatorComponent{

    
    };
}