#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 重映射remap：按照原图按照新定的满足一定规律重新赋值的操作
 * https://blog.csdn.net/shuiyixin/article/details/79961040
 */
void test1() {
    Mat src = imread("a.jpeg");
    Mat dst;
    Mat map_x(src.size(), CV_32FC1);
    Mat map_y(src.size(), CV_32FC1);
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            //这里定义的不是像素，而且取转换后的像素的位置
            //左右调换
//            map_x.at<float>(row, col) = static_cast<float >(src.cols - col);
//            map_y.at<float>(row, col) = row;

            //上下调换
//            map_x.at<float>(row, col) = col;
//            map_y.at<float>(row, col) = static_cast<float >(src.rows - row);

            //上下左右调换
//            map_x.at<float>(row, col) = static_cast<float >(src.cols - col);
//            map_y.at<float>(row, col) = static_cast<float >(src.rows - row);

        }
    }

    //map1:x方向的重映射规律
    //map2:y方向的重映射规律
    //interpolation:插值方式，因为输出图像的尺寸不一定与原图像相同，而与映射表尺寸相同。比如比原图大，这时候就需要用插值运算了
    remap(src, dst, map_x, map_y, INTER_NEAREST);
    imshow("dst", dst);
    waitKey(0);
}

int main() {
    test1();
}