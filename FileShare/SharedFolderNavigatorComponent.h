#pragma once
#include"SharedFolderNavigatorSelf.h"
#include "SharedFolderNavigatorOther.h"

namespace FileShare {
    using SfnSelf = SharedFolderNavigatorSelf;
    using SfnOther = SharedFolderNavigatorOther;

    class SharedFolderNavigatorComponent{
    public:
        SharedFolderNavigatorComponent() = default;
        SharedFolderNavigatorComponent(USHORT, USHORT);

        SharedFolderNavigatorSelf self;
        SharedFolderNavigatorOther other;
    };

    using SfnComponent = SharedFolderNavigatorComponent;
}