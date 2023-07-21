#include "jws/Server.h"

namespace jws {

    Endpoint::Endpoint(const jutil::String &pattern, ServerBase *svr) : path(pattern), srv(svr) {

    }

    const jutil::String &Endpoint::getPath() const {
        return this->path;
    }

    Session *Endpoint::getSession(const jws::Request &req) {
        SessionServer *srv;
        if (!(srv = this->getSessionServer())) return nullptr;
        const jutil::String &COOKIE_NAME = srv->getCookieName();
        const httplib::Headers &headers = req.headers;
        auto range = headers.equal_range("Cookie");
        for (auto it = range.first; it != range.second; ++it) {
            jutil::String _cookies(it->second.c_str());
            jutil::Queue<jutil::String> cookies = jutil::split(_cookies, ';');
            for (auto &_it: cookies) {
                jutil::Queue<jutil::String> cookie = jutil::split(_it, '=');
                if (cookie.size() > 1) {
                    if (jutil::trim(cookie[0]) == jutil::String(COOKIE_NAME)) {
                        Session *sess = srv->getSession(jutil::trim(cookie[1]));
                        return sess;
                    }
                }
            }
        }
        return nullptr;
    }

    void Endpoint::sendSession(const Session *sess, jws::Response &res) {
        SessionServer *srv;
        if (!(srv = this->getSessionServer())) return;
        const jutil::String &COOKIE_NAME = srv->getCookieName();
        jutil::String cookie = COOKIE_NAME + "=" + sess->getId() + "; Max-Age=31536000";
        C_STR(cookie_cstr, cookie);
        res.set_header("Set-Cookie", cookie_cstr);
    }

    ServerBase *Endpoint::getServerBase() {
        return this->srv;
    }
    SessionServer *Endpoint::getSessionServer() {
        SessionServer *ss = dynamic_cast<SessionServer*>(this->srv);
        return ss;
    }

    Server *Endpoint::getServer() {
        Server *s = dynamic_cast<Server*>(this->srv);
        return s;
    }
}