#include "Log.h"
#include "../thread/thread.h"

using namespace std;

LogStream::LogStream() {
    writeThread_ = nullptr;
    deque_ = nullptr;
    _toDay = 0;
    _curFilePath = "";
}

LogStream::~LogStream() {
    if(writeThread_) {
        while(!deque_->empty()) {
            deque_->flush();
        };
        deque_->Close();
    }
    if(_output.is_open()) {
        lock_guard<mutex> locker(mtx_);
        _output.close();
    }
}

int LogStream::GetLevel() {
    lock_guard<mutex> locker(mtx_);
    return _level;
}

void LogStream::SetLevel(int level) {
    lock_guard<mutex> locker(mtx_);
    _level = level;
}

void LogStream::init(int level, const char* dirPath) 
{
    _level = level;
    _dirPath = dirPath;
    if(!deque_) {
        unique_ptr<BlockDeque<std::string>> newDeque(new BlockDeque<std::string>);
        deque_ = move(newDeque);
        
        std::unique_ptr<Thread> NewThread(new Thread(_flushLogThread, "LogThread"));
        writeThread_ = move(NewThread);
        writeThread_->detach();
    }
    
    updateFilePath();

    {
        lock_guard<mutex> locker(mtx_);
        if(_output.is_open()) {
            LOG_WARN << "日志流已初始化过";
            _output.close(); 
        }

        _output.open(_curFilePath, std::ofstream::app);
        assert(_output.is_open());
    }
}

std::string LogStream::levelAsString()
{
    string level_str;
    switch(_level) {
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
    _toDay = ltm->tm_mday;
    std::ostringstream fileName;
    fileName << setfill('0') << "TcpServer" << " " << 1900 + ltm->tm_year << setw(2) << 1 + ltm->tm_mon << \
                _toDay << '-' << setw(2) << ltm->tm_hour << setw(2) << ltm->tm_min << \
                setw(2) << ltm->tm_sec << ".127.0.0.1." << CurrentThread::tid() << ".log";
    _curFilePath = _dirPath + fileName.str();
}

std::string LogStream::getFilePath()
{
    assert(_curFilePath != "");
    return _curFilePath;
}

void LogStream::flush(const std::string& msg, const std::string& filePath, int lineCount) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int today = ltm->tm_mday;

    if(_toDay != today){
        updateFilePath();
        {
            lock_guard<mutex>locker(mtx_);
            if(_output.is_open()){
                _output.close();
            }
            std::ofstream _output(_curFilePath, std::ofstream::app);
            assert(_output.is_open());
        }
    }

    std::ostringstream line;
    line << setfill('0') << 1900 + ltm->tm_year << setw(2) << 1 + ltm->tm_mon << \
            setw(2) << _toDay << " " << setw(2) << ltm->tm_hour << ":" << \
            setw(2) << ltm->tm_min << ":" << setw(2) << ltm->tm_sec << " " << \
            CurrentThread::tid() << " " << levelAsString() << " " << msg << \
            " - " << filePath << ":" << lineCount << std::endl;
    
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
        _output.flush();
    }
}

LogStream* LogStream::Instance() {
    static LogStream inst;
    return &inst;
}

void LogStream::_flushLogThread() {
    LogStream::Instance()->AsyncWrite_();
}