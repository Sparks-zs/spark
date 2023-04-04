纯c++实现的http网络服务框架

仅需几行代码就可实现http服务的搭建

```
int main(){
  HttpServer server(8000);
      server.get("/", [](const HttpRequest& request, HttpResponse& response){
          response.setContent(Buffer("hello world"), "text/plain");
          response.setCodeState(OK);
      });
  server.start();
}
```

浏览器打开ip:8000

![image](https://user-images.githubusercontent.com/44738662/229850183-e64bd4d0-fccf-4e7e-8b85-ad7cd47199e5.png)
