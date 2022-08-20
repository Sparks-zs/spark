#include "HttpResponse.h"
#include "../buffer/Buffer.h"

using namespace std;

const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

const unordered_map<int, string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

HttpResponse::HttpResponse()
    : _code(-1)
{

}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::init()
{

}

void HttpResponse::makeResponse(Buffer& buff)
{
    _addStateLine(buff);
    _addHeader(buff);
    _addBody(buff);
}

void HttpResponse::_addStateLine(Buffer& buff)
{

}

void HttpResponse::_addHeader(Buffer& buff)
{

}

void HttpResponse::_addBody(Buffer& buff)
{

}