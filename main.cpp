#include <cassert>
#include <locale>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <glog/logging.h>
#include <leveldb/db.h>
#include "cvxFont.hpp"
#include "pathUtils.hpp"
#include "strUtil.hpp"


cv::Mat crop(cv::Mat src) {
    assert(src.data);

    cv::Mat gray;
    cv::cvtColor(src, gray, CV_BGR2GRAY);

    cv::Mat thresMat;
    cv::threshold(gray, thresMat, 125, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

    cv::Mat result = thresMat;
    int left = 0, top = 0, width = result.cols - 1, height = result.rows - 1;

    bool flag = false;
    for (int i=0; i<result.cols; i++) {
        for (int j=1; j<result.rows; j++) {
            if (result.at<uchar>(j, i) != 255) {
                //                left = i - 1 >= 0 ? i - 1 : 0;
                left = i;
                flag = true;
                break;
            }
        }
        if (flag == true) {
            break;
        }
    }

    flag = false;
    for (int i=0; i<result.rows; i++) {
        for (int j=1; j<result.cols; j++) {
            if (result.at<uchar>(i, j) != 255) {
//                top = i - 1 >= 0 ? i - 1 : 0;
                top = i;
                flag = true;
                break;
            }
        }
        if (flag == true) {
            break;
        }
    }

    flag = false;
    for (int i=result.cols-1; i>=0; i--) {
        for (int j=1; j<result.rows; j++) {
            if (result.at<uchar>(j, i) != 255) {
//               width = i + 1 < result.cols ? i + 1 : result.cols - 1;
                width = i;
                flag = true;
                break;
            }
        }
        if (flag == true) {
            break;
        }
    }

    flag = false;
    for (int i=result.rows-1; i>=0; i--) {
        for (int j=1; j<result.cols; j++) {
            if (result.at<uchar>(i, j) != 255) {
                //                height = i + 1 < result.rows ? i + 1 : result.rows - 1;
                height = i;
                flag = true;
                break;
            }
        }
        if (flag == true) {
            break;
        }
    }

    cv::Rect rect(left, top, width - left + 1, height - top + 1);
    cv::Mat crop = src(rect);
    return crop;
}

std::wstring readFile(std::string path) {
    std::ios_base::sync_with_stdio(false);
    std::wcout.imbue(std::locale("zh_CN.utf8"));

    std::wifstream in(path.c_str(), std::ios::binary);
    in.imbue(std::locale("zh_CN.utf8"));

    if (!in.is_open()) {
        LOG(ERROR) << "file " + path << " does not exists";
        std::exit(1);
    }

    std::wfilebuf *pbuf = in.rdbuf();

    long size = pbuf->pubseekoff(0, std::ios::end, std::ios::in);
    pbuf->pubseekpos(0, std::ios::in);

    wchar_t *buffer = new wchar_t[size];
    pbuf->sgetn(buffer, size);

    std::wstring str(buffer);
    str.erase(std::remove_if(str.begin(), str.end(), [](char x){return std::isspace(x);}), str.end());
    str.erase(std::remove_if(str.begin(), str.end(), [](char x){return std::iswspace(x);}), str.end());
    str.erase(std::remove_if(str.begin(), str.end(), [](char x){return std::isblank(x);}), str.end());
    str.erase(std::remove_if(str.begin(), str.end(), [](char x){return std::iswblank(x);}), str.end());

    std::ios_base::sync_with_stdio(true);
    return str;
}

int main() {
    std::setlocale(LC_ALL, "en_US.utf8");
    std::string imgDir = "output/images/";
    if (!Path::createDir(imgDir)) {
        exit(1);
    }

    std::string annDir = "output/annotations/";
    if (!Path::createDir(annDir)) {
        exit(1);
    }

    boost::filesystem::path path("words");
    if (!boost::filesystem::exists(path)) {
        exit(1);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis1(1, 8);
    std::uniform_int_distribution<> dis2(0, 255);

    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, annDir, &db);
    assert(status.ok());

    long  index = 0;
    std::string tmpDir;
    for (boost::filesystem::directory_iterator it(path); it!=boost::filesystem::directory_iterator(); it++) {
        CvxFont font("simsun.ttc");

        std::wstring wstr = readFile(it->path().string().c_str());
        int pos = 0;

        while (pos < wstr.size()) {
            int cnt = 0;
            if (pos + cnt < wstr.size()) {
                if (index % 10000 == 0) {
                    tmpDir = imgDir + std::to_string(index / 10000) + "/";
                    if (!Path::createDir(tmpDir)) {
                        exit(1);
                    }
                }
                for (int i=0; i<10; i++) {
                    cnt = dis1(gen);
                    std::wstring t = wstr.substr(pos, cnt);
                    LOG(INFO) << "pos "<< pos << " generate " << cnt << " chars " << t << std::endl;

                    font.setPen(cv::Point(0, 0));
                    font.setFontSize(32);
                    cv::Mat img;

                    if (i == 1) {
                        img = cv::Mat(40, 32*cnt, CV_8UC3, cv::Scalar(255, 255, 255));
                        font.strToMat(img, t, cv::Scalar(0, 0, 0));

                    } else {
                        img = cv::Mat(40, 32*cnt, CV_8UC3, cv::Scalar(dis2(gen), dis2(gen), dis2(gen)));
                        font.strToMat(img, t, cv::Scalar(0, 0, 0));
                    }

                    status = db->Put(leveldb::WriteOptions(), std::to_string(index), wstr2str(t));
                    if (status.ok()) {
                        std::string imgPath = tmpDir + std::to_string(index) + ".jpg";
                        cv::imwrite(imgPath, crop(img));
                    }

//                   std::string result;
//                   status = db->Get(leveldb::ReadOptions(), std::to_string(index), &result);
//                   assert(status.ok());
//                   std::cout << result << std::endl;

                    index++;
                }
            }
            pos += cnt;
        }
    }

    //    CvxFont font("fonts/msyhbd.ttf");
    //    font.setPen(cv::Point(20, 20));
    //    //font.setMatrix(0.5);
    //    font.setFontSize(64);
    ////    std::wstring t = L"王青泽";
    ////    std::wstring t = L"王";
    //
    //
    ////    std::wstring t = L"abc";
    ////	cv::Mat img(64, 64, CV_8UC3, cv::Scalar(255, 255, 255));
    ////    font.strToMat(img, t, cv::Scalar(255, 0, 0));
    //
    ////    wchar_t *t = L"啊";
    //////    wchar_t *t = L"b";
    ////	cv::Mat img(64, 64, CV_8UC3, cv::Scalar(255, 255, 255));
    ////    font.charToMat(img, t, cv::Scalar(255, 0, 0));
    //
    //    wchar_t *t = L"啊";
    //	cv::Mat img(64, 64, CV_8UC1, cv::Scalar(255, 255, 255));
    //    cv::Mat result = font.charToMat(img, t, cv::Scalar(255, 0, 0));
    //	cv::imwrite("result.png", result);
    return 0;
}
