#pragma once
#include"SharedFolderNavigatorSelf.h"
#include "SharedFolderNavigatorOther20.h"

namespace FileShare 
{
    class SharedFolderNavigatorComponent
    {
    public:
        SharedFolderNavigatorComponent() = default;
        SharedFolderNavigatorComponent(USHORT);

        SharedFolderNavigatorSelf self;
        SharedFolderNavigatorOther20 other20;
    };

    using SfnComponent = SharedFolderNavigatorComponent;
}