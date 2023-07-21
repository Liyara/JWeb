#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <JUtil/jutil.h>

#define C_STR(a, b) char a[b.size() - 1]; b.array(a);

namespace jws {
    using json = nlohmann::json;
    using Request = httplib::Request;
    using Response = httplib::Response;
}

#endif