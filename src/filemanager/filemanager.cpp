#include "filemanager.h"
#include "../http/HTTP.h"

using namespace std;

FileManager::FileManager()
{
    _fileState = { 0 };
    _filePath = "";
    _fileType = "";
}

FileManager::~FileManager()
{
}

void FileManager::readToBuffer(const string& path, Buffer* buff, int from, int to)
{
    if(!checkFilePath(path)){
        LOG_ERROR << "File path is error";
        return;
    }

    _filePath = path;
    _fileType = getFileType(path);

    int fileFd = open(_filePath.data(), O_RDONLY);    
    if(fileFd < 0){
        LOG_ERROR << "File NotFound!";
        return;
    }

    int length = to == -1 ? _fileState.st_size : to - from;    
    mmap(buff->beginWrite(), length, PROT_READ, MAP_PRIVATE, fileFd, from);
    close(fileFd);
}

bool FileManager::checkFilePath(const string& path)
{
    if(stat(path.data(), &_fileState) < 0 || S_ISDIR(_fileState.st_mode))
        return false;
    if(_fileState.st_mode & S_IROTH)  // 无权限
        return false;
    return true;
}

std::vector<std::string> FileManager::GetSubDirName(std::string path)
{
    DIR *dir;
    struct dirent *ptr;
    vector<string> ret;

    if ((dir = opendir(path.c_str())) == NULL)
    {
        perror("Open dir error...");
    }      
	
    while((ptr = readdir(dir)) != NULL){
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 4)	// 目录
        {
            ret.push_back(ptr->d_name);
        }
        else if (ptr->d_type == 8)      // 文件
        {
            ret.push_back(ptr->d_name);
        }
    }
    closedir(dir);
    return ret;
}

size_t FileManager::GetSubDirNum(std::string dirPath)
{
    DIR *dir;
    struct dirent *ptr;
    size_t num = 0;

    if((dir = opendir(dirPath.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if(ptr->d_type == 4 || ptr->d_type == 8)   // 文件或目录
        {
            num++;
        }
    }
    closedir(dir);
    return num;
}

string FileManager::getFileType(const string& path)
{
    /* 判断文件类型 */
    string::size_type idx = path.find_last_of('.');
    if(idx == string::npos) {
        return "text/plain";
    }
    string suffix = path.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

size_t FileManager::size()
{
    return _fileState.st_size;
}