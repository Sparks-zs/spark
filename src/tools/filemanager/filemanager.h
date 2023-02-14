#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fcntl.h>       // open
#include <dirent.h>   
#include <unistd.h>      // close
#include <string.h>
#include <sys/mman.h>    // mmap, unmap
#include <unordered_map>

#include "Log.h"
#include "Buffer.h"

class FileManager
{
public:
    FileManager();
    ~FileManager();

    // 文件处理模块
    void readToBuffer(const std::string& path, Buffer* buff, int from = 0, int to = -1);
    bool checkFilePath(const std::string& path);

    size_t size();
    std::string type() { return _fileType; }
    std::string getFileType(const std::string& path);

private:
    std::vector<std::string> GetSubDirName(std::string path);
    size_t GetSubDirNum(std::string dirPath);

    struct stat _fileState;
    std::string _filePath;
    std::string _fileType;
};


#endif //FILEMANAGER_H
