#include <sys/stat.h>
#include <string>
#include <fstream>
#include <cctype>
#include "core/tingx_parser.h"

namespace tingx {

// ------------------------- Array--------------------------

Array::Array() {
    type_ = Type::Array;
}

void Array::Add(ParserObject *val) {
    obj_.push_back(val);
}

ArrayObj& Array::Get() {
    return obj_;
}

int Array::Parse(const char *pos, int len) {

    return 0;
}



// ------------------------- Dict --------------------------
Dict::Dict() {
    type_ = Type::Dict;
}

void Dict::Add(const std::string& key, ParserObject* val) {
    obj_.insert(std::make_pair(key, val));
}

DictObj& Dict::Get() {
    return obj_;
}


int Dict::Parse(const char *pos, int len) {

    return 0;
}


// ------------------------- String --------------------------
String::String() {
    type_ = Type::String;
}

StringObj& String::Get() {
    return obj_;
}

int String::Parse(const char *pos, int len) {

    return 0;
}


// ------------------------- Integer --------------------------
Integer::Integer() {
    type_ = Type::Integer;
}

IntegerObj Integer::Get() {
    return obj_;
}

int Integer::Parse(const char *pos, int len) {

    return 0;
}


// ------------------------- ConfigFileParser --------------------------
ConfigFileParser::ConfigFileParser(std::string& filename) : filename_(filename) {
    struct stat buf;
    if (stat(filename_.c_str(), &buf) != 0) {
        perror(filename_.c_str());
        return;
    }

    is_open_ = false;
    pos_ = 0;
    std::ifstream infile(filename_);

    if (!infile.is_open()) {
        return;
    }

    char c;
    infile.get(c);
    while (!infile.eof()) {
        buffer.push_back(c);
        infile.get(c);
    }
    infile.close();
    is_open_ = true;

    root_.Attach(new Dict());
}

bool ConfigFileParser::GetIsOpen() {
    return is_open_;
}

int ConfigFileParser::Parse() {
    return Parse(START, root_);
}

int ConfigFileParser::Parse(Status status_, ParserObject* root) {
    ArrayObj objs;

    Status status = status_;
    Ptr<ParserObject> key, val;
    int st, ed;
    while(pos_ < buffer.length()) {
        if (buffer[pos_] == '#') {
            Parse(COMMENT, root);
            ++pos_;
        }

        switch (status) {
        case START:
            if (isgraph(buffer[pos_])) {
                st = pos_;
                if (key == nullptr)
                    status = KEY;
                else 
                    status = VALUE;
            }
            break;

        case KEY:
            if (!isgraph(buffer[pos_])) {
                ed = pos_;
                String *p = new String();
                p->Parse(&buffer[st], ed - st);
                key.Attach(p);
                status = START;
            }
            break;

        case VALUE:
            if (!isgraph(buffer[pos_])) {
                ed = pos_;
                String *p = new String();
                p->Parse(&buffer[st], ed - st);
                if (val == nullptr)
                    val.Attach(p);
                status = START;
            }
        
        case COMMENT:
            if (buffer[pos_] == '\n')
                return 0;
            break;
        default:
            break;
        }

        ++pos_;
    }



}

ParserObject* ConfigFileParser::Get() {
    return root_;
}

}
