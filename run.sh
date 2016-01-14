rm binary
g++ main.cpp cvxFont.cpp pathUtils.cpp -std=c++11 -lopencv_core -lopencv_highgui \
    -lopencv_imgproc -lopencv_objdetect -lopencv_ml -I/usr/include/freetype2 -lfreetype \
    -lboost_system -lboost_filesystem -lglog -lleveldb -o binary -w -g
./binary 


