#pragma once
#include <string>
#include <vector>

namespace FileShare {
    using String = std::string;
    using FileVector = std::vector<std::string>;
    using Bool = int;

    class BasicSharedFolderNavigator {
        virtual FileVector GetFileList() = 0;
    };

    class SharedFolderNavigatorSelf:
        protected BasicSharedFolderNavigator
    {
    public:
        SharedFolderNavigatorSelf(); // find out if exists, create if doesnt

        virtual FileVector GetFileList() override; 

        String FileCreate(const String& fileName);
        Bool   FileDelete(const String& fileName);
        Bool   FileOpen  (const String& fileName); // text - notepad, picture - paint, music/video - wmplayer
        Bool   FileExists(const String& fileName);
        Bool   FileRename(const String& nameOld, const String& nameNew);

    protected:
        String FileCreateUniqueName(const String&);
        
        Bool SharedFolderExists();
        Bool CreateSharedFolder();
        
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

        Bool SendFileToUser();
        Bool ReceiveFileFromUser();
    };

    class SharedFolderNavigatorComponent{
    public:
        SharedFolderNavigatorSelf& self;
        SharedFolderNavigatorOther& other;
    };
}