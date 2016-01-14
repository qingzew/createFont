#include <iostream>
#include <glog/logging.h>
#include <regex>
#include "pathUtils.hpp"

bool Path::createDir(const std::string &path) {
	std::regex re("^(?!-)[a-zA-Z0-9]+(/(?!-)[a-zA-Z0-9]+)*/$");
    if (!std::regex_match(path, re)) {
        LOG(ERROR) << "path must be digit and number, like: a/b/";
        exit(1);
    }
    char prefix[PATH_MAX + 1];
    if (getcwd(prefix, PATH_MAX) == NULL) {
        LOG(ERROR) << "load absolute path error";
        return false;
    }

    std::string curPath(prefix);

    bool flag = true;
    std::string p = path;
    int index = 0,  pre = 0;
    while ((index = p.find_first_of("/")) != std::string::npos) {
            std::string dir = path.substr(pre, index);
            pre = pre + index + 1;
            p = path.substr(pre);

            curPath = curPath + "/" + dir;
            if (access(curPath.c_str(), F_OK) == -1) {
                if (mkdir(curPath.c_str(), S_IRWXU|S_IRWXG|S_IRWXO) == -1) {
                    LOG(ERROR) << "create dir " + curPath << " error";
                    flag = false;
                }
            }
    }

    return flag;
}
