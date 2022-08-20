#ifndef HTTP_H
#define HTTP_H

#include <unordered_set>
#include <unordered_map>
#include <string>

enum HTTP_CODE_STATUS{
    OK = 200,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405
};

const std::unordered_map<int, std::string> HTTP_CODE_REASON = {
    { OK, "OK" },
    { BAD_REQUEST, "Bad Request" },
    {UNAUTHORIZED, "Unauthorized"},
    { FORBIDDEN, "Forbidden" },
    { NOT_FOUND, "Not Found" },
    {METHOD_NOT_ALLOWED, "Method not allowed"}
};

enum HTTP_METHOD{
    GET = 0,
    HEAD,
    POST,
    PUT,
    TRACE,
    OPTIONS,
    DELETE
};

#endif // HTTP_H