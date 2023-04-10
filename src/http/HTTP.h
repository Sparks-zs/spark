#ifndef HTTP_H
#define HTTP_H

#include <unordered_set>
#include <unordered_map>
#include <string>

enum HTTP_CODE_STATUS{
    NO_REQUEST = -1,
    OK = 200,
    NO_CONTENT = 204,
    PARTIAL_CONTENT = 206,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OHTER = 303,
    NOT_MOIDIFIED = 304,
    TEMPORARY_REDIRECT = 307,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503,
    
};

const std::unordered_map<int, std::string> HTTP_CODE_REASON = {
    { OK, "OK" },
    { NO_CONTENT, "No Content"},
    { PARTIAL_CONTENT, "Partial_Content"},
    { MOVED_PERMANENTLY, "moved permanently"},
    { FOUND, "found"},
    { SEE_OHTER, "see other"},
    { NOT_MOIDIFIED, "not moidified"},
    { TEMPORARY_REDIRECT, "temporary redirect"},
    { BAD_REQUEST, "Bad Request" },
    { UNAUTHORIZED, "Unauthorized"},
    { FORBIDDEN, "Forbidden" },
    { NOT_FOUND, "Not Found" },
    { METHOD_NOT_ALLOWED, "Method not allowed"},
    { REQUESTED_RANGE_NOT_SATISFIABLE, "REQUESTED RANGE NOT SATISFIABLE"},
    { INTERNAL_SERVER_ERROR, "internal server error"},
    { SERVICE_UNAVAILABLE, "service unavailable"}
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

const std::unordered_map<std::string, std::string> SUFFIX_TYPE = {
    { ".txt",   "text/plain" },
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
    { ".json",  "application/json"},
    { ".xhtml", "application/xhtml+xml" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
};


#define PLAIN "text/plain"
#define HTML "text/html"
#define JSON "application/json"
#define PNG "image/png"
#define MP4 "video/mp4"

#endif // HTTP_H