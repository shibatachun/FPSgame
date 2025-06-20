#pragma once
#include <string>
struct FileInfo {
    std::string path;
    std::string name;
    std::string ext;
    std::string dir;
    //std::string GetFileNameWithExtension() {
    //    if (ext.length() > 0) {
    //        return name + "." + ext;
    //    }
    //    else {
    //        return name;
    //    }
    //}
};