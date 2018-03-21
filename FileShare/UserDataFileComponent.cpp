#include <Windows.h>
#include <regex>
#include <algorithm>
#include <utility>
#include <exception>
#include <iostream>

#include "UserDataFileComponent.h"
#include "tinyxml.h"
#include "Logger.h"

using namespace FileShare;

/**
    ifndef used for outlining
*/
#ifndef USER_DATA_PART_OF

const UserData UserData::BadData = UserData();

UserData::UserData(const String& als, const UserAddr& adr, const UserStatus& sts):
    alias(als),
    address(adr),
    status(sts)
{}

void UserData::Alias(const String& als){
    alias = als;
}
void UserData::Address(const UserAddr& addr){
    address = addr;
}
void UserData::Status(const UserStatus& sts){
    status = sts;
}
Bool UserData::IsBadAlias(const String &als){
    Bool soBad = FALSE;
    
    std::regex whiteSpace(R"(^(.*\s+.*)+$)");
    std::regex notWordOrNumber(R"(^(.*\W+.*)+$)");
    std::regex badPrefix(R"(^\b\d+.*)");
    
    ++soBad;
    if (als.empty())
        return soBad;

    ++soBad;
    if (std::regex_match(als, whiteSpace))
        return soBad;

    ++soBad;
    if (std::regex_match(als, notWordOrNumber))
        return soBad;

    ++soBad;
    if (std::regex_match(als, badPrefix))
        return soBad;

    return FALSE;
}  
bool UserData::operator==(const UserData & other) const{
    return{
        alias == other.alias &&
        address == other.address &&
        status == other.status
    };
}
bool UserData::operator!=(const UserData & other) const{
    return !(*this==other);
}

#ifndef USER_ADDRESS_PART_OF
String UserData::UserAddr::to_str() const
{
    return addr;
}

#undef USER_ADDR
#endif //USER_ADDRESS_PART_OF

#ifndef USER_STATUS_PART_OF

const String UserData::UserStatus::statusString[] = {
    "ugly", "bad", "good", "self"
};
UserData::UserStatus::UserStatus(StatusValue sts) :
    value(sts)
{}
String UserData::UserStatus::to_str() const{
    return statusString[static_cast<int>(value)];
}

#endif //USER_STATUS_PART_OF
#endif

#ifndef UDF_COMPONENT_PART_OF

const PCHAR udfXml = "UDF.xml";

namespace Udfn {
    const PCHAR root = "root";
    const PCHAR user = "user";
    const PCHAR alias = "alias";
    const PCHAR address = "address";
    const PCHAR status = "status";
};

String UdfPath();

static TiXmlDocument udf;
UDFComponent::UDFComponent(){
    {
        #ifdef LOGGER
        Log::TextInRed(UDF()->);
        __Begin;
        #endif // LOGGERsd
    }

    bool loadResult = udf.LoadFile(UdfPath().c_str(), TIXML_ENCODING_LEGACY);
    {
        #ifdef LOGGER
        IF(loadResult)
            Log::TextInRed(udf.LoadFile()+);
        ELSE
            Log::TextInRed(udf.LoadFile()-);
        #endif // LOGGER
    }
   
    if (udf.NoChildren()) {
        {
            #ifdef LOGGER
            Log::TextInRed(udf no root creating one);
            #endif
        }
        udf.LinkEndChild(new TiXmlElement(Udfn::root));
    }

    bool saveResult = udf.SaveFile();
    {
        #ifdef LOGGER
        IF(saveResult)
            Log::TextInRed(udf.SaveFile()+);
        ELSE
            Log::TextInRed(udf.SaveFile()-);
        #endif // LOGGER
    }

    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-UDF());
        #endif // LOGER
    }
}
UDFComponent::~UDFComponent() 
{
    //udf.SaveFile();
}

String UdfPath() {
    static String directoryPathShort;
    {
        {
            #ifdef LOGGER
            Log::TextInRed(UdfPath()->);
            __Begin;
            #endif // LOGGER
        }
        if (directoryPathShort.empty())
        {
            TCHAR tmodulePath[MAX_PATH]{};
            GetModuleFileName(NULL, tmodulePath, MAX_PATH);
            String directoryPath = std::regex_replace(tmodulePath, std::regex("([^\\\\]+[.]exe)"), "");
            TCHAR tdirectPathShort[MAX_PATH]{};
            GetShortPathName(directoryPath.c_str(), tdirectPathShort, MAX_PATH);
            directoryPathShort = String(tdirectPathShort);
        }

        {
            #ifdef LOGGER
            Log::InRed("UdfPath: " + directoryPathShort);
            #endif
        }

        {
            #ifdef LOGGER
            __End;
            Log::TextInRed(<-UdfPath());
            #endif // LOGGER
        }
    }
    return directoryPathShort + udfXml;
}

