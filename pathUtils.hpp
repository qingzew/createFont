#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>

class Path {
public:
    static bool createDir(const std::string &path);
};

#endif
