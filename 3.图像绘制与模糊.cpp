#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 绘制形状和文字
 */
void test1() {
    Mat src = imread("a.jpeg");
    //画线
    //LINE_AA：抗锯齿
//    line(src, Point(100, 100), Point(200, 200), Scalar(255, 0, 0), 2, LINE_4);

    //矩形
//    rectangle(src, Point(100, 100), Point(300, 300), Scalar(0, 0, 255), 2, LINE_8);

    //椭圆
//    ellipse(src, Point(src.cols / 2, src.rows / 2), Size(src.cols / 8, src.rows / 4), 180, 0, 360, Scalar(0, 255, 0),1);

    //圆
    //LINE_AA表示抗锯齿，https://blog.csdn.net/wenhao_ir/article/details/125535836
//    circle(src, Point(src.cols / 2, src.rows / 2), 100, Scalar(255, 255, 0), 3,LINE_AA);

    //填充多边形
//    Point pts[1][5];
//    pts[0][0] = Point(100, 100);
//    pts[0][1] = Point(100, 200);
//    pts[0][2] = Point(200, 300);
//    pts[0][3] = Point(200, 100);
//    pts[0][4] = Point(100, 100);
//
//    const Point *ptss[] = {pts[0]};
//    const int npts[] = {5};
//    fillPoly(src, ptss, npts, 1, Scalar(255, 255, 0), 8);

    //画字，基于左上角画
//    putText(src, "Hello OpenCV", Point(100, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 125, 0), 1, LINE_AA);

    //随机画，rng生成随机数
    RNG rng(time(NULL));
    //创建一张图，与src的宽高和颜色通道一致，所有点都是0
    Mat dst = Mat::zeros(src.size(), src.type());
    for (int i = 0; i < 1000; i++) {
        Point sp;
        sp.x = rng.uniform(0, dst.cols);
        sp.y = rng.uniform(0, dst.rows);

        Point ep;
        ep.x = rng.uniform(0, dst.cols);
        ep.y = rng.uniform(0, dst.rows);

        Scalar color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        line(dst, sp, ep, color);

    }

    imshow("src", dst);
    waitKey(0);
}

/**
 * 卷积的概念：https://blog.csdn.net/chaipp0607/article/details/72236892
 *
 * 自己实现卷积操作
 * 边缘锐化
 * [ 0,-1,0
 *  -1,5,-1
 *  0,-1,0]
 */
void test2() {
    Mat src = imread("a.jpeg");
    Mat dst = Mat::zeros(src.size(), src.type());
    int cols = (src.cols - 1) * src.channels();
    int rows = src.rows;
    int offsetX = src.channels();
    for (int row = 1; row < rows - 1; row++) {
        uchar *previous = src.ptr<uchar>(row - 1);
        uchar *current = src.ptr<uchar>(row);
        uchar *next = src.ptr<uchar>(row + 1);
        uchar *output = dst.ptr<uchar>(row);
        for (int col = offsetX; col < cols; col++) {
            output[col] = saturate_cast<uchar>(
                    5 * current[col] -
                    (current[col - offsetX] + current[col + offsetX] + previous[col] + next[col]));
        }
    }
    imshow("src", dst);
    waitKey(0);
}

/**
 * opencv提供了掩膜操作，掩膜的概念：https://blog.csdn.net/bitcarmanlee/article/details/79132017
 * 简单来说掩膜就是两幅图像之间进行的各种位运算操作。
 */
void test3() {
    Mat src = imread("a.jpeg");
    Mat dst;
    Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
            -1, 5, -1,
            0, -1, 0);

    /**
     * 第一个参数: 输入图像
       第二个参数: 输出图像，和输入图像具有相同的尺寸和通道数量
       第三个参数: 目标图像深度，输入值为-1时，目标图像和原图像深度保持一致。
       第四个参数: 卷积核，是一个矩阵
       第五个参数：内核的基准点(anchor)，其默认值为(-1,-1)说明位于kernel的中心位置。基准点即kernel中与进行处理的像素点重合的点。
       第五个参数: 在储存目标图像前可选的添加到像素的值，默认值为0
       第六个参数: 像素向外逼近的方法，默认值是BORDER_DEFAULT。
     */
    filter2D(src, dst, src.depth(), kernel);
    imshow("src", dst);
    waitKey(0);
}

/**
 * 模糊分为：
 * 1.均值模糊 blur。均值模糊里的卷积的kernel矩阵每个值都是1
 * 2.高斯模糊 GaussianBlur
 * 3.中值滤波，主要用来降噪，对所有考虑的点进行排序，取中间的那个值
 * 4.双边滤波，主要用来美容
 */
