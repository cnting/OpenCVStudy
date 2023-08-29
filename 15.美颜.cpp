#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

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

void test1() {
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
    test1();
}