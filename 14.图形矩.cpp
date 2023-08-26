#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 多边形测试
 */
void test1() {
    int r = 100;
    //画六边形
    Mat src = Mat::zeros(Size(4 * r, 4 * r), CV_8UC1);
    vector<Point2f> vert(6);
    vert[0] = Point(1.5 * r, 1.34 * r);
    vert[1] = Point(1 * r, 2 * r);
    vert[2] = Point(1.5 * r, 2.866 * r);
    vert[3] = Point(2.5 * r, 2.866 * r);
    vert[4] = Point(3 * r, 2 * r);
    vert[5] = Point(2.5 * r, 1.34 * r);

    for (int j = 0; j < 6; j++) {
        line(src, vert[j], vert[(j + 1) % 6], Scalar(255), 3, 8);
    }
    //查找轮廓
    vector<vector<Point>> contours;
    findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    //计算每个像素点到六边形的距离
    Mat raw_dist(src.size(), CV_32FC1);
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            /**
             * pointPolygonTest() 用于测试一个点是否在多边形上
             * 最后一个参数measureDist设为true时：返回点到多边形的距离
             * （1）如果返回值为正，表示点在多边形内部；
             * （2）如果返回值为负，表示点在多边形外部
             * （3）0表示在多边形上
             *
             * measureDist设为false时：只返回-1，0，1
             * （1）1表示在多边形内部
             * （2）-1表示在多边形外部
             * （3）0表示在多边形上
             */
            raw_dist.at<float>(row, col) = pointPolygonTest(contours[0], Point2f(col, row), true);
        }
    }

    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            if (raw_dist.at<float>(row, col) < 0) {//外面
                drawing.at<Vec3b>(row, col)[0] = saturate_cast<int>(raw_dist.at<float>(row, col));
            } else if (raw_dist.at<float>(row, col) > 0) { //里面
                drawing.at<Vec3b>(row, col)[2] = saturate_cast<int>(raw_dist.at<float>(row, col));
            } else {  //矩形上面
                drawing.at<Vec3b>(row, col)[0] = 255;
                drawing.at<Vec3b>(row, col)[1] = 255;
                drawing.at<Vec3b>(row, col)[2] = 255;
            }
        }
    }

    imshow("src", drawing);

    waitKey(0);
}

/**
 * 图片分水岭，用于图像切割
 * https://zhuanlan.zhihu.com/p/67741538
 * https://blog.csdn.net/dcrmg/article/details/52498440
 */
void test2() {
    Mat src = imread("a.jpeg");
    Mat markers;
    watershed(src, markers);
}

int main() {
    test2();
}
