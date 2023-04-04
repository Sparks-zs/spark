#include "HttpServer.h"

int main()
{   
    HttpServer server(8000);

    server.get("/", [](const HttpRequest& request, HttpResponse& response){
        response.setContent(Buffer("hello world"), "text/plain");
        response.setCodeState(OK);
    });

    server.start();

    return 0;
}