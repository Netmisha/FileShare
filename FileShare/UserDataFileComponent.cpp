
#include <Windows.h>
#include <regex>
#include <utility>

#include "UserDataFileComponent.h"
#include "tinyxml.h"

using namespace FileShare;

UserData::UserData(const String& als, const UserAddr & adr, UserStatus sts):
    alias(als),
    address(adr),
    status(sts)
{}

UserData::operator String() const
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

bool FileShare::UserData::operator==(const UserData & obj)const
{
    return alias == obj.alias && address == obj.address && status==obj.status;
}



String UserData::to_str(UserAddr){
    return "default addr";
}
String UserData::to_str(UserStatus sts){
    if (sts == UserStatus::Self)    return UserStatusConsts().Self;
    if (sts == UserStatus::Good)    return UserStatusConsts().Good;
    if (sts == UserStatus::Bad)     return UserStatusConsts().Bad;
    /*(str == UserStatus::Ugly)*/   return UserStatusConsts().Ugly;
}
UserData::UserStatus UserData::str_to_status(const String& str)
{
    if (str == UserStatusConsts().Self)  return UserStatus::Self;
    if (str == UserStatusConsts().Good)  return UserStatus::Good;
    if (str == UserStatusConsts().Bad)   return UserStatus::Bad;
    /*(str == UserStatusConsts().Ugly)*/ return UserStatus::Ugly;
}

UDFComponent::UDFComponent(){
    UserDataFileSetup();
}

UserData ConvertTiXmlElementToUserData(const TiXmlElement*);
TiXmlElement* ConvertUserDataToTiXmlElement(const UserData&);

bool UDFComponent::AppendUser(const UserData& usr){
    if (UserAlreadyExists(usr))
        return false;

    auto udf = TiXmlDocument("UDF.xml");
    if(!udf.LoadFile())
        return false;

    auto root = udf.RootElement();
    auto elem = ConvertUserDataToTiXmlElement(usr);
    root->LinkEndChild(elem);

    return true;
}

void UDFComponent::UserDataFileSetup()
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

/**
    find user in data file
    converts xmlelemet* to userdata
*/
bool UDFComponent::UserAlreadyExists(const UserData& usr)
{
    TiXmlDocument udf("UDF.xml");
    udf.LoadFile();

    auto root = udf.RootElement();

    for (auto iter = root->FirstChildElement("user"); iter; iter = iter->NextSiblingElement("user"))
        if(ConvertTiXmlElementToUserData(iter) == usr)
            return true;

    return false;
}

/**
xml element -> to user data
can be chaged later
*/
UserData ConvertTiXmlElementToUserData(const TiXmlElement* element) {
    auto alias = element->Attribute("alias");
    auto address = UserData::UserAddr(element->Attribute("address"));
    auto status = UserData::str_to_status(element->Attribute("status"));
    return UserData(alias, address, status);
}

/**
user data -> xml element
can be chaged later
*/
TiXmlElement* ConvertUserDataToTiXmlElement(const UserData& usr) {
    auto alias   = usr.alias;
    auto address = UserData::to_str(usr.address);
    auto status  = UserData::to_str(usr.status);

    auto element = new TiXmlElement("user");
    
    element->SetAttribute("alias", alias);
    element->SetAttribute("address", address);
    element->SetAttribute("status", status);

    return element;
}

