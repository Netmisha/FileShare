#include <Windows.h>
#include <regex>
#include <utility>
#include <exception>

#include "UserDataFileComponent.h"
#include "tinyxml.h"
#define TIXML_USE_STL

using namespace FileShare;

/**
    ifndef used for outlining
*/
#ifndef USER_DATA_PART_OF
#define USER_DATA UserData::

const UserData USER_DATA BadData = UserData();

USER_DATA UserData(const String& als, const UserAddr& adr, const UserStatus& sts):
    alias(als),
    address(adr),
    status(sts)
{}

void USER_DATA Alias(const String& als){
    alias = als;
}
void USER_DATA Address(const UserAddr& addr){
    address = addr;
}
void USER_DATA Status(const UserStatus& sts){
    status = sts;
}
Bool USER_DATA IsBadAlias(const String &als){
    Bool soBad = FALSE;
    
    std::regex whiteSpace(R"(\s)");
    std::regex notWordOrNumber(R"(\W)");
    std::regex badPrefix(R"(^\b\d+.*)");//"^\\b\\d+.*");
    
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
bool USER_DATA operator==(const UserData & other) const{
    return{
        alias == other.alias &&
        address == other.address &&
        status == other.status
    };
}
bool USER_DATA operator!=(const UserData & other) const{
    return !(*this==other);
}

#ifndef USER_ADDRESS_PART_OF
#define USER_ADDR UserData::UserAddr:: 
String USER_ADDR to_str() const
{
    return "default address";
}

#undef USER_ADDR
#endif //USER_ADDRESS_PART_OF

#ifndef USER_STATUS_PART_OF
#define USER_STATUS UserData::UserStatus::
#define USER_STATUS_STRING UserData::UserStatus::StatusString::
#define USER_STATUS_VALUE UserData::UserStatus::StatusValue::

const String USER_STATUS_STRING Ugly = "ugly";
const String USER_STATUS_STRING Bad  = "bad" ;
const String USER_STATUS_STRING Good = "good";
const String USER_STATUS_STRING Self = "self";

USER_STATUS UserStatus(StatusValue sts) :
    value(sts)
{}
USER_STATUS UserStatus(const String & sts) :
    value(StatusValue::Ugly)
{
    if (sts == USER_STATUS_STRING Self)  value = USER_STATUS_VALUE Self;
    if (sts == USER_STATUS_STRING Good)  value = USER_STATUS_VALUE Good;
    if (sts == USER_STATUS_STRING Bad)   value = USER_STATUS_VALUE Bad;
}

String USER_STATUS to_str() const
{
    if (value == USER_STATUS_VALUE Self) return USER_STATUS_STRING Self;
    if (value == USER_STATUS_VALUE Good) return USER_STATUS_STRING Good;
    if (value == USER_STATUS_VALUE Bad ) return USER_STATUS_STRING Bad;
    /*(str == UserStatus::Ugly)*/        return USER_STATUS_STRING Ugly;
}

#undef USER_STATUS
#undef USER_STATUS_STRING
#undef USER_STATUS_VALUE
#endif //USER_STATUS_PART_OF
#undef USER_DATA
#endif

#ifndef UDF_COMPONENT_PART_OF

const PCHAR udfXml = "UDF.xml";

namespace Udfn {
    const PCHAR user = "user";
    const PCHAR alias = "alias";
    const PCHAR address = "address";
    const PCHAR status = "status";
};

static TiXmlDocument udf(udfXml);
UDFComponent::UDFComponent()
{
    UserDataFileSetup();
    udf.LoadFile();
}
UDFComponent::~UDFComponent() 
{
    udf.SaveFile();
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

#ifndef UDF_XML_HELPER_FUNCTIONS
UserData      ConvertTiXmlElementToUserData(const TiXmlElement*);
TiXmlElement* ConvertUserDataToTiXmlElement(const UserData&);
TiXmlElement* FindUserInTiXmlUserDataFile(const TiXmlDocument&, const UserData&);
#endif //UDF_XML_HELPER_FUNCTIONS

UserVector UDFComponent::FindUsersInFile() {
    UserVector uv;

    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user))
        uv.push_back(ConvertTiXmlElementToUserData(iter));

    return UserVector();
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
UserData UDFInterface::FindUsersInFile(const UserData::UserAddr& adr)
{
    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user)) {
        auto ud = ConvertTiXmlElementToUserData(iter);
        if (ud.Address() == adr)
            return ud;
    }
    return UserData();

    return UserData::BadData;
}


bool UDFComponent::AppendUser(const UserData& usr){
    //try to find if usr already exists in udf
    if (FindUserInTiXmlUserDataFile(udf, usr))
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
    auto elem = FindUserInTiXmlUserDataFile(udf, usr);
    if (!elem)
        return false;

    udf.RootElement()->RemoveChild(elem);
    
    udf.SaveFile();
    return true;
}
bool UDFComponent::ModifyUser(const UserData & usrOld, const UserData & usrNew)
{
    auto elemOld = FindUserInTiXmlUserDataFile(udf, usrOld);
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
    auto elem = FindUserInTiXmlUserDataFile(udf, usr);  
    return elem != nullptr;
}

#ifndef UDF_XML_HELPER_FUNCTIONS
/**
xml element -> to user data
can be chaged later
*/
UserData ConvertTiXmlElementToUserData(const TiXmlElement* element) {
    auto alias = String(element->Attribute(Udfn::alias));
    auto address = UserData::UserAddr(element->Attribute(Udfn::address));
    auto status = UserData::UserStatus((element->Attribute(Udfn::status)));

    return UserData(alias, address, status);
}

/**
user data -> xml element
can be chaged later
*/
TiXmlElement* ConvertUserDataToTiXmlElement(const UserData& usr) {
    auto alias   = usr.Alias().c_str();
    auto address = usr.Address().to_str().c_str();
    auto status  = usr.Status().to_str().c_str();

    auto element = new TiXmlElement(Udfn::user);
    
    element->SetAttribute(Udfn::alias, alias);
    element->SetAttribute(Udfn::address, address);
    element->SetAttribute(Udfn::status, status);

    return element;
}

/**
look trough udf to find needed user
can be chaged later
*/
TiXmlElement* FindUserInTiXmlUserDataFile(const TiXmlDocument& udf, const UserData& usr) {
    auto root = udf.RootElement();
    for (auto iter = root->FirstChildElement(Udfn::user); iter; iter = iter->NextSiblingElement(Udfn::user))
        if (ConvertTiXmlElementToUserData(iter) == usr)
            return const_cast<TiXmlElement*>(iter);
    return nullptr;
}
#endif UDF_XML_HELPER_FUNCTIONS
#endif //UDF_COMPONENT_PART_O