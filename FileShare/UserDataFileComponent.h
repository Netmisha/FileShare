#pragma once
#include <vector>
#include <string>

namespace FileShare {
    using String = std::string;
    using UserVector = std::vector<class UserData>;
    using Bool = BOOL;

    /**
        UserData started of as a simple construct
        containing some data elements about USER
        but then i decided to make it a class
        with data validation setters
    */
    class UserData
    {
    public:
        class UserAddr;
        class UserStatus;
        class UserStatusConsts;
        class TiXmlDocument;

        UserData() = default; // bad data
        UserData(const String& alias, const UserAddr& address, const UserStatus& status);

        const String&       Alias()     const { return alias; }
        const UserAddr&     Address()   const { return address; }
        const UserStatus&   Status()    const { return status; }

        /**
        ----User.Alias should
        --------not contain whitespaces
        --------not contain odd symbols or operator-characters
        --------begin with word-character
        ----call of IsBadAlias is better be left to Controller
        */
        void Alias(const String& alias);
        void Address(const UserAddr& address);
        void Status(const UserStatus& status);

        /**
        ----IsBadAlias returns int to specify the degree of unfitness
        */
        static Bool IsBadAlias(const String& alias);

        bool operator==(const UserData& other) const;
        bool operator!=(const UserData& other) const;

        class UserAddr {
        public:
            UserAddr() = default;
            /*
                sockaddr_in?
                ip, port?
            */
            UserAddr(const String& adr): addr(adr) {}
            
            /**
            yet to make it right
            */
            String to_str() const;

            bool operator==(const UserAddr& other) const { return addr == other.addr; }
            bool operator!=(const UserAddr& other) const { return !(*this == other); }

            String addr;
        };

        class UserStatus{
        public:
            enum class StatusValue: int{
                Ugly, 
                Bad,
                Good,
                Self
            };
            
            UserStatus() = default;
            UserStatus(StatusValue);

            bool operator== (const UserStatus& other) const { return value == other.value; }
            bool operator!= (const UserStatus& other) const { return value != other.value; }

            String to_str() const;

            static const String statusString[];

            StatusValue value;
        };

        const static UserData BadData;
    private:
        String      alias;
        UserAddr    address;
        UserStatus  status;

        friend class UDFComponent;
    };
    
    class UDFInterface {
    public:
        virtual UserVector  FindUsersInFile()                               = 0; // find all (self excluded)        
        virtual UserVector  FindUsersInFile(const UserData::UserStatus&)    = 0;        
        virtual UserData    FindUsersInFile(const String&)                  = 0; 
        virtual UserData    FindUsersInFile(const UserData::UserAddr&)      = 0;

        virtual bool AppendUser(const UserData&) = 0;  // first user to append is gonna be SELF
        virtual bool RemoveUser(const UserData&) = 0;  // remove(findinfile(name))
        virtual bool ModifyUser(const UserData&, const UserData&) = 0; 
    protected:
        virtual bool UserAlreadyExists(const UserData&) = 0;
    };

    class UDFComponent:
        public UDFInterface
    {
    public:
        UDFComponent();
        ~UDFComponent();

        virtual UserVector  FindUsersInFile()                                 override;      
        virtual UserVector  FindUsersInFile(const UserData::UserStatus& sts)  override;
        virtual UserData    FindUsersInFile(const String& alias)              override;
        virtual UserData    FindUsersInFile(const UserData::UserAddr& addr)   override;
        //
        virtual bool AppendUser(const UserData&)    override;
        virtual bool RemoveUser(const UserData&)    override;  
        virtual bool ModifyUser(const UserData& oldUD, const UserData& newUD) override;
    protected:
        virtual bool UserAlreadyExists(const UserData&) override;
    };
}