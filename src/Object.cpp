#include "jws/Object.h"

namespace jws {

    namespace __Object__helper {
        template<>
        jutil::String get<jutil::String>(const jutil::String &keyName, const json &file, Object *__p) {
            C_STR(keyName_cstr, keyName);
            std::string rawValue = file[keyName_cstr];
            jutil::String value = __p->translateString(rawValue);
            return value;
        }

        template<>
        jutil::String get<jutil::String>(size_t index, const json &file, Object *__p) {
            std::string rawValue = file[index];
            jutil::String value = __p->translateString(rawValue);
            return value;
        }

        template<>
        Object get<Object>(const jutil::String &keyName, const json &file, Object *__p) {
            C_STR(keyName_cstr, keyName);
            json rawValue = file[keyName_cstr];
            return Object(rawValue);
        }

        template<>
        Object get<Object>(size_t index, const json &file, Object *__p) {
            json rawValue = file[index];
            return Object(rawValue);
        }

        template<>
        void set(const jutil::String &keyName, const jutil::String &value, json &file) {
            C_STR(keyName_cstr, keyName);
            C_STR(value_cstr, value);
            file[keyName_cstr] = std::string(value_cstr);
        }

        template<>
        void set(const jutil::String &keyName, const Object &value, json &file) {
            C_STR(keyName_cstr, keyName);
            for (auto &it: value.file.items()) {
                file[keyName_cstr][it.key()] = it.value();
            }
        }

        template<>
        void set(size_t index, const jutil::String &value, json &file) {
            C_STR(value_cstr, value);
            file[index] = std::string(value_cstr);
        }

        template<>
        void set(size_t index, const Object &value, json &file) {
            for (auto &it: value.file.items()) {
                file[index][it.key()] = it.value();
            }
        }
    }

    Object::Object() {
        this->valid = true;
    }

    Object::Object(const jutil::String &fileName) {
        C_STR(fileName_cstr, fileName);
        std:: ifstream ifs(fileName_cstr);
        if (!ifs.is_open()) {
            this->file = json::parse(ifs);
            if (this->file.is_discarded()) {
                this->valid = false;
                return;
            }
        }

        this->file = json::parse(ifs);
        this->valid = true;
    }

    Object::Object(const char *fileName) : Object(jutil::String(fileName)) {}

    Object::Object(const json &input) {
        this->file = input;
        valid = true;
    }

    bool Object::isValid() const {
        return this->valid;
    }

    size_t Object::size() const {
        return this->file.size();
    }

    jutil::String Object::translateString(const std::string &str) {
        return jutil::String(str.c_str());
    }

    std::string Object::translateString(const jutil::String &str) {
        char arr[str.size() - 1];
        str.array(arr);
        return std::string(arr);
    }
}