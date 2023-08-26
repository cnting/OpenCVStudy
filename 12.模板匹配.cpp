#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 模板匹配
 */
void test1() {
    Mat src = imread("team.webp");
    Mat find = imread("find.png");
    //创建输出结果的矩阵
    Mat result(src.rows - find.rows + 1, src.cols - find.cols + 1, CV_32FC1);
    int method = TM_SQDIFF;
    matchTemplate(src, find, result, method);
    //从结果里找最小值
    double minVal = 0;
    double maxVal = 0;
    Point minLoc;
    Point maxLoc;
    //定位最匹配的位置
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    Point matchLoc;
    // 对于方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值代表更高的匹配结果. 而对于其他方法, 数值越大匹配越好
    if (method == TM_SQDIFF || method == TM_SQDIFF_NORMED) {
        matchLoc = minLoc;
    } else {
        matchLoc = maxLoc;
    }

    rectangle(src, matchLoc, Point(matchLoc.x + find.cols, matchLoc.y + find.rows), Scalar(0, 0, 255), 2, 8);
    imshow("src", src);
    waitKey(0);
}

int main() {
    test1();
}