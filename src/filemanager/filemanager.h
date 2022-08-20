#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, unmap
#include <dirent.h>     
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "../log/Log.h"
#include "../buffer/Buffer.h"

class FileManager
{
public:
    FileManager();
    ~FileManager();

    // 文件处理模块
    void readFile(const std::string& path, Buffer* buff);
    size_t Size();
    void UnmapFile();
    char* Get(std::string file, int from = 0, int to = -1);
    std::vector<std::string> GetSubDirName(std::string path);
    std::string GetFileType(std::string path);

private:
    size_t GetSubDirNum(std::string dirPath);

    char* mmFile_;
    struct stat mmFileStat_;
    std::string curFilePath_;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;  // 文件后缀对应的编码类型
};


#endif FILEMANAGER_H