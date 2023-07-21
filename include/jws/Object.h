#ifndef JSON_FILE_H
#define JSON_FILE_H

#include "jws/dependencies.h"

namespace jws {

    class Object;

    namespace __Object__helper {
        template <typename T>
        T get(const jutil::String&, const json&, Object*);

        template <typename T>
        T get(size_t index, const json&, Object*);

        template<>
        jutil::String get<jutil::String>(const jutil::String&, const json&, Object*);

        template<>
        jutil::String get<jutil::String>(size_t index, const json&, Object*);

        template<>
        Object get<Object>(const jutil::String&, const json&, Object*);

        template<>
        Object get<Object>(size_t index, const json&, Object*);

        template <typename T>
        void set(const jutil::String&, const T&, json&);

        template <typename T>
        void set(size_t, const T&, json&);

        template<>
        void set(const jutil::String&, const jutil::String&, json&);

        template<>
        void set(const jutil::String&, const Object&, json&);

        template<>
        void set(size_t, const jutil::String&, json&);

        template<>
        void set(size_t, const Object&, json&);
        
    }

    class Object {
        public:
        Object();
        explicit Object(const jutil::String&);
        explicit Object(const char*);
        explicit Object(const json&);

        bool isValid() const;
        size_t size() const;

        template <typename T = jutil::String>
        T get(const jutil::String &keyName) {
            return __Object__helper::get<T>(keyName, this->file, this);
        }

        template <typename T = jutil::String>
        T get(size_t index) {
            return __Object__helper::get<T>(index, this->file, this);
        }

        template <typename T>
        void set(const jutil::String &keyName, const T &value) {
            __Object__helper::set<T>(keyName, value, this->file);
        }

        template <typename T>
        void set(size_t index, const T &value) {
            __Object__helper::set<T>(index, value, this->file);
        }

        protected:
        static jutil::String translateString(const std::string&);
        static std::string translateString(const jutil::String&);
        template <typename T>
        friend T __Object__helper::get(const jutil::String&, const json&, Object*);
        template <typename T>
        friend T __Object__helper::get(size_t, const json&, Object*);
        template <typename T>
        friend void __Object__helper::set(const jutil::String&, const T&, json&);
        template <typename T>
        friend void __Object__helper::set(size_t, const T&, json&);
        json file;
        bool valid;
    };

    namespace __Object__helper {
        template <typename T>
        T get(const jutil::String &keyName, const json &file, Object *__p) {
            C_STR(keyName_cstr, keyName);
            T rawValue = file[keyName_cstr];

            return rawValue;
        }

        template <typename T>
        T get(size_t index, const json &file, Object *__p) {
            T rawValue = file[index];

            return rawValue;
        }

        template <typename T>
        void set(const jutil::String &keyName, const T &value, json &file) {
            C_STR(keyName_cstr, keyName);
            file[keyName_cstr] = value;
        }

        template <typename T>
        void set(size_t index, const T &value, json &file) {
            file[index] = value;
        }
    }
}

#endif