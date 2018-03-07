#pragma once
#include <vector>
#include <string>

namespace FileShare {
    using String = std::string;
    using UserVector = std::vector<struct UserData>;
    #define TIXML_USE_STL
    struct UserData{
        struct UserAddr {
            UserAddr(const String&) {}
            bool operator==(const UserAddr&) const { return false; }
        };
        enum class UserStatus{
            Ugly, 
            Bad,
            Good,
            Self          
        };
        static class UserStatusConsts {
        public:
            const String Ugly = "ugly";
            const String Bad  = "bad ";
            const String Good = "good";
            const String Self = "self";
        };

        String      alias;
        UserAddr    address;/*??*/
        UserStatus  status;

        UserData(const String& alias, const UserAddr& address, UserStatus status);

        operator String() const;
        bool operator==(const UserData&) const;

        static String to_str(UserStatus);
        static String to_str(UserAddr);
        static UserStatus str_to_status(const String&);
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
        virtual bool UserAlreadyExists(const UserData&) = 0;
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
        virtual bool AppendUser(const UserData&)    override;
        //virtual bool RemoveUser(const UserData&)    override;  
        //virtual bool ModifyUser(const UserData& oldUD, const UserData& newUD) override;
    protected:
        virtual void UserDataFileSetup() override;
        virtual bool UserAlreadyExists(const UserData&) override;

    };
}