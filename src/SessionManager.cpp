#include "jws/SessionManager.h"

namespace jws {

    jutil::String __sha256__(const jutil::String &str) {
        C_STR(string, str);
        char outputBuffer[65];
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, string, strlen(string));
        SHA256_Final(hash, &sha256);
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
        }
        return jutil::String(outputBuffer);
    }

    bool _file_exists(const jutil::String &f) {
        jutil::File file(f, jutil::READ);
        return file.open();
    }

    SessionManager::SessionManager(Object &cfg) : mutex(jutil::createMutexObject()) {
        this->context = cfg.get("session_directory");
        this->cookie = cfg.get("session_cookie");
        if (this->context.last() != '/') this->context += "/";
    }

    const jutil::String &SessionManager::getCookieName() const {
        return this->cookie;
    }

    Session *SessionManager::createSession() {
        jutil::RNG rng;
        uint64_t n;
        jutil::String nstr;
        do {
            rng.get(&n);
            nstr = __sha256__(jutil::String(n));
        } while (_file_exists(this->context + nstr));
        Session *ns = new Session(this->context, nstr);
        jutil::mutexLock(&this->mutex);
        this->sessions.insert(ns);
        jutil::mutexUnlock(&this->mutex);
        return ns;
    }

    Session *SessionManager::getSession(const jutil::String &id) {
        //Check loaded sessions
        for (auto &it: this->sessions) {
            if (it->getId() == id) return it;
        }

        //Check unloaded sessions, load if found
        jutil::Directory dir(this->context);
        const auto &entries = dir.entries();
        for (const auto &entry: entries) {
            if (entry.name == id) {
                Session *ns = new Session(this->context, id);
                jutil::mutexLock(&this->mutex);
                this->sessions.insert(ns);
                jutil::mutexUnlock(&this->mutex);
                return ns;
            }
        }

        //No such session was found
        return nullptr;
    }

    bool SessionManager::deleteSession(Session *session) {
        size_t i;
        if (this->sessions.find(session, &i)) {
            jutil::mutexLock(&this->mutex);
            this->sessions.erase(i);
            jutil::mutexUnlock(&this->mutex);
            return true;
        }
        return false;
    }

    SessionManager::~SessionManager() {
        for (auto &it: this->sessions) {
            it->writeFile();
            delete it;
        }
    }
}