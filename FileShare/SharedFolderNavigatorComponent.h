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
        virtual FileVector GetFileList() = 0;
    };

    class SharedFolderNavigatorSelf:
        protected BasicSharedFolderNavigator
    {
    public:
        SharedFolderNavigatorSelf(); // find out if exists, create if doesnt

        virtual FileVector GetFileList() override; 

        String FileCreate(const String& fileName);
        BOOL   FileDelete(const String& fileName);
        BOOL   FileOpen  (const String& fileName); // text - notepad, picture - paint, music/video - wmplayer
        BOOL   FileExists(const String& fileName);
        BOOL   FileRename(const String& nameOld, const String& nameNew);

    protected:
        String FileCreateUniqueName(const String&);
        
        BOOL SharedFolderExists();
        BOOL CreateSharedFolder();
        
        const String& ModuleDirectoryPath();
        const String& SharedFolderPath();
        
        FileVector FilesInDirectory(const String&);

        const String sharedFolderName;

        friend class SharedFolderNavigatorComponent;
    };

    class SharedFolderNavigatorOther:
    protected BasicSharedFolderNavigator
    {
    public:
        virtual FileVector GetFileList() override;

        BOOL SendFileToUser();
        BOOL ReceiveFileFromUser();
    };


    class SharedFolderNavigatorComponent{
    public:
        SharedFolderNavigatorSelf& self;
        SharedFolderNavigatorOther& other;
    };
}