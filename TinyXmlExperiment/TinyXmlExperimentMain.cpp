#include <iostream>
#include <string>

#include"tinyxml.h"
#include "tinystr.h"

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
int main() {
    hello_world_11();
    return 0;
}