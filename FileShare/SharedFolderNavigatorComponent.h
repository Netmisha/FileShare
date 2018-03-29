#pragma once
#include"SharedFolderNavigatorSelf.h"
#include "SharedFolderNavigatorOther.h"
#include "SharedFolderNavigatorOther20.h"

namespace FileShare {
    using SfnSelf = SharedFolderNavigatorSelf;
    using SfnOther = SharedFolderNavigatorOther;

    class SharedFolderNavigatorComponent{
    public:
        SharedFolderNavigatorComponent() = default;
        SharedFolderNavigatorComponent(USHORT, USHORT);

        SharedFolderNavigatorSelf self;
        //SharedFolderNavigatorOther other;
        SharedFolderNavigatorOther20 other20;
    };

    using SfnComponent = SharedFolderNavigatorComponent;
}