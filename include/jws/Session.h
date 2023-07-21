#ifndef SESSION_H
#define SESSION_H

#include "jws/Object.h"

namespace jws {
    class Session : public Object {
        public:
        //Context (directory), SessionID
        Session(const jutil::String&, const jutil::String&);
        void writeFile();
        const jutil::String &getId() const;

        private:
        jutil::String context, id;
    };
}

#endif