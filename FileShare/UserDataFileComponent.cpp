
#include <Windows.h>
#include <regex>
#include <utility>

#include "UserDataFileComponent.h"

FileShare::UserData::UserData(const String& als, const UserAddr & adr, UserStatus sts):
    alias(als),
    address(adr),
    status(sts)
{}

FileShare::UserData::operator FileShare::String() const
{
    String format = {
        "<user>" "\n"
            "<name>%name</name>" "\n"
            "<address>%addr</address>" "\n"
            "<status>%status<status>" "\n"
        "</user>"
    };

    for (auto p :std::vector<std::pair<std::regex, std::string>>
        {
        std::pair<std::regex, std::string>{std::regex("%name"), alias},
        std::pair<std::regex, std::string>{std::regex("%addr"), to_str(address)},
        std::pair<std::regex, std::string>{std::regex("%status%"), to_str(status)}
        }
    ) {
        format = std::regex_replace(format, p.first, p.second);
    }

    return format;
}

FileShare::String FileShare::UserData::to_str(UserStatus status){
    switch (status){
        case UserData::UserStatus::Self:    return "self";
        case UserData::UserStatus::Good:    return "good";
        case UserData::UserStatus::Bad:     return "bad" ;
        case UserData::UserStatus::Ugly:    return "ugly";
    }
}

FileShare::String FileShare::UserData::to_str(UserAddr){
    return "default addr";
}

FileShare::UDFComponent::UDFComponent(){
    UserDataFileSetup();
}

void FileShare::UDFComponent::UserDataFileSetup()
{
    CreateFile(
        "UDF.xml",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_HIDDEN,
        NULL
    );
}
