#ifndef SERVER_H
#define SERVER_H

#include "jws/Endpoint.h"
#include "jws/SessionManager.h"
#include "jws/Database.hpp"

namespace jws {
    class ServerBase {
        public:
        ServerBase(Object&);
        virtual ~ServerBase();
        void listen();
        void addEndpoint(Endpoint*);

        protected:
        ServerBase();
        void __init__(Object&);
        jutil::Queue<Endpoint*> pages;
        jutil::String ip, mountPoint;
        jutil::Queue<jutil::String> defaultFiles;
        int port;
        httplib::SSLServer *srv;
        virtual void _handle_default(const jws::Request&, jws::Response&);

        private:
        bool initialized;
    };

    class SessionServer : virtual public ServerBase, public SessionManager {
        public:
        //Server Config, Session Config
        SessionServer(Object&, Object&);
        virtual ~SessionServer();

        protected:
        SessionServer();
    };

    class DatabaseServer : virtual public ServerBase, public Database {
        public:
        //Server Config, DB Config
        DatabaseServer(Object&, Object&);
        virtual ~DatabaseServer();

        protected:
        DatabaseServer();
    };

    class Server : public SessionServer, public DatabaseServer {
        public:
        //Server Config, Session Config, DB Config
        Server(Object&, Object&, Object&);
    };
}

#endif