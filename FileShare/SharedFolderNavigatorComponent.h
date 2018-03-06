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

        String FileCreate(const String& fileName);
        bool   FileDelete(const String& fileName);
        bool   FileOpen  (const String& fileName); // text - notepad, picture - paint, music/video - wmplayer
        bool   FileExists(const String& fileName);
        bool   FileRename(const String& nameOld, const String& nameNew);


    protected:
        String FileCreateUniqueName(const String&);
        
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
    public:
        SharedFolderNavigatorSelf& self;
        SharedFolderNavigatorOther& other;
    };
}