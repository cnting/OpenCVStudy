#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 上采样：放大。采用拉普拉斯。
 * 降采样：缩小。先对图像进行高斯模糊，再过滤左右临近点
 */
void test1() {
    Mat src = imread("a.jpeg");
    Mat dst;
    pyrUp(src, dst, Size(src.cols * 2, src.rows * 2));
    imshow("dst", dst);
    waitKey(0);
}

void test2() {
    Mat src = imread("a.jpeg");
    Mat dst;
    //使用高斯，会比较慢
    pyrDown(src, dst, Size(src.cols * 0.5, src.rows * 0.5));
    imshow("dst", dst);
    waitKey(0);
}

int main() {
    test1();
}