#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

void test1() {
    //读图片转Mat
    Mat src = imread("a.jpeg");

    Mat dst;
    cvtColor(src, dst, COLOR_BGR2GRAY);

    //把Mat写入文件
    imwrite("gray.jpeg", dst);

    //显示图片
    imshow("test", dst);
    waitKey(0);
}

/**
 * 指针操作：https://blog.csdn.net/shuiyixin/article/details/106182880
 *
 * 操作指针换灰度图
 *
 * uchar 1个字节，8bit，范围：0~255
 *
 */
void test2() {
    Mat src = imread("a.jpeg");
    //rows:高，cols:宽
    //channels是3，表示一个像素点3颜色通道，包含三个信息 BGR
    printf("%d,%d,%d", src.rows, src.cols, src.channels());

    //彩色转灰度，转换公式 F = 0.11R + 0.59G + 0.3B
    for (int i = 0; i < src.rows; i++) {
        //读取当前行的首地址
        uchar *start_pixels = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++) {
            uchar b = start_pixels[0];
            uchar g = start_pixels[1];
            uchar r = start_pixels[2];

            //变灰
            //这个值不会超过255，所以可以用uchar来接
//            uchar gray = 0.11f * r + 0.59f * g + 0.30f * b;
//            start_pixels[0] = gray;
//            start_pixels[1] = gray;
//            start_pixels[2] = gray;

            //底片效果
//            start_pixels[0] = 255 - b;
//            start_pixels[1] = 255 - g;
//            start_pixels[2] = 255 - r;

            //增加饱和度
            //直接写1.2*b 这样三个加起来会超过255，要用saturate_cast
//            start_pixels[0] = saturate_cast<uchar>(1.2f * b);
//            start_pixels[1] = saturate_cast<uchar>(1.2f * g);
//            start_pixels[2] = saturate_cast<uchar>(1.2f * r);

            //黑白照
            uchar b_w = (b + g + r) / 3 > 155 ? 255 : 0;
            start_pixels[0] = b_w;
            start_pixels[1] = b_w;
            start_pixels[2] = b_w;


            //指针移到下一个像素点
            start_pixels += 3;
        }
    }

    imshow("test", src);
    waitKey(0);
}

int main() {
    test2();
}
