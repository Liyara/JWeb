#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "jws/Object.h"

namespace jws {

    class ServerBase;
    class SessionServer;
    class DatabaseServer;
    class Server;
    class Session;

    class Endpoint {
        public:
        Endpoint(const jutil::String&, ServerBase*);
        const jutil::String &getPath() const;
        virtual void onLoad(const jws::Request&, jws::Response&) = 0;

        protected:
        jutil::String path;
        ServerBase *getServerBase();
        SessionServer *getSessionServer();
        Server *getServer();
        virtual Session *getSession(const jws::Request&);
        virtual void sendSession(const Session*, jws::Response&);

        private:
        ServerBase *srv;
    };
}

#endif