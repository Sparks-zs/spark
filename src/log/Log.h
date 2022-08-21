/*
*   文件名: 程序名(进程名) 文件的创建时间 机器名称 进程id 文件后缀名(.log)
*   输出格式: 日期 时间 微妙 线程 级别 正文 - 源文件名:行号
*
*/

#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <ctime>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include <iostream>
#include<iomanip>
#include <fstream>
#include <sstream>

#include "BlockQueue.h"

class Thread;

class LogStream {
public:
    void init(int level, const char* dirPath = "./", 
              int maxQueueCapacity = 1024);

    template<class T>
    LogStream &operator<<(const T& t){
        _temp << t;
        return *this;
    }

    static LogStream* Instance();
    static void FlushLogThread();

    void flush();

    int GetLevel();
    void SetLevel(int level);
    std::string levelAsString();

    bool IsOpen() { 
        return _output.is_open();
    }

    void updateFilePath();
    std::string getFilePath();
    
    void setOpt(std::string fileName, int linecount){
        _fileName = fileName;
        _lineCount = linecount;
    }

private:
    LogStream();
    ~LogStream();
    void AsyncWrite_();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;
    int MAX_LINES_;

    const char* _dirPath;
    std::string _curFilePath;
    std::string _fileName;  // 日志打印所在的源文件
    int _lineCount;         // 日志打印所在的源文件的行数
    int toDay_;

    int _level;
    bool isAsync_;

    std::ostringstream _temp;   // 存储当前输入的字符，最后合并输出
    std::ofstream _output;
    std::unique_ptr<BlockDeque<std::string>> deque_; 
    std::unique_ptr<Thread> writeThread_;
    std::mutex mtx_;
};

class Logger
{
public:
    Logger(const std::string& file, int line){
        LogStream::Instance()->setOpt(file, line);
    }
    
    ~Logger(){
        LogStream::Instance()->flush();
    }

    LogStream& stream(){
        return *LogStream::Instance();
    }

    static int getLevel(){
        return LogStream::Instance()->GetLevel();
    }
};

#define LOG_BASE(level, FILE, LINE) \
    if (Logger::getLevel() <= level) \
        Logger(FILE, LINE).stream()

#define LOG_DEBUG LOG_BASE(0, __FILE__, __LINE__)
#define LOG_INFO LOG_BASE(1, __FILE__, __LINE__)
#define LOG_WARN LOG_BASE(2, __FILE__, __LINE__)
#define LOG_ERROR LOG_BASE(3, __FILE__, __LINE__)

#endif //LOG_H