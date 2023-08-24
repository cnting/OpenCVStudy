#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * https://blog.csdn.net/shuiyixin/article/details/104423534
 *
 * 图像二值化：也叫阈值操作，就是将图像上的像素点设置为0或255，也就是将整个图像呈现出明显的黑白效果
 * 通过图像二值化，可以更好的分析物体的形状和轮廓。
 * 一般采用 阈值法 进行二值化
 *
 * 全局阈值：对整个图像中的每个像素都选用相同的阈值
 * 局部阈值：假定图像在一定区域内受到的光照比较接近。它用一个滑窗扫描图像，并取滑窗中心点亮度与滑窗内其他区域（称为邻域, neighborhood area）的亮度进行比较。如果中心点亮度高于邻域亮度，则将中心点标记为白色，否则标记为黑色。
 *
 * 局部阈值对白纸黑字的处理非常有效，OCR和二维码扫描算法中，很多都用了局部阈值操作
 */
void test1() {
    Mat src = imread("a.jpeg");

    //一般要进行二值化前都先进行灰度处理，速度更快
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat dst;
    //如果当前像素点超过100(thresh)，当前像素点取255，否则取最小值0
//    threshold(gray, dst, 100, 255, THRESH_BINARY);

    //如果当前像素点超过100(thresh)，当前像素点取0，否则取最大值255
//    threshold(gray, dst, 100, 255, THRESH_BINARY_INV);

    //如果当前像素点超过100(thresh)，当前像素点取thresh，否则取原来值
//    threshold(gray, dst, 100, 255, THRESH_TRUNC);

    //如果当前像素点超过100(thresh)，当前像素点取原来的值，否则取0
//    threshold(gray, dst, 100, 255, THRESH_TOZERO);

    //如果当前像素点超过100(thresh)，当前像素点取0，否则取原来的值
//    threshold(gray, dst, 100, 255, THRESH_TOZERO_INV);

    //自动阈值，取每个像素点（整个图像）计算一个thresh，会忽略输入的阈值
//    threshold(gray, dst, 100, 255, THRESH_OTSU|THRESH_BINARY);

    //自动阈值
    threshold(gray, dst, 100, 255, THRESH_TRIANGLE|THRESH_BINARY);

    //对局部区域计算一个 thresh
    //ADAPTIVE_THRESH_MEAN_C 计算出领域的平均值再减去第七个参数double C的值
    //ADAPTIVE_THRESH_GAUSSIAN_C 计算出领域的高斯均值再减去第七个参数double C的值
//    adaptiveThreshold(gray, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, 0);

    imshow("dst", dst);
    waitKey(0);
}

int main() {
    test1();
}
