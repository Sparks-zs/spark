#include "sfile.h"
#include "base64.h"
#include <fstream>
#include <sstream>
#include <spark/HTTP.h>

using namespace std;
namespace fs = std::filesystem;

SFile::SFile(const string& path)
    : _path(path){
    if(fs::exists(path))
        _path = _path.lexically_normal();
}

SFile::~SFile()
{

}

bool SFile::read(Buffer* buffer, uint64_t from, uint64_t to){
    string data;
    bool ret = read(data, from, to);
    buffer->append(data);
    return ret;
}

bool SFile::read(string& in, uint64_t from, uint64_t to){
    if(!fs::exists(_path) || isDirectory()){
        LOG_ERROR << _path << "读取文件不存在";
        return false;
    }
    
    ifstream infile(_path, ios::in | ios::binary);
    if(!infile){
        LOG_ERROR << _path << "文件打开错误";
        return false;
    }

    uint64_t file_size = size();
    if(file_size == 0){
        in = "";
        return true;
    }
    if(to == 0 || to > file_size) to = file_size;
    if(from >= file_size || to <= from) return false;
    uint64_t read_size = to - from;

    string data;
    data.resize(read_size);

    infile.seekg(from);
    if(!infile.read(&data[0], read_size)){
        LOG_ERROR << _path << "读取失败";
        return false;
    }

    //LOG_DEBUG << "base64 encode start"; 
    //string encoded_data = base64_encode(reinterpret_cast<const unsigned char*>(data.c_str()), data.size());
    //in = move(encoded_data);
    //LOG_DEBUG << "base64 encode finished";

    in = data;
    return true;
}

uint64_t SFile::write(const string&& data, uint64_t from) const{
    if(exists() && isDirectory()){
        LOG_ERROR << _path << "写入文件类型为文件夹";
        return 0;
    }

    // 不存在则先创建文件
    if(!exists()){
        ofstream file(_path, ios::out);
        file.close();
    }

    ofstream outfile(_path, ios::in | ios::out | ios::binary );

    if(!outfile.is_open()){
        LOG_ERROR << _path << "文件打开错误";
        return 0;
    }
    
    if(from > 0)
        outfile.seekp(from, ios::beg);
    outfile.write(data.c_str(), data.size());
    outfile.flush();

    uint64_t size = static_cast<uint64_t>(outfile.tellp()) - from;
    return size;
}

bool SFile::exists() const{
    return fs::exists(_path);
}

bool SFile::isDirectory() const{
    if(!exists()) return false;
    return fs::is_directory(_path);
}

bool SFile::isRegularFile() const{
    if(!exists()) return false;
    return fs::is_regular_file(_path);
}

uint64_t SFile::size(){
    if(!exists()) return false;
    if(isDirectory()){
        return directory_size(_path);
    }
    else return fs::file_size(_path);
}

uint64_t SFile::directory_size(const fs::path& dir_path){
    uint64_t size = 0;

    for(auto& p : fs::directory_iterator(dir_path)){
        if(p.is_directory()){
            size += directory_size(p);
        }
        else size += p.file_size();
    }

    return size;
}

const string SFile::name() const{
    return _path.filename();
}

bool SFile::rename(std::string new_name){
    if(!exists()) return false;
    fs::path new_path = _path.parent_path().string() + '/' + new_name;
    fs::rename(_path, new_path);
    _path = new_path;
    if(!exists()) return false;
    return true;
}

bool SFile::createDir(){
    if(exists()){
        return false;
    }
    return fs::create_directory(_path);
}

bool SFile::remove_all(){
    if(!exists()) return false;
    fs::remove_all(_path);
    return true;
}

bool SFile::remove(){
    if(!exists() || isDirectory()) return false;
    return fs::remove(_path);
}

const fs::path& SFile::path() const{
    return _path;
}

vector<string> SFile::listdir(){
    if(!fs::exists(_path) || !isDirectory()){
        LOG_ERROR << _path << "读取文件不存在或不为文件夹";
        return {};
    }

    vector<string> ret;
    for(auto& p : fs::directory_iterator(_path)){
        ret.push_back(fs::relative(p.path(), _path));
    }
    return ret;
}

/*
string SFile::getFileType(const string& path)
{
    // 判断文件类型
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
*/