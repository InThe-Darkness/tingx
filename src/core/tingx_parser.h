#ifndef SRC_CORE_TINGX_PARSER_H_
#define SRC_CORE_TINGX_PARSER_H_


#include <string>
#include <vector>
#include <map>
#include "tingx_refcount.hpp"

namespace tingx {


class ParserObject : public RefCounted {
public:
    enum Type {ARRAY, BLOCK, STRING, KVITEM};
    ParserObject() : parent_(nullptr) {};
    virtual ~ParserObject() {}
    virtual void Add(ParserObject* ) = 0;
    Type GetType() { return type_; }

    void SetParent(ParserObject* parent) { parent_ = parent; }
    ParserObject* GetParent() { return parent_; }

    void SetCatched(bool val) { catched_ = val; }
    bool GetCatched() { return catched_; }

    static void SerializeOut(ParserObject *root, std::string &buffer, int indent = 0);
    static ParserObject* GetRoot(ParserObject* obj);
    template<typename T>
    static T* BuildInstance(ParserObject *parent) {
        T* obj = new T();
        static_cast<ParserObject*>(obj)->SetParent(parent);
        return obj;
    }
protected:
    ParserObject* parent_;
    Type type_;
    bool catched_ = false;
};


using ArrayObj = std::vector<Ptr<ParserObject>>;
using BlockObj = ArrayObj;//std::map<std::string, Ptr<ParserObject>>;
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


class Block : public ParserObject {
public:
    Block();
    void Add(ParserObject *val);
    BlockObj& Get();
    virtual int Parse(const char *pos, int len);

private:
    BlockObj obj_;
};

// Key-Value Item
class KVItem : public ParserObject {
public:
    KVItem();
    KVItem(ParserObject* key, ParserObject *val);
    void Add(ParserObject* val);

    Ptr<ParserObject> first;
    Ptr<ParserObject> second;

};

class String : public ParserObject {
public:
    String();

    // this function do nothing
    void Add(ParserObject*);
    StringObj& Get();
    virtual int Parse(const char *pos, int len);
private:
    StringObj obj_;
};



class Parser : public RefCounted {
public:
    Parser() {}
    virtual ~Parser() {}

    virtual int Parse() = 0;

};

class ConfigFileParser : public Parser {
public:
    enum Status {START = 0, KEY, VALUE, COMMENT, BLOCK_START, BLOCKING, BLOCK_DONE};

    //ConfigFileParser(const char *filename);
    ConfigFileParser(std::string &&filename);
    ConfigFileParser(const std::string &filename);
    bool GetIsOpen();
    virtual int Parse();
    int Parse(Status status, ParserObject* root);
    ParserObject* Get();

private:
    void Init();

    std::string filename_;
    bool is_open_;
    int pos_;
    std::string buffer_;
    Ptr<ParserObject> root_;
};


} // namespace tingx


#endif
