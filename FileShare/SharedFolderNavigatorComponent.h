#pragma once
#include"SharedFolderNavigatorSelf.h"
#include "SharedFolderNavigatorOther.h"

namespace FileShare {
    using SfnSelf = SharedFolderNavigatorSelf;
    using SfnOther = SharedFolderNavigatorOther;

    class SharedFolderNavigatorComponent{
    public:
        SfnSelf& self;
        SfnOther& other;
    };

    using SfnComponent = SharedFolderNavigatorComponent;
}