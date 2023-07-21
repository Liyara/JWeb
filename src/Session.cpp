#include "jws/Session.h"

namespace jws {

    Session::Session(const jutil::String &context_, const jutil::String &id_) : Object(), context(context_) , id(id_) {
        jutil::File file(context + id, jutil::READ | jutil::CREATE);
        jutil::String contents;
        file.dump(&contents);
        if (contents.size() < 2) {
            contents = "{}";
        }
        C_STR(contents_cstr, contents);
        this->file = json::parse(std::string(contents_cstr));
    }

    void Session::writeFile() {
        jutil::File file(context + id, jutil::WRITE | jutil::INSERT | jutil::ERASE);
        std::string _contents = this->file.dump(4);
        jutil::String contents(_contents.c_str());
        file << contents;
    }

    const jutil::String &Session::getId() const {
        return this->id;
    }
}