void test4() {
    Mat src = imread("a.jpeg");
    //均值模糊
    Mat dst;
    //size宽高只能是奇数
    blur(src, dst, Size(121, 121), Point(-1, -1));
    imshow("blur", dst);

    //高斯模糊
    Mat gaussian;
    //sigmaX、sigmaY:用于控制正态分布是否平缓
    //最后还有个参数borderType，控制 卷积操作时 边缘是如何填充的
    GaussianBlur(src, gaussian, Size(121, 121), 0);
    imshow("gaussian", gaussian);
    waitKey(0);
}

/**
 * 中值滤波，主要用来降噪，对所有考虑的点进行排序，取中间的那个值
 * https://blog.51cto.com/yang/4585251
 */
void test5() {
    //使用椒盐图片
    Mat src = imread("d.webp");
    Mat dst;
    medianBlur(src, dst, 7);
    imshow("dst", dst);
    waitKey(0);
}

/**
 * 双边滤波，主要用来美容。
 * 基于高斯，但能做到边缘的保护（使边缘明显一点）
 */
void test6() {
    Mat src = imread("e.jpeg");
    Mat dst;
    //这几个值是经验值
    bilateralFilter(src, dst, 15, 100, 5);
    imshow("dst", dst);

    //上一步做完会比较糊，再进行掩膜，锐化一下
    Mat final;
    Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
            -1, 5, -1,
            0, -1, 0);
    filter2D(dst, final, dst.depth(), kernel);
    imshow("final", final);

    waitKey(0);
}

/**
 * 卷积操作时，处理边缘类型
 */
void test7() {
    Mat src = imread("a.jpeg");
    Mat dst;
    //BORDER_DEFAULT：用周边边缘来填充
    //BORDER_WRAP：用对边填充
    //BORDER_REFLECT:用周边像素来填充
    //BORDER_CONSTANT:用常数来填充
    copyMakeBorder(src, dst, src.rows / 8, src.rows / 8, src.cols / 8, src.cols / 8, BORDER_CONSTANT,
                   Scalar(255, 0, 0));
    imshow("dst", dst);
    waitKey(0);
}

/**
 * https://www.bilibili.com/video/BV1Ay4y117HV/?p=271&spm_id_from=pageDriver&vd_source=cfa545ca14ba2d7782dd4c30ae22638e
 * https://www.guyuehome.com/35483
 *
 * 积分图的值 = 当前值(原图) + 上(积分图) + 左(积分图) - 左上(积分图)
 * 原图的值 = 当前值(积分图) - 左(积分图) - 右(积分图) + 左上(积分图)
 * 某一块的值(原图) = 右下(积分图) - 右上(积分图) - 左下(积分图) + 左上(积分图)
 *
 * 积分图的模糊算法
 * @param size 模糊直径
 */
int getBlockSum(Mat &sum_mat, int x0, int y0, int x1, int y1, int ch) {
    int lt = sum_mat.at<Vec3i>(y0, x0)[ch];
    int lb = sum_mat.at<Vec3i>(y1, x0)[ch];
    int rt = sum_mat.at<Vec3i>(y0, x1)[ch];
    int rb = sum_mat.at<Vec3i>(y1, x1)[ch];

    int sum = rb - rt - lb + lt;

    return sum;
}

float getBlockSqSum(Mat &sqsum_mat, int x0, int y0, int x1, int y1, int ch) {
    float lt = sqsum_mat.at<Vec3f>(y0, x0)[ch];
    float lb = sqsum_mat.at<Vec3f>(y1, x0)[ch];
    float rt = sqsum_mat.at<Vec3f>(y0, x1)[ch];
    float rb = sqsum_mat.at<Vec3f>(y1, x1)[ch];

    float sum = rb - rt - lb + lt;

    return sum;
}

void myBlur(Mat &src, Mat &dst, int size, int sigma) {
    //边界要填充
    Mat mat;
    int radius = size / 2;
    copyMakeBorder(src, mat, radius, radius, radius, radius, BORDER_DEFAULT);

    Mat sum_mat, sqsum_mat;
    /**
     * image：输入W×H源图像，8bit字符型，或32bit、64bit浮点型矩阵
     * sum：输出(W+1)×(H +1)积分图像，32bit整型或32bit、64bit浮点型矩阵
     * sqsum：输出(W+1)×(H +1)平方积分图像，双精度浮点型矩阵。
     * tilted：输出旋转45°的(W+1)×(H +1)积分图像，数据类型同sum
     * sdepth：积分图像sum或titled的位深度：CV_32S、CV_32F或CV_64F
     */
    integral(mat, sum_mat, sqsum_mat, CV_32S, CV_32F);

    dst.create(src.size(), src.type());
    int imgH = src.rows;
    int imgW = src.cols;
    //求四个点，左上，左下，右上，右下
    int x0, y0, x1, y1;
    int area = size * size;
    int channels = src.channels();
    for (int row = 0; row < imgH; row++) {
        y0 = row;
        y1 = y0 + size;
        for (int col = 0; col < imgW; col++) {
            x0 = col;
            x1 = x0 + size;
            for (int i = 0; i < channels; i++) {
                //边缘保留：https://www.bilibili.com/video/BV1Ay4y117HV/?p=274&spm_id_from=pageDriver&vd_source=cfa545ca14ba2d7782dd4c30ae22638e
                int sum = getBlockSum(sum_mat, x0, y0, x1, y1, i);
                float sq_sum = getBlockSqSum(sqsum_mat, x0, y0, x1, y1, i);
                float diff_sq = (sq_sum - (sum * sum) / area) / area;
                float k = diff_sq / (diff_sq + sigma);
                int pixels = src.at<Vec3b>(row, col)[i];
                pixels = (1 - k) * (sum / area) + k * pixels;
                dst.at<Vec3b>(row, col)[i] = pixels;
            }
        }
    }
}

