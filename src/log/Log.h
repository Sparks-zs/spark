/*
*   文件名: 程序名(进程名) 文件的创建时间 机器名称 进程id 文件后缀名(.log)
*   输出格式: 日期 时间 微妙 线程 级别 正文 - 源文件名:行号
*
*/

#ifndef LOG_H
#define LOG_H

#include <string>
#include <ctime>
#include <sys/time.h>
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include <iostream>
#include <iomanip>       // setfill
#include <fstream>
#include <sstream>

#include "BlockQueue.h"

class Thread;

class LogStream{
public:
    static LogStream* Instance();
    void init(int level, const char* dirPath);
    void flush(const std::string& msg, const std::string& filePath, int lineCount);

    int GetLevel();
    void SetLevel(int level);
    std::string levelAsString();
    void updateFilePath();
    std::string getFilePath();

private:
    LogStream();
    ~LogStream();
    void AsyncWrite_();
    static void _flushLogThread();

    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;
    int MAX_LINES_;

    const char* _dirPath;
    std::string _curFilePath;
    int _toDay;

    int _level;
    bool _isFlushing;

    std::ofstream _output;
    std::unique_ptr<BlockDeque<std::string>> deque_; 
    std::unique_ptr<Thread> writeThread_;
    std::mutex mtx_;
};

class Logger
{
public:
    Logger(const std::string& file, int line)
        : _filePath(file), _lineCount(line){}
    
    ~Logger(){
        LogStream::Instance()->flush(_temp.str(), _filePath, _lineCount);
    }
    
    template<class T>
    Logger &operator<<(const T& t){
        _temp << t;
        return *this;
    }

    static int getLevel(){
        return LogStream::Instance()->GetLevel();
    }

private:
    std::string _filePath;
    int _lineCount;
    std::ostringstream _temp;   // 存储当前输入的字符，最后合并输出
};

#define LOG_BASE(level, FILE, LINE) \
    if (Logger::getLevel() <= level) \
        Logger(FILE, LINE)

#define LOG_DEBUG LOG_BASE(0, __FILE__, __LINE__)
#define LOG_INFO LOG_BASE(1, __FILE__, __LINE__)
#define LOG_WARN LOG_BASE(2, __FILE__, __LINE__)
#define LOG_ERROR LOG_BASE(3, __FILE__, __LINE__)

#endif //LOG_H