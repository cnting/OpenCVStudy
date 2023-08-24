#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * https://blog.csdn.net/shuiyixin/article/details/104415952
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

/**
 * 自己实现上采样，扩大2倍
 * 思路：取原图的一个像素，然后把那个像素周边的右上、左下、右下填充成同样像素
 */
void test3() {
    Mat src = imread("a.jpeg");
    Mat dst = Mat::zeros(src.size() * 2, src.type());

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            Vec3b v = src.at<Vec3b>(i, j);

            int di = i * 2;
            int dj = j * 2;
            dst.at<Vec3b>(di, dj)[0] = v[0];
            dst.at<Vec3b>(di, dj)[1] = v[1];
            dst.at<Vec3b>(di, dj)[2] = v[2];
            di = i * 2;
            dj = j * 2 + 1;
            dst.at<Vec3b>(di, dj)[0] = v[0];
            dst.at<Vec3b>(di, dj)[1] = v[1];
            dst.at<Vec3b>(di, dj)[2] = v[2];
            di = i * 2 + 1;
            dj = j * 2;
            dst.at<Vec3b>(di, dj)[0] = v[0];
            dst.at<Vec3b>(di, dj)[1] = v[1];
            dst.at<Vec3b>(di, dj)[2] = v[2];
            di = i * 2 + 1;
            dj = j * 2 + 1;
            dst.at<Vec3b>(di, dj)[0] = v[0];
            dst.at<Vec3b>(di, dj)[1] = v[1];
            dst.at<Vec3b>(di, dj)[2] = v[2];
        }
    }
    imshow("dst", dst);
    waitKey(0);
}

/**
 * 自己实现缩小一倍
 */
void test4() {
    Mat src = imread("a.jpeg");
    imshow("src", src);
    Mat dst = Mat::zeros(src.size() / 2, src.type());

    for (int i = 0; i < src.rows; i += 2) {
        for (int j = 0; j < src.cols; j += 2) {
            Vec3b v = src.at<Vec3b>(i, j);

            int di = i / 2;
            int dj = j / 2;
            dst.at<Vec3b>(di, dj)[0] = v[0];
            dst.at<Vec3b>(di, dj)[1] = v[1];
            dst.at<Vec3b>(di, dj)[2] = v[2];
        }
    }
    imshow("dst", dst);
    waitKey(0);
}

int main() {
    test4();
}