/**
 * 找皮肤区域
 * https://www.bilibili.com/video/BV1Ay4y117HV/?p=274&spm_id_from=pageDriver&vd_source=cfa545ca14ba2d7782dd4c30ae22638e
 */
void skinDetect(const Mat &src, Mat &skinMask) {
    skinMask.create(src.size(), CV_8UC1);
    int rows = src.rows;
    int cols = src.cols;

    Mat ycrcb;
    cvtColor(src, ycrcb, COLOR_BGR2YCrCb);

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Vec3b pixels = ycrcb.at<Vec3b>(row, col);
            uchar y = pixels[0];
            uchar cr = pixels[1];
            uchar cb = pixels[2];
            //在这个范围是黄种人皮肤
            if (y > 80 && 85 < cb && cb < 135 && 135 < cr && cr < 180) {
                skinMask.at<uchar>(row, col) = 255;
            } else {
                skinMask.at<uchar>(row, col) = 0;
            }
        }
    }
}

/**
 * 融合，如果是皮肤区域，用blur；如果不是，用src
 */
void fuseSkin1(const Mat &src, const Mat &blurMat, Mat &dst, const Mat &mask) {
    dst.create(src.size(), src.type());
    int rows = src.rows;
    int cols = src.cols;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            uchar mask_pixels = mask.at<uchar>(row, col);
            //人脸位置
            if (mask_pixels == 255) {
                dst.at<Vec3b>(row, col) = blurMat.at<Vec3b>(row, col);
            } else {
                dst.at<Vec3b>(row, col) = src.at<Vec3b>(row, col);
            }
        }
    }
}

void fuseSkin2(const Mat &src, const Mat &blurMat, Mat &dst, const Mat &mask) {
    dst.create(src.size(), src.type());
    //对mask进行一次模糊，柔和一点
    GaussianBlur(mask, mask, Size(3, 3), 0, 0);

    Mat mask_f;
    mask.convertTo(mask_f, CV_32F);
    //归一到0~1之间
    normalize(mask_f, mask_f, 1.0, 0.0, NORM_MINMAX);

    int rows = src.rows;
    int cols = src.cols;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            uchar b1 = src.at<Vec3b>(row, col)[0];
            uchar g1 = src.at<Vec3b>(row, col)[1];
            uchar r1 = src.at<Vec3b>(row, col)[2];

            uchar b2 = blurMat.at<Vec3b>(row, col)[0];
            uchar g2 = blurMat.at<Vec3b>(row, col)[1];
            uchar r2 = blurMat.at<Vec3b>(row, col)[2];

            //如果是皮肤区域，k会趋近于1。
            float k = mask_f.at<float>(row, col);

            dst.at<Vec3b>(row, col)[0] = (1 - k) * b1 + k * b2;
            dst.at<Vec3b>(row, col)[1] = (1 - k) * g1 + k * g2;
            dst.at<Vec3b>(row, col)[2] = (1 - k) * r1 + k * r2;
        }
    }
}

void test8() {
    Mat src = imread("face.jpeg");
    imshow("src", src);

    //模糊
    Mat dst;
    int size = 15;
    myBlur(src, dst, size, size * size);
    imshow("dst", dst);

    //皮肤检测
    Mat skinMask;
    skinDetect(src, skinMask);
//    imshow("skin", skinMask);

    //融合
    Mat fuseDst;
    fuseSkin2(src, dst, fuseDst, skinMask);

    //边缘的提升
    Mat cannyMask;
    Canny(src, cannyMask, 150, 300, 3, false);
    //&的运算，canny得到是值要么是0要么是255
    bitwise_and(src, src, fuseDst, cannyMask);

    //稍微提升亮度
    add(fuseDst, Scalar(10, 10, 10), fuseDst);

    imshow("fuse", fuseDst);


    waitKey(0);
}

int main() {
    test8();
}