#include "jws/Server.h"

namespace jws {

    ServerBase::ServerBase(Object &cfg) : initialized(false) {
        this->__init__(cfg);
    }

    ServerBase::ServerBase() : initialized(false) {}

    void ServerBase::__init__(Object &cfg) {
        if (this->initialized) return;
        jutil::out << "Initializing Server..." << jutil::endl;
        jutil::String
            sslCert = cfg.get("ssl_cert"),
            sslKey = cfg.get("ssl_key"),
            serverIp = cfg.get("server_ip"),
            mountPoint = cfg.get("data_directory");
        ;

        Object defaultFiles = cfg.get<Object>("default_files");

        int serverPort = cfg.get<int>("server_port");

        for (size_t i = 0; i < defaultFiles.size(); ++i) {
            this->defaultFiles.insert(defaultFiles.get<jutil::String>(i));
        }

        this->ip = serverIp;
        this->port = serverPort;
        this->mountPoint = mountPoint;

        C_STR(sslCert_cstr, sslCert);
        C_STR(sslKey_cstr, sslKey);
        C_STR(mountPoint_cstr, mountPoint);

        this->srv = new httplib::SSLServer(sslCert_cstr, sslKey_cstr);
        this->srv->set_mount_point("/", mountPoint_cstr);

        this->initialized = true;
    }

    void ServerBase::_handle_default(const jws::Request &req, jws::Response &res) {
        jutil::String reqPath(req.path.c_str());
        for (auto &i: this->defaultFiles) {
            jutil::String serverAddr = reqPath + (reqPath.last() == '/'? i: "/" + i);
            jutil::String addr = mountPoint + serverAddr;
            jutil::File file(addr);
            if (file.open()) {
                jutil::String content;
                file.dump(&content);
                C_STR(content_cstr, content)
                res.set_content(content_cstr, "text/html");
                return;
            }
        }

        res.status = 404;
        res.set_content("Requested document not found; error 404.", "text/plain");
    }

    ServerBase::~ServerBase() {
        delete this->srv;
    }

    void ServerBase::listen() {

        for (auto &page: this->pages) {
            jutil::String path = page->getPath();
            C_STR(path_cstr, path)
            this->srv->Get(path_cstr, HTTP_RESPONSE(req, res) {
                page->onLoad(req, res);
            });
        }

        this->srv->Get(R"(/.*)", [this](const jws::Request& req, jws::Response& res) {
            this->_handle_default(req, res);
        });

        C_STR(ip_cstr, this->ip);
        this->srv->listen(ip_cstr, this->port);
    }

    void ServerBase::addEndpoint(Endpoint *wp) {
        this->pages.insert(wp);
    }

    SessionServer::SessionServer(Object &cfg, Object &sesscfg) : SessionManager(sesscfg) {
        this->__init__(cfg);
    }

    SessionServer::~SessionServer() {}

    DatabaseServer::DatabaseServer(Object &cfg, Object &dbcfg) : Database(dbcfg) {
        this->__init__(cfg);
    }

    DatabaseServer::~DatabaseServer() {}

    Server::Server(Object &cfg, Object &sesscfg, Object &dbcfg) : SessionServer(cfg, sesscfg), DatabaseServer(cfg, dbcfg) {}
}