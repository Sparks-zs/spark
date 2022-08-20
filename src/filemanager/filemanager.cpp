#include "filemanager.h"

using namespace std;

const unordered_map<string, string> FileManager::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

FileManager::FileManager()
{
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
    curFilePath_ = "";
}

FileManager::~FileManager()
{
    UnmapFile();
}

char* FileManager::Get(string path, int from, int to)
{
    if(stat(path.data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode))
        return;
    if(mmFileStat_.st_mode & S_IROTH)  // 无权限
        return;

    int fileFd = open(path.data(), O_RDONLY);    
    if(fileFd < 0){
        LOG_DEBUG << "File NotFound!";
        return;
    }

    int length = to == -1 ? mmFileStat_.st_size : to - from;       
    int* mmRet = (int*)mmap(0, length, PROT_READ, MAP_PRIVATE, fileFd, from);
    if(*mmRet == -1){
        LOG_DEBUG << "mmap failed";
        return;
    }
    mmFile_ = (char*)mmRet;
    close(fileFd);

    return mmFile_;
}

void FileManager::UnmapFile()
{
    if(mmFile_) {
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
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

string FileManager::GetFileType(string path)
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

size_t FileManager::Size()
{
    return mmFileStat_.st_size;
}