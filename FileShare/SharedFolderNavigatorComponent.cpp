
#include <thread>
#include <string>
#include <vector>

#include "SharedFolderNavigatorComponent.h"

using namespace FileShare;
using Vector = std::vector<std::string>;

SharedFolderNavigatorComponent::SharedFolderNavigatorComponent(USHORT p1, USHORT p2) :
    other(p1, p2)
{}
