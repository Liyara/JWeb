#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "jws/Session.h"

namespace jws {
    class SessionManager {
        public:
        SessionManager(Object&);
        Session* createSession();
        Session *getSession(const jutil::String&);
        const jutil::String &getCookieName() const;
        bool deleteSession(Session*);
        virtual ~SessionManager();

        protected:
        jutil::Queue<Session*> sessions;
        jutil::String context, cookie;
        jutil::Mutex mutex;
    };
}

#endif