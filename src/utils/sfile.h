#ifndef SFILE_H
#define SFILE_H

#include <vector>
#include <filesystem>

#include <spark/Log.h>
#include <spark/Buffer.h>

class SFile
{
public:
    SFile(const std::string& path);
    ~SFile();

    bool read(Buffer* buffer, uint64_t from = 0, uint64_t to = 0);

    bool read(std::string& data, uint64_t from = 0, uint64_t to = 0);
    
    uint64_t write(const std::string&& data, uint64_t from = 0) const;
    
    uint64_t size();
    
    bool exists() const;
    
    bool isDirectory() const;
    
    bool isRegularFile() const;
    
    const std::string name() const;

    bool createDir();
    
    bool rename(std::string new_name);
    
    bool remove();
    
    bool remove_all();

    const std::filesystem::path& path() const;

    std::vector<std::string> listdir();
    
    uint64_t directory_size(const std::filesystem::path& path);

private: 
    std::filesystem::path _path;
};

#endif // SFILE_H