#ifndef UDF_XML_HELPER_FUNCTIONS
/**
xml element -> to user data
can be chaged later
*/
UserData      ConvertTiXmlElementToUserData(const TiXmlElement*);
/**
user data -> xml element
can be chaged later
*/
TiXmlElement* ConvertUserDataToTiXmlElement(const UserData&);
/**
look trough udf to find needed user
can be chaged later
*/
TiXmlElement* FindUserInTiXmlDocument(const TiXmlDocument&, const UserData&);
#endif //UDF_XML_HELPER_FUNCTIONS

UserVector UDFComponent::FindUsersInFile() {
    UserVector uv;

    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user))
        uv.push_back(ConvertTiXmlElementToUserData(iter));

    return uv;
}
UserVector UDFComponent::FindUsersInFile(const UserData::UserStatus& sts)
{
    UserVector uv;

    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user)) {
        auto ud = ConvertTiXmlElementToUserData(iter);
        if (ud.Status() == sts)
            uv.push_back(ud);
    }

    return uv;
}
UserData UDFComponent::FindUsersInFile(const String& als){
    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user)) {
        auto ud = ConvertTiXmlElementToUserData(iter);
        if (ud.Alias() == als)
            return ud;
    }
    return UserData::BadData;
}
UserData FileShare::UDFComponent::FindUsersInFile(const UserData::UserAddr& addr){
    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user)) {
        auto ud = ConvertTiXmlElementToUserData(iter);
        if (ud.Address() == addr)
            return ud;
    }
    return UserData::BadData;
}
UserData UDFInterface::FindUsersInFile(const UserData::UserAddr& adr)
{
    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user)) {
        auto ud = ConvertTiXmlElementToUserData(iter);
        if (ud.Address() == adr)
            return ud;
    }
    return UserData::BadData;
}


bool UDFComponent::AppendUser(const UserData& usr){
    //try to find if usr already exists in udf
    if (FindUserInTiXmlDocument(udf, usr))
        return false; // if exists AppendUser fail
    else {
        auto elem = ConvertUserDataToTiXmlElement(usr); // create new
        if (!elem)
            return false;
        else {
            auto root = udf.RootElement();
            root->LinkEndChild(elem);
            udf.SaveFile();
            return true;
        }
    }
}
bool UDFComponent::RemoveUser(const UserData& usr)
{
    auto elem = FindUserInTiXmlDocument(udf, usr);
    if (!elem)
        return false;

    udf.RootElement()->RemoveChild(elem);
    
    udf.SaveFile();
    return true;
}
bool UDFComponent::ModifyUser(const UserData & usrOld, const UserData & usrNew)
{
    auto elemOld = FindUserInTiXmlDocument(udf, usrOld);
    if (!elemOld)
        return false;
    
    auto elemNew = ConvertUserDataToTiXmlElement(usrNew);
    if (!elemNew)
        return false;
    
    auto root = udf.RootElement();
    if (!root->ReplaceChild(elemOld, *elemNew))
        return false;

    udf.SaveFile();
    return true;
}
bool UDFComponent::UserAlreadyExists(const UserData& usr){
    auto elem = FindUserInTiXmlDocument(udf, usr);  
    return elem != nullptr;
}

#ifndef UDF_XML_HELPER_FUNCTIONS
UserData ConvertTiXmlElementToUserData(const TiXmlElement* element) {
    auto alias = String(element->Attribute(Udfn::alias));
    auto address = UserData::UserAddr(element->Attribute(Udfn::address));  
    auto status = static_cast<UserData::UserStatus::StatusValue>(std::stoi(element->Attribute(Udfn::status)));

    return UserData(alias, address, status);
}
TiXmlElement* ConvertUserDataToTiXmlElement(const UserData& usr) {
    auto alias   = usr.Alias();
    auto address = usr.Address().to_str();
    auto status  = static_cast<int>(usr.Status().value);

    auto element = new TiXmlElement(Udfn::user);
    
    element->SetAttribute(Udfn::alias, alias.c_str());
    element->SetAttribute(Udfn::address, address.c_str());
    element->SetAttribute(Udfn::status, std::to_string(status).c_str());

    return element;
}
TiXmlElement* FindUserInTiXmlDocument(const TiXmlDocument& udf, const UserData& usr) {
    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user))
        if (ConvertTiXmlElementToUserData(iter).Alias() == usr.Alias())
            return const_cast<TiXmlElement*>(iter);
    return nullptr;
}
#endif UDF_XML_HELPER_FUNCTIONS
#endif //UDF_COMPONENT_PART_O

