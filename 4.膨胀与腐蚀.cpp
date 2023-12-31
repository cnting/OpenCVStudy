#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;
/**
 * https://zhuanlan.zhihu.com/p/110330329
 * 膨胀与腐蚀，一般用来处理黑白图片
 * 膨胀用来处理缺陷，腐蚀用来处理毛刺
 */

int element_size = 1;
int max_size = 21;
Mat src;

void trackbarCallback(int pos, void *userdata) {
    //创建一个 kernel
    int size = element_size * 2 + 1;
    //最后一个参数anchor(-1，-1)表示卷积核覆盖的矩阵的中心点。请注意，只有十字形元素的形状取决于锚定位置。在其他情况下，锚只是调节形态学操作结果的移动量。
    Mat kernel = getStructuringElement(MORPH_RECT, Size(size, size));
    Mat dst;
    erode(src, dst, kernel);
    imshow("output image", dst);
}

/**
 * 腐蚀，取最小值
 */
void test1() {
    src = imread("a.jpeg");

    //创建一个window
    namedWindow("output image");
    //动态控制
    createTrackbar("Trackbar", "output image", &element_size, max_size, trackbarCallback);

    waitKey(0);
}

void trackbarCallback1(int pos, void *userdata) {
    //创建一个 kernel
    int size = element_size * 2 + 1;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(size, size));
    Mat dst;
    dilate(src, dst, kernel);
    imshow("output image", dst);
}

/**
 * 膨胀，取最大值
 */
void test2() {
    src = imread("a.jpeg");

    //创建一个window
    namedWindow("output image");
    //动态控制
    createTrackbar("Trackbar", "output image", &element_size, max_size, trackbarCallback1);

    waitKey(0);
}
/**
 * https://blog.csdn.net/shuiyixin/article/details/104369163
 */
void test3() {
    Mat s = imread("f.webp");
    imshow("原始", s);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat dst;
    //开操作：先腐蚀后膨胀，可以去掉小对象
//    morphologyEx(s, dst, MORPH_OPEN, kernel);

    //闭操作：先膨胀后腐蚀，可以填充小对象
//    morphologyEx(s, dst, MORPH_CLOSE, kernel);

    //梯度：膨胀-腐蚀，得到轮廓
//    morphologyEx(s, dst, MORPH_GRADIENT, kernel);

    //顶帽：原图像与开操作图像之间的差值图像
//    morphologyEx(s, dst, MORPH_TOPHAT, kernel);

    //黑帽：闭操作图像与原图像之间的差值图像
    morphologyEx(s, dst, MORPH_BLACKHAT, kernel);


    imshow("dst", dst);

    waitKey(0);
}

/**
 * 验证码识别 黑白
 */
void test4() {
    Mat s = imread("code.webp");

    Mat kernel = getStructuringElement(MORPH_RECT, Size(9, 9));
    Mat dst;
    morphologyEx(s, dst, MORPH_CLOSE, kernel);
    imshow("dst", dst);

    waitKey(0);
}

/**
 * 验证码识别 彩色
 */
void test5() {
    Mat s = imread("code1.jpeg");

    //1.把彩色变黑白
    Mat gray;
    cvtColor(s, gray, COLOR_BGR2GRAY);
    //二值化
    Mat binary;
    adaptiveThreshold(~gray, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, 0);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat dst;
    erode(binary, dst, kernel);
    dilate(dst, dst, kernel);
    imshow("dst", dst);

    waitKey(0);
}

/**
 * https://www.bilibili.com/video/BV1Ay4y117HV/?p=234&spm_id_from=pageDriver&vd_source=cfa545ca14ba2d7782dd4c30ae22638e
 * 提取水平和垂直线
 * 思路：kernel可以是任意形状，可以把它定义为一根线
 * 比如说要找水平的线，把kernel定义为一根横的线，然后用膨胀算法，遇到竖的线会变成白色（因为周边是白色的），遇到横的线还是原来的颜色
 */
void test6() {
    Mat s = imread("line.jpg");
    //先变黑白
    Mat gray;
    cvtColor(s, gray, COLOR_BGR2GRAY);
    Mat binary;
    adaptiveThreshold(~gray, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 45, 0);
    imshow("binary", binary);

    //取水平
//    Mat dst;
//    Mat kernel = getStructuringElement(MORPH_RECT, Size(s.cols/16, 1));
//    erode(binary,dst,kernel);
//    dilate(dst,dst,kernel);
//    imshow("dst",dst);

    //取垂直
    Mat dst;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(1, s.rows / 16));
    erode(binary, dst, kernel);
    dilate(dst, dst, kernel);
    imshow("dst", dst);

    waitKey(0);
}

int main() {
    test6();
}