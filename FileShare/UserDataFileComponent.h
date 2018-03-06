#pragma once
#include <vector>
#include <string>

namespace FileShare {
    using String = std::string;
    using UserVector = std::vector<struct UserData>;

    struct UserData{
        struct UserAddr {

        };
        enum class UserStatus{ 
            Self, Good, Bad, Ugly
        };

        String      alias;
        UserAddr    address;/*??*/
        UserStatus  status;

        UserData(const String& alias, const UserAddr& address, UserStatus status);

        operator String() const;

        static String to_str(UserStatus);
        static String to_str(UserAddr);
    };
    
    class UDFInterface {
    public:
        virtual UserVector  FindUsersInFile()                       = 0; // find all (self excluded)        
        virtual UserVector  FindUsersInFile(UserData::UserStatus)   = 0;        
        virtual UserData    FindUsersInFile(const String&)          = 0; 
        virtual UserData    FindUsersInFile(UserData::UserAddr)     = 0;

        virtual bool AppendUser(const UserData&) = 0;  // first user to append is gonna be SELF
        virtual bool RemoveUser(const UserData&) = 0;  // remove(findinfile(name))
        virtual bool ModifyUser(const UserData&, const UserData&) = 0; 
    protected:
        virtual void UserDataFileSetup() = 0; // find if exists, create if doesnt
    };

    class UDFComponent:
        public UDFInterface
    {
    public:
        UDFComponent();
        //virtual UserVector  FindUsersInFile()                       override;      
        //virtual UserVector  FindUsersInFile(UserData::UserStatus)   override;
        //virtual UserData    FindUsersInFile(const String&)          override;
        //virtual UserData    FindUsersInFile(UserData::UserAddr)     override;
        //
        //virtual bool AppendUser(const UserData&)    override;
        //virtual bool RemoveUser(const UserData&)    override;  
        //virtual bool ModifyUser(const UserData& oldUD, const UserData& newUD) override;
    protected:
        virtual void UserDataFileSetup() override;
    };
}