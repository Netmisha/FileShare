#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <algorithm>
#include <regex>

#include"tinyxml.h"

using SPair = std::pair<std::string, std::string>;
using SMap = std::map<std::string, std::string>;
using VVector = std::vector<SMap>;

void hello_world_10() {
    TiXmlDocument doc;
    
    doc.LinkEndChild(new TiXmlDeclaration("1.0", "", ""));

    TiXmlElement * element = new TiXmlElement("Hello");
    TiXmlText * text = new TiXmlText("World");   
    element->LinkEndChild(text);    
    doc.LinkEndChild(element);

    TiXmlElement* elem;
    TiXmlText* txt;

    elem = new TiXmlElement("Goodbye");
    doc.LinkEndChild(elem);
    TiXmlElement * elem2 = elem;
    for (auto s : { "C","R","U","E","L" }) {
        TiXmlElement * newElem = new TiXmlElement(s);
        elem2->LinkEndChild(newElem);
        elem2 = newElem;
    }
    elem2->LinkEndChild(new TiXmlText("~world~"));
    elem2->LinkEndChild(new TiXmlElement("KEK"));

    doc.SaveFile("HelloWorld.xml");
}
void hello_world_11() {
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);

    TiXmlElement * element = new TiXmlElement("Hello");
    doc.LinkEndChild(element);

    TiXmlText * text = new TiXmlText("World");
    element->LinkEndChild(text);

    doc.SaveFile("HelloWorld(1).xml");
}

void TryMakingUsers1(const VVector& vv) {
    TiXmlDocument doc;

    doc.LinkEndChild(new TiXmlComment("UserData as attributes"));

    TiXmlElement* root = new TiXmlElement("USERS");
    doc.LinkEndChild(root);

    for (auto& sm : vv) {
        TiXmlElement* elem = new TiXmlElement("USER");
        for (auto& sp : sm) {
            elem->SetAttribute(sp.first, sp.second);
        }
        root->LinkEndChild(elem);
    }
    doc.SaveFile("users1.xml");
}
void TryMakingUsers2(const VVector& vv) {
    TiXmlDocument doc;

    doc.LinkEndChild(new TiXmlComment("UserData as nodes"));

    auto root = new TiXmlElement("USERS");
    doc.LinkEndChild(root);
    for (auto& sm : vv) {
        auto elem = new TiXmlElement("USER");
        for (auto& sp : sm) {
            auto node = new TiXmlElement(sp.first);
            elem->LinkEndChild(node);

            auto text = new TiXmlText(sp.second);
            node->LinkEndChild(text);
        }
        root->LinkEndChild(elem);
    }
    doc.SaveFile("users2.xml");
}

void TryInsertingUser1(const SMap& sm) {
    TiXmlDocument doc("users1.xml");
    
    if (!doc.LoadFile())
        std::cerr << "couldnt load file1" << std::endl;

    auto elem = new TiXmlElement("USER");
    for (auto&sp : sm)
        elem->SetAttribute(sp.first, sp.second);

    doc.RootElement()->LinkEndChild(elem);
    doc.SaveFile();
}
void TryInsertingUser2(const SMap& sm) {
    TiXmlDocument doc("users2.xml");

    if(!doc.LoadFile())
        std::cerr << "couldnt load file2" << std::endl;

    auto elem = new TiXmlElement("USER");
    for (auto&sp : sm) {
        auto node = new TiXmlElement(sp.first);
        auto text = new TiXmlText(sp.second);
        node->LinkEndChild(text);
        elem->LinkEndChild(node);
    }

    doc.RootElement()->LinkEndChild(elem);
    doc.SaveFile();
}

/**
    modifies all users the condition comes true for
*/
void TryModifyingUsers1(const SMap& smCondition, const SMap smCorrection) {
    TiXmlDocument doc("users1.xml");
    doc.LoadFile();

    auto root = doc.RootElement();
    
    for (auto iter = root->FirstChildElement("USER"); iter; iter = iter->NextSiblingElement("USER")) 
    {
        static auto allOfCondition = [&](const SPair& sp)
        {
            auto attr = iter->Attribute(sp.first);
            auto condition = std::regex(sp.second);
            return std::regex_match(*attr, condition);
        };

        if (std::all_of(smCondition.begin(), smCondition.end(), allOfCondition)) 
            for(auto&sp: smCorrection)
                iter->SetAttribute(sp.first, sp.second);
    }
    doc.SaveFile();
}
/**
modifies all users the condition comes true for
*/
void TryModifyingUsers2(const SMap& smCondition, const SMap smCorrection) {
    TiXmlDocument doc("users2.xml");
    doc.LoadFile();

    auto root = doc.RootElement();
    
    for (auto iter = root->FirstChildElement("USER"); iter; iter = iter->NextSiblingElement("USER")) 
    {
        static auto allOfCondition = [&](const SPair& sp) {
            auto elem = iter->FirstChildElement(sp.first);
            auto text = elem->GetText();
            auto condition = std::regex(sp.second);
            return std::regex_match(text, condition);
        };

        if (std::all_of(smCondition.begin(), smCondition.end(), allOfCondition)) 
            for (auto& sp : smCorrection) 
            {
                auto newText = new TiXmlText(sp.second);
                auto newElement = new TiXmlElement(sp.first);
                newElement->LinkEndChild(newText);

                iter->ReplaceChild(iter->FirstChild(sp.first), *newElement);
            }
    }
    doc.SaveFile();
}

int main() {
    //hello_world_10();
    //hello_world_11();

    auto vv = VVector{
        {{ "NAME","Vasyl"  }, { "IP","1.1.1.1" }, { "STATUS","self" }},
        {{ "NAME","Petro"  }, { "IP","1.1.1.2" }, { "STATUS","good" }},
        {{ "NAME","Ivan"   }, { "IP","1.1.1.3" }, { "STATUS","bad"  }},
        {{ "NAME","Stepan" }, { "IP","1.1.1.4" }, { "STATUS","ugly" }}
    };

    TryMakingUsers1(vv);
    TryMakingUsers2(vv);
    
    SMap sm{ { "NAME", "Ochrin" },{ "IP", "1.1.1.5" },{ "STATUS", "vicious" } };

    TryInsertingUser1(sm);
    TryInsertingUser2(sm);

    //TryModifyingUsers1({ {"IP", "1\\.1\\.1\\.\\d"} }, { {"NAME", "MaLittleBitch"} });
    //TryModifyingUsers2({ {"IP", "1\\.1\\.1\\.\\d"} }, { {"NAME", "MaLittleBitch"} });



    return 0;
}