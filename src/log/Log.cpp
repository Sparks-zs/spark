#include "Log.h"
#include "../thread/thread.h"

using namespace std;

LogStream::LogStream() {
    _lineCount = -1;
    isAsync_ = false;
    writeThread_ = nullptr;
    deque_ = nullptr;
    toDay_ = 0;
    _curFilePath = "";
    _fileName = "";
}

LogStream::~LogStream() {
    if(writeThread_) {
        while(!deque_->empty()) {
            deque_->flush();
        };
        deque_->Close();
        writeThread_->join();
    }
    if(_output.is_open()) {
        lock_guard<mutex> locker(mtx_);
        _output.close();
    }
}

int LogStream::GetLevel() {
    lock_guard<mutex> locker(mtx_);
    return level_;
}

void LogStream::SetLevel(int level) {
    lock_guard<mutex> locker(mtx_);
    level_ = level;
}

void LogStream::init(int level, const char* dirPath, int maxQueueSize) 
{
    level_ = level;
    _dirPath = dirPath;
    _lineCount = -1;
    _fileName = "";
    if(maxQueueSize > 0) {
        isAsync_ = true;
        if(!deque_) {
            unique_ptr<BlockDeque<std::string>> newDeque(new BlockDeque<std::string>);
            deque_ = move(newDeque);
            
            std::unique_ptr<Thread> NewThread(new Thread(FlushLogThread, "LogThread"));
            writeThread_ = move(NewThread);
        }
    } else {
        isAsync_ = false;
    }

    updateFilePath();

    {
        lock_guard<mutex> locker(mtx_);
        if(_output.is_open()) { 
            flush();
            _temp.clear();
            _output.close(); 
        }

        _output.open(_curFilePath, std::ofstream::app);
        assert(_output.is_open());
    }
}

std::string LogStream::levelAsString()
{
    string level_str;
    switch(level_) {
    case 0:
        level_str = "DEBUG";
        break;
    case 1:
        level_str = "INFO";
        break;
    case 2:
        level_str = "WARN";
        break;
    case 3:
        level_str = "ERROR";
        break;
    default:
        level_str = "INFO";
        break;
    }
    return level_str;
}

void LogStream::updateFilePath()
{
    assert(_dirPath != "");

    time_t now = time(0);
    tm* ltm = localtime(&now);
    toDay_ = ltm->tm_mday;
    std::ostringstream fileName;
    fileName << setfill('0') << "TcpServer" << " " << 1900 + ltm->tm_year << setw(2) << 1 + ltm->tm_mon << \
                toDay_ << '-' << setw(2) << ltm->tm_hour << setw(2) << ltm->tm_min << \
                setw(2) << ltm->tm_sec << ".127.0.0.1." << CurrentThread::tid() << ".log";
    _curFilePath = _dirPath + fileName.str();
}

std::string LogStream::getFilePath()
{
    assert(_curFilePath != "");
    return _curFilePath;
}

void LogStream::flush() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int today = ltm->tm_mday;

    if(toDay_ != today){
        updateFilePath();
        {
            lock_guard<mutex>locker(mtx_);
            if(_output.is_open()){
                _output.flush();
                _output.close();
            }
            std::ofstream _output(_curFilePath, std::ofstream::app);
            assert(_output.is_open());
        }
    }

    std::ostringstream line;
    line << setfill('0') << 1900 + ltm->tm_year << setw(2) << 1 + ltm->tm_mon << \
            setw(2) << toDay_ << " " << setw(2) << ltm->tm_hour << ":" << \
            setw(2) << ltm->tm_min << ":" << setw(2) << ltm->tm_sec << " " << \
            CurrentThread::tid() << " " << levelAsString() << " " << _temp.str() << \
            " - " << _fileName << ":" << _lineCount << std::endl;
    _temp.str("");      // 清空
    
    {
        lock_guard<mutex> locker(mtx_);
        if(deque_ && !deque_->full()){
            deque_->push_back(line.str());
        }
        else{
            _output << line.str();
        }
    }
}

void LogStream::AsyncWrite_() {
    string str = "";
    while(deque_->pop(str)) {
        lock_guard<mutex> locker(mtx_);
        _output << str;
    }
}

LogStream* LogStream::Instance() {
    static LogStream inst;
    return &inst;
}

void LogStream::FlushLogThread() {
    LogStream::Instance()->AsyncWrite_();
}