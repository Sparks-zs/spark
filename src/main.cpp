#include "HttpServer.h"
#include "filemanager.h"

int main()
{   
    LogStream::Instance()->init(0, "../log/");
    HttpServer server(8888);
    server.Get("/index.html", [](const HttpRequest& request, HttpResponse& response){
        Buffer buff;
        FileManager filemanager;
        filemanager.readToBuffer("../www" + request.getPath(), &buff);
        response.setContent(buff, filemanager.type());
    });
    server.start();

    return 0;
}