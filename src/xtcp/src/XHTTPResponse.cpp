#include "XHTTPResponse.h"

#include <regex>

class XHTTPResponse::PImpl
{
public:
    PImpl(XHTTPResponse *owenr);
    ~PImpl();

public:
    XHTTPResponse *owenr_ = nullptr;

    int filesize = 0;
    FILE *fp = NULL;
};

XHTTPResponse::PImpl::PImpl(XHTTPResponse *owenr) : owenr_(owenr) {}

XHTTPResponse::PImpl::~PImpl() = default;

XHTTPResponse::XHTTPResponse() { impl_ = std::make_shared<PImpl>(this); }

XHTTPResponse::~XHTTPResponse() = default;

auto XHTTPResponse::setRequest(std::string request) -> bool
{
    if (request.empty())
    {
        return false;
    }

    /// Responding to HTTP GET requests
    // GET /favicon.ico HTTP/1.1
    // Host: 127.0.0.1:8080
    // Connection: keep-alive
    // sec-ch-ua-platform: "Windows"
    // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko)
    // Chrome/129.0.0.0 Safari/537.36 sec-ch-ua: "Google Chrome";v="129", "Not=A?Brand";v="8",
    // "Chromium";v="129" sec-ch-ua-mobile: ?0 Accept:
    // image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8 Sec-Fetch-Site: same-origin
    // Sec-Fetch-Mode: no-cors
    // Sec-Fetch-Dest: image
    // Referer: http://127.0.0.1:8080/
    // Accept-Encoding: gzip, deflate, br, zstd
    // Accept-Language: zh-CN,zh;q=0.9

    std::string src = request;
    // /   /index.html /ff
    std::string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1";
    std::regex r(pattern);
    std::smatch mas;
    std::regex_search(src, mas, r);
    if (mas.size() == 0)
    {
        printf("%s failed!\n", pattern.c_str());
        return false;
    }
    std::string type = mas[1];
    std::string path = "/";
    path += mas[2];
    std::string filetype = mas[3];
    std::string query = mas[4];
    if (!filetype.empty())
    {
        filetype = filetype.substr(1, filetype.size() - 1);
    }
    printf("type:[%s]\npath:[%s]\nfiletype:[%s]\nquery:[%s]\n", type.c_str(), path.c_str(), filetype.c_str(),
           query.c_str());


    if (type != "GET")
    {
        printf("Not GET!!!\n");
        return false;
    }

    std::string filename = path;
    if (path == "/")
    {
        filename = "/index.html";
    }

    std::string filepath = "www";
    filepath += filename;

    // php-cgi www/index.php id=1 name=xcj >  www/index.php.html
    if (filetype == "php")
    {
        std::string cmd = "php-cgi ";
        cmd += filepath;
        cmd += " ";
        // query id=1&name=xcj
        //  id=1 name=xcj
        for (int i = 0; i < query.size(); i++)
        {
            if (query[i] == '&')
                query[i] = ' ';
        }
        cmd += query;

        cmd += " > ";
        filepath += ".html";
        cmd += filepath;

        // printf("%s\n",cmd.c_str());
        system(cmd.c_str());
    }


    impl_->fp = fopen(filepath.c_str(), "rb");
    if (impl_->fp == NULL)
    {
        printf("open file %s failed!\n", filepath.c_str());
        return false;
    }
    /// 获取文件大小
    fseek(impl_->fp, 0, SEEK_END);
    impl_->filesize = ftell(impl_->fp);
    fseek(impl_->fp, 0, 0);
    printf("file size is %d\n", impl_->filesize);

    if (filetype == "php")
    {
        char c = 0;
        /// \r\n\r\n
        int headsize = 0;
        /// 逐行读取文件
        char line[256]; /// 用于存储每行读取的数据
        while (fgets(line, sizeof(line), impl_->fp))
        {
            headsize += strlen(line); /// 计算当前行的大小
            if (strcmp(line, "\r\n") == 0 || strcmp(line, "\n") == 0)
            {
                break; /// 找到空行，表示头部结束
            }
        }
        impl_->filesize = impl_->filesize - headsize;
    }

    return true;
}

auto XHTTPResponse::getHead() -> std::string const
{
    /// 回应http GET请求
    /// 消息头
    std::string rmsg = "";
    rmsg = "HTTP/1.1 200 OK\r\n";
    rmsg += "Server: XHttp\r\n";
    rmsg += "Content-Type: text/html\r\n";
    rmsg += "Content-Length: ";
    char bsize[128] = {0};
    sprintf(bsize, "%d", impl_->filesize);
    rmsg += bsize;
    // rmsg +=
    // rmsg += "10\r\n";
    rmsg += "\r\n\r\n";
    // rmsg += "0123456789";

    return rmsg;
}

auto XHTTPResponse::read(char *buf, int bufsize) -> int
{
    /// 发送正文
    return fread(buf, 1, bufsize, impl_->fp);
}
