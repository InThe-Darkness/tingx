#ifndef SRC_CORE_TINGX_PARSER_H_
#define SRC_CORE_TINGX_PARSER_H_


#include <string>
#include <vector>
#include <map>
#include "tingx_refcount.hpp"

namespace tingx {



class Parser : public RefCounted {
public:
    Parser() {}
    virtual ~Parser() {}

    virtual int Parse() = 0;

};


class ParserObject : public RefCounted {
public:
    enum Type {Array, Dict, String, Integer};
    ParserObject() {};
    virtual ~ParserObject() {}

    virtual int Parse(const char *pos, int len) = 0;
    Type GetType() { return type_; }

protected:
    Type type_;
    bool catched_ = false;
};


using ArrayObj = std::vector<Ptr<ParserObject>>;
using IntegerObj = long long;
using DictObj = std::map<std::string, Ptr<ParserObject>>;
using StringObj = std::string;


class Array : public ParserObject {
public:
    Array ();
    void Add(ParserObject *val);
    ArrayObj& Get();
    virtual int Parse(const char *pos, int len);

private:
    ArrayObj obj_;
};


class Dict : public ParserObject {
public:
    Dict();
    void Add(const std::string& key, ParserObject* val);
    DictObj& Get();
    virtual int Parse(const char *pos, int len);

private:
    DictObj obj_;
};

class String : public ParserObject {
public:
    String();
    StringObj& Get();
    virtual int Parse(const char *pos, int len);
private:
    StringObj obj_;
};

class Integer : public ParserObject {
public:
    Integer();
    IntegerObj Get();
    virtual int Parse(const char *pos, int len);
private:
    IntegerObj obj_;
};

class ConfigFileParser : public Parser {
public:
    enum Status {START = 0, KEY, VALUE, COMMENT};

    ConfigFileParser(std::string &filename);
    bool GetIsOpen();
    virtual int Parse();
    virtual int Parse(Status status, ParserObject* root);
    ParserObject* Get();

private:
    

    std::string filename_;
    bool is_open_;
    int pos_;
    std::string buffer;
    Ptr<ParserObject> root_;
};


} // namespace tingx


#endif
