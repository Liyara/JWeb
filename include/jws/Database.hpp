#ifndef DATABASE_H
#define DATABASE_H

#include "jws/Object.h"
#include <mariadb/conncpp.hpp>

namespace jws {

    class Database {
        public:

        typedef sql::ResultSet ResultSet;

        class Query {
            public:
            Query(const jutil::String &_sql, sql::Connection *c) : sql(_sql), conn(c), index(1) {
                C_STR(sql_cstr, this->sql);
                jutil::mutexLock(&this->mutex);
                this->stmt = this->conn->prepareStatement(sql_cstr);
                jutil::mutexUnlock(&this->mutex);
            }

            template <typename T>
            size_t addParameter(const T &param) {
                if (jutil::Convert<T, jutil::String>::Value) {
                    return addParameter(jutil::String(param));
                }
                return 0;
            }

            const jutil::String &getSQL() const {
                return this->sql;
            }


            private:
            jutil::String sql;
            sql::PreparedStatement *stmt;
            sql::Connection *conn;
            size_t index;

            friend class Database;
        };

        Database(Object &f) : mutex(jutil::createMutexObject()) {
            jutil::String
                hostname = f.get("hostname"),
                username = f.get("username"),
                password = f.get("password"),
                database = f.get("database")
            ;

            int port = f.get<int>("port");

            jutil::String _url = "jdbc:mariadb://" + hostname + ":" + jutil::String(port) + "/" + database;
            
            C_STR(username_cstr, username);
            C_STR(password_cstr, password);
            C_STR(url_cstr, _url);

            this->driver = sql::mariadb::get_driver_instance();
            sql::SQLString url(url_cstr);
            sql::Properties properties({{"user", username_cstr}, {"password", password_cstr}});
            this->conn = driver->connect(url, properties);
        }

        virtual ~Database() {}

        ResultSet *execute(Query &q) {
            jutil::mutexLock(&this->mutex);
            ResultSet *res = q.stmt->executeQuery();
            jutil::mutexUnlock(&this->mutex);
            return res;
        }

        Query prepare(const jutil::String &sql) {
            Query q(sql, this->conn);
            return q;
        }

        template <
            typename FirstParamT, 
            typename... RestParamsT
        >
        Query prepare(const jutil::String &sql, const FirstParamT &fp, const RestParamsT&... rp) {
            Query q(sql, this->conn);
            q.addParameter<FirstParamT>(fp);
            this->__prepare__(&q, sql, rp...);
            return q;
        }

        protected:

        void __prepare__(Query *q, const jutil::String &sql) {}

        template <
            typename FirstParamT, 
            typename... RestParamsT
        >
        void __prepare__(Query *q, const jutil::String &sql, const FirstParamT &fp, const RestParamsT&... rp) {
            q->addParameter<FirstParamT>(fp);
            this->__prepare__(q, sql, rp...);
        }

        jutil::String dbName, user, pass, ip;
        sql::Driver* driver;
        sql::Connection *conn;
        jutil::Mutex mutex;
    };

    #define ADDPARAM_INST(parent, child) \
        template <> \
        inline size_t Database::Query::addParameter<child>(const child &param) { \
            return addParameter<parent>(static_cast<parent>(param)); \
        }

    template <>
    inline size_t Database::Query::addParameter<int64_t>(const int64_t &param) {
        this->stmt->setInt64(this->index++, param);
        return (this->index - 1);
    }

    template <>
    inline size_t Database::Query::addParameter<uint64_t>(const uint64_t &param) {
        this->stmt->setUInt64(this->index++, param);
        return (this->index - 1);
    }

    template <>
    inline size_t Database::Query::addParameter<double>(const double &param) {
        this->stmt->setDouble(this->index++, param);
        return (this->index - 1);
    }

    template <>
    inline size_t Database::Query::addParameter<bool>(const bool &param) {
        this->stmt->setBoolean(this->index++, param);
        return (this->index - 1);
    }

    template <>
    inline size_t Database::Query::addParameter<jutil::String>(const jutil::String &param) {
        C_STR(param_cstr, param);
        this->stmt->setString(this->index++, param_cstr);
        return (this->index - 1);
    }

    ADDPARAM_INST(int64_t, int32_t);
    ADDPARAM_INST(int64_t, int16_t);
    ADDPARAM_INST(int64_t, int8_t);
    ADDPARAM_INST(uint64_t, uint32_t);
    ADDPARAM_INST(uint64_t, uint16_t);
    ADDPARAM_INST(uint64_t, uint8_t);
    ADDPARAM_INST(double, float);
}

#endif