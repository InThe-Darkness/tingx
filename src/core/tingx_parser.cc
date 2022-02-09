#include <sys/stat.h>
#include <string>
#include <fstream>
#include <cctype>
#include "core/tingx_parser.h"

namespace tingx {

void ParserObject::SerializeOut(ParserObject *root, std::string &buffer, int indent) {
    if (root->GetType() == Type::STRING) {
        if (indent) {
            buffer.append(std::string(indent, ' '));
        }
        buffer.append(static_cast<String*>(root)->Get());
        buffer.push_back(';');
        buffer.push_back('\n');
        return;
    } else if (root->GetType() == Type::KVITEM) {
        buffer.append(std::string(indent, ' '));
        KVItem *p = static_cast<KVItem*>(root);
        buffer.append(static_cast<String*>(p->first.Get())->Get());
        buffer.push_back(' ');

        if (p->second->GetType() == Type::ARRAY) {
            ArrayObj &obj = static_cast<Array*>(p->second.Get())->Get();
            for (auto iter : obj) {
                if (iter->GetType() == Type::BLOCK) {
                    buffer.push_back('{');
                    buffer.push_back('\n');
                    SerializeOut(iter, buffer, indent + 4);
                } else {
                    buffer.append(static_cast<String*>(iter.Get())->Get());
                    buffer.push_back(' ');
                }
            }
            buffer.push_back(';');
            buffer.push_back('\n');
            if (obj.back()->GetType() == Type::BLOCK) {
                buffer.append(std::string(indent, ' '));
                buffer.push_back('}');
                buffer.push_back('\n');
            }
        } else if (p->second->GetType() == Type::BLOCK) {
            buffer.push_back('{');
            buffer.push_back('\n');
            SerializeOut(p->second, buffer, indent + 4);
            buffer.append(std::string(indent, ' '));
            buffer.push_back('}');
            buffer.push_back('\n');
        } else {
            buffer.append(static_cast<String*>(p->second.Get())->Get());
            buffer.push_back(';');
            buffer.push_back('\n');
        }
    } else { // BLOCK
        Block* pBlock = static_cast<Block*>(root);
        BlockObj& obj = pBlock->Get();
        for (auto iter : obj) {
            SerializeOut(iter, buffer, indent);
        }
    }
}

ParserObject* ParserObject::GetRoot(ParserObject *obj) {
    ParserObject* root = obj->parent_;
    while (root && root->parent_ != nullptr) {
        obj = root;
        root = root->parent_;
    }
    return obj;
}


// ------------------------- Array--------------------------

Array::Array() {
    type_ = Type::ARRAY;
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



// ------------------------- Block --------------------------
Block::Block() {
    type_ = Type::BLOCK;
}

void Block::Add(ParserObject *val) {
    obj_.push_back(val);
}

BlockObj& Block::Get() {
    return obj_;
}


int Block::Parse(const char *pos, int len) {

    return 0;
}


// ------------------------- KVItem --------------------------
KVItem::KVItem() {
    type_ = Type::KVITEM;
}

KVItem::KVItem(ParserObject* key, ParserObject *value) {
    type_ = Type::KVITEM;
    first = key;
    second = value;
};

void KVItem::Add(ParserObject* val) {
    if (first == nullptr) {
        first = val;

    } else {
        second = val;
    }
}


// ------------------------- String --------------------------

String::String() {
    type_ = Type::STRING;
}

void String::Add(ParserObject*) { }

StringObj& String::Get() {
    return obj_;
}

int String::Parse(const char *pos, int len) {
    obj_ = std::string(pos, len);
    return 0;
}



// ------------------------- ConfigFileParser --------------------------
// ConfigFileParser::ConfigFileParser(const char *filename) {
//     ConfigFileParser(std::string(filename));
// }

ConfigFileParser::ConfigFileParser(std::string &&filename) : filename_(filename) {
    Init();
}

ConfigFileParser::ConfigFileParser(const std::string& filename) : filename_(filename) {
    Init();
}

void ConfigFileParser::Init() {
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
        buffer_.push_back(c);
        infile.get(c);
    }
    infile.close();
    is_open_ = true;

    root_.Attach(new Block());
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
    while (pos_ < buffer_.length()) {
        char c = buffer_[pos_];
        if (c == '#') {
            Parse(COMMENT, root);
            ++pos_;
        }

        switch (status) {
        case START:
            if (isgraph(c) && c != ';') {
                st = pos_;
                if (key == nullptr) {
                    status = KEY;
                }
                else {
                    status = VALUE;
                    if (c == '{') {
                        ++pos_;
                        if (val == nullptr) {
                            val.Attach(ParserObject::BuildInstance<Block>(key));
                            Parse(START, val);

                        } else if (val->GetType() == ParserObject::Type::ARRAY) {
                            Ptr<ParserObject> p(ParserObject::BuildInstance<Block>(val));
                            static_cast<Array*>(val.Get())->Add(p);
                            Parse(START, p);

                        } else {
                            Ptr<ParserObject> pArray(ParserObject::BuildInstance<Array>(key));
                            Ptr<ParserObject> pBlock(ParserObject::BuildInstance<Block>(pArray));
                            static_cast<Array*>(pArray.Get())->Add(val);
                            val->SetParent(pArray);
                            static_cast<Array*>(pArray.Get())->Add(pBlock);
                            val.Attach(pArray.Detach());
                            Parse(START, pBlock);

                        }
                        KVItem* pKVItem = ParserObject::BuildInstance<KVItem>(root);
                        pKVItem->Add(key.Detach());
                        pKVItem->Add(val.Detach());
                        root->Add(pKVItem);
                        status = START;
                    }
                }
            }
            break;

        case KEY:
            if (!isgraph(c) || c == ';' ) {
                ed = pos_;
                String *p = ParserObject::BuildInstance<String>(root);
                p->Parse(&buffer_[st], ed - st);
                key.Attach(p);

                if (c == ';') {
                    root->Add(key.Detach());
                }
                status = START;
            }
            break;

        case VALUE:
            if (!isgraph(c) || c == ';') {
                ed = pos_;
                String *p = ParserObject::BuildInstance<String>(key);
                p->Parse(&buffer_[st], ed - st);
                if (val == nullptr) {
                    val.Attach(p);

                } else if (val->GetType() == ParserObject::Type::ARRAY || 
                           val->GetType() == ParserObject::Type::BLOCK) {
                    val->Add(p);

                } else {
                    Array* pArray = ParserObject::BuildInstance<Array>(key);
                    val->SetParent(pArray);
                    pArray->Add(val.Detach());
                    pArray->Add(p);
                    val.Attach(pArray);
                }

                if (c == ';') {
                    KVItem* pKVItem = ParserObject::BuildInstance<KVItem>(root);
                    key->SetParent(pKVItem);
                    pKVItem->Add(key.Detach());
                    pKVItem->Add(val.Detach());
                    root->Add(pKVItem);
                    status = START;
                }

            }
        
        case COMMENT:
            if (c == '\n')
                return 0;
            break;
        default:
            break;
        }

        if (c == '}')
            return 0;
        ++pos_;
    }


    return 0;
}

ParserObject* ConfigFileParser::Get() {
    return root_;
}

}
