#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 边缘检测的目的是标识数字图像中亮度变化明显的点。图像属性中的显著变化通常反映了属性的重要事件和变化。
 * Sobel算子、Scharr算子、Laplance算子、Canny算子都可以用来做边缘检测
 */

/**
 * Robert算子（算子就是卷积核）
 * [+1,0
 * 0,-1]
 */
void test1() {
    Mat src = imread("a.jpeg");
    Mat kernel = (Mat_<char>(2, 2) << 1, 0, 0, -1);
    Mat dst;
    filter2D(src, dst, src.depth(), kernel);
    imshow("dst", dst);
    waitKey(0);
}

/**
 * https://blog.csdn.net/shuiyixin/article/details/104484635
 * Sobel算子，常用来做边缘检测。有两个算子计算，就比较慢
 *
 * x方向明显：
 * [-1,0,1
 * -2,0,2
 * -1,0,1]
 *
 * y方向明显：
 * [-1,-2,-1
 * 0,0,0
 * 1,2,1]
 */
void test2() {
    Mat src = imread("card1.jpeg");

    //自己实现
//    Mat gray;
//    cvtColor(src, gray, COLOR_BGR2GRAY);

//    Mat kernel = (Mat_<char>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
//
//    Mat dst;
//    filter2D(gray, dst, src.depth(), kernel);
//    imshow("x", dst);
//
//    kernel = (Mat_<char>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
//    filter2D(dst, dst, src.depth(), kernel);
//    imshow("y", dst);

    //系统已经提供了这个方法，就不用上面那样写了
    //一般步骤：
    //1.降噪（高斯）
    Mat gaussian;
    GaussianBlur(src, gaussian, Size(3, 3), 0);
    //2.转灰度
    Mat gray;
    cvtColor(gaussian, gray, COLOR_BGR2GRAY);
    //3.Sobel(梯度)
    Mat sobel_x, sobel_y;
    //ddepth -1表示与gray.depth相同，这里要传比gray的depth高的值
    Sobel(gray, sobel_x, CV_32F, 1, 0, 3);
    Sobel(gray, sobel_y, CV_32F, 0, 1, 3);
    //4.取正值
    //sobel_x和sobel_y里可能有负数，合成时候要先取正值。取正值有两种方式：
    //（1）sobel = 开根号(sobel_x^2 + sobel_y^2)，这种会慢一点
    //（2）sobel = abs(sobel_x) + abs(sobel_y)，这种是近似的结果
    //我们这里就用第二种
    convertScaleAbs(sobel_x, sobel_x);
    convertScaleAbs(sobel_y, sobel_y);
    //5.合成
    Mat sobel;
    addWeighted(sobel_x, 0.5, sobel_y, 0.5, 0, sobel);

    imshow("sobel_x", sobel_x);
    imshow("sobel_y", sobel_y);
    imshow("sobel", sobel);

    waitKey(0);
}

/**
 * Scharr是Sobel的增强版，效果号，速度慢
 */
void test3() {
    Mat src = imread("card1.jpeg");

    //系统已经提供了这个方法，就不用上面那样写了
    //一般步骤：
    //1.降噪（高斯）
    Mat gaussian;
    GaussianBlur(src, gaussian, Size(3, 3), 0);
    //2.转灰度
    Mat gray;
    cvtColor(gaussian, gray, COLOR_BGR2GRAY);
    //3.Sobel(梯度)
    Mat scharr_x, scharr_y;
    //ddepth -1表示与gray.depth相同，这里要传比gray的depth高的值
    Scharr(gray, scharr_x, CV_16S, 1, 0, 3);
    Scharr(gray, scharr_y, CV_16S, 0, 1, 3);
    //4.取正值
    convertScaleAbs(scharr_x, scharr_x);
    convertScaleAbs(scharr_y, scharr_y);
    //5.合成
//    Mat scharr;
//    addWeighted(scharr_x, 0.5, scharr_y, 0.5, 0, scharr);

    //这里手写下第5步
    Mat scharr(gray.size(), gray.type());
    int width = gray.cols;
    int height = gray.rows;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int x_p = scharr_x.at<uchar>(i, j);
            int y_p = scharr_y.at<uchar>(i, j);
            int p = x_p + y_p;
            scharr.at<uchar>(i, j) = saturate_cast<uchar>(p);
        }
    }

    imshow("scharr_x", scharr_x);
    imshow("scharr_y", scharr_y);
    imshow("scharr", scharr);

    waitKey(0);
}

/**
 * https://blog.csdn.net/shuiyixin/article/details/104510390
 * 拉普拉斯算子
 * [0,-1,0
 * -1,4,-1
 * 0,-1,0]
 */
void test4() {
    Mat src = imread("a.jpeg");
    Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat dst;
    filter2D(gray, dst, src.depth(), kernel);
    imshow("dst", dst);
    waitKey(0);
}

/**
 * 拉普拉斯算子，用系统提供的方法，就一个算子，会比较快
 */
void test5() {
    Mat src = imread("card1.jpeg");

    //系统已经提供了这个方法，就不用上面那样写了
    //一般步骤：
    //1.降噪（高斯）
    Mat gaussian;
    GaussianBlur(src, gaussian, Size(3, 3), 0);
    //2.转灰度
    Mat gray;
    cvtColor(gaussian, gray, COLOR_BGR2GRAY);
    //3.拉普拉斯
    Mat lpls;
    Laplacian(gray, lpls, CV_16S, 5);
    //4.会有负数，要求绝对值
    convertScaleAbs(lpls, lpls);
    imshow("lpls", lpls);

    Mat binary;
    //二值化下，边界更明显点
    threshold(lpls, binary, 0, 255, THRESH_BINARY | cv::THRESH_OTSU);
    imshow("binary", binary);

    waitKey(0);
}

/**
 * 自定义模糊
 */
void test6() {
    Mat src = imread("a.jpeg");
    imshow("src", src);

    int size = 5;
    //有小数点，要用CV_32F,可以放float
    Mat kernel = Mat::ones(size, size, CV_32F) / (size * size);

    Mat dst;
    filter2D(src, dst, src.depth(), kernel);
    imshow("dst", dst);
    waitKey(0);
}

/**
 * https://blog.csdn.net/shuiyixin/article/details/104529465
 * Canny算子原理：
 * 1.用高斯模糊去噪声
 * 2.灰度转换
 * 3.计算梯度 Sobel/Scharr
 * 4.非最大信号抑制（只保留最大值），所以线比较细
 * 5.高低阈值输出二值图像（0，255）threshold1是低阈值，threshold2是高阈值。
 *   在threshold1~threshold2之间取255，否则取0
 *   取值时尽量1：2 或1：3，比如30-60、30-90、50-100、50-150
 */
void test7() {
    Mat src = imread("card1.jpeg");
    Mat dst;
    //最后一个参数：L2gradient是false，L1gradient是true
    //在test2()里说到要取正值有两种方法：
    //（1）sobel = 开根号(sobel_x^2 + sobel_y^2)，这个对应的是L1gradient
    //（2）sobel = abs(sobel_x) + abs(sobel_y)，这个对应的是L2gradient
    //最后一个参数传了false，就是用第二种方法取正值
    Canny(src, dst, 50, 150, 3, false);
    imshow("dst", dst);

    waitKey(0);
}

int main() {
    test7();
}
