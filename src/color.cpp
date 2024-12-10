#include "color.h"
#include <iostream>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "color.h"
#include "color_opencv.h"

using namespace cv;
using namespace std;
#if COLOR_THRESHOLD_DEBUG
// 蓝色HSV阈值
int BLUE_H_L = 100, BLUE_H_H = 138;
int BLUE_S_L = 133, BLUE_S_H = 255;
int BLUE_V_L = 40, BLUE_V_H = 255;
// 绿色HSV阈值
int GREEN_H_L = 52, GREEN_H_H = 92;
int GREEN_S_L = 73, GREEN_S_H = 245;
int GREEN_V_L = 73, GREEN_V_H = 255;
// 红色HSV阈值
int RED_H_L = 160, RED_H_H = 180;
int RED_S_L = 100, RED_S_H = 255;
int RED_V_L = 100, RED_V_H = 255;
// 红色2HSV阈值
int RED2_H_L = 0, RED2_H_H = 10;
int RED2_S_L = 100, RED2_S_H = 255;
int RED2_V_L = 100, RED2_V_H = 255;

// 蓝色环HSV阈值
int BLUE1_H_L = 100, BLUE1_H_H = 138;
int BLUE1_S_L = 44, BLUE1_S_H = 240;
int BLUE1_V_L = 13, BLUE1_V_H = 250;
// 绿色环HSV阈值
int GREEN1_H_L = 59, GREEN1_H_H = 80;
int GREEN1_S_L = 23, GREEN1_S_H = 255;
int GREEN1_V_L = 0, GREEN1_V_H = 255;
// 红色环HSV阈值
int RED1_H_L = 156, RED1_H_H = 180;
int RED1_S_L = 0, RED1_S_H = 255;
int RED1_V_L = 34, RED1_V_H = 255;

Mat frame;

Mat green_img;
Mat red_img;
Mat blue_img;

// 创建掩码（mask）
Mat red_mask;
Mat red2_mask;
Mat green_mask;
Mat blue_mask;

// 自适应阈值
Mat red_adap;

Mat red1_img;
Mat green1_img;
Mat blue1_img;
Mat dst2;
Mat hsv;
cuda::GpuMat imgGpu;
vector<cuda::GpuMat> gpuMats;
cuda::GpuMat mat;
vector<Mat> hsvSplit; // 创建向量容器，存放HSV的三通道数据
cv::VideoCapture cap(4);

int detect_color;
int detect_centerX;
int detect_centerY;

#define BLUE_LOWERB() Scalar(BLUE_H_L, BLUE_S_L, BLUE_V_L)
#define BLUE_UPPER() Scalar(BLUE_H_H, BLUE_S_H, BLUE_V_H)
#define GREEN_LOWERB() Scalar(GREEN_H_L, GREEN_S_L, GREEN_V_L)
#define GREEN_UPPER() Scalar(GREEN_H_H, GREEN_S_H, GREEN_V_H)
#define RED_LOWERB() Scalar(RED_H_L, RED_S_L, RED_V_L)
#define RED_UPPER() Scalar(RED_H_H, RED_S_H, RED_V_H)
#define RED2_LOWERB() Scalar(RED2_H_L, RED2_S_L, RED2_V_L)
#define RED2_UPPER() Scalar(RED2_H_H, RED2_S_H, RED2_V_H)

#define BLUE1_LOWERB() Scalar(BLUE1_H_L, BLUE1_S_L, BLUE1_V_L)
#define BLUE1_UPPER() Scalar(BLUE1_H_H, BLUE1_S_H, BLUE1_V_H)
#define GREEN1_LOWERB() Scalar(GREEN1_H_L, GREEN1_S_L, GREEN1_V_L)
#define GREEN1_UPPER() Scalar(GREEN1_H_H, GREEN1_S_H, GREEN1_V_H)
#define RED1_LOWERB() Scalar(RED1_H_L, RED1_S_L, RED1_V_L)
#define RED1_UPPER() Scalar(RED1_H_H, RED1_S_H, RED1_V_H)
#else
#define BLUE_LOWERB() Scalar(100, 44, 45)
#define BLUE_UPPER() Scalar(138, 240, 250)
#define GREEN_LOWERB() Scalar(0, 80, 0)
#define GREEN_UPPER() Scalar(0, 200, 0)
#define RED_LOWERB() Scalar(0, 0, 80)
#define RED_UPPER() Scalar(0, 0, 200)
#endif

// private:
//     cv::Point centre;
//     cv::Mat grayimage;
//     cv::Mat out_image;
//     cv::Rect r;

void color::color_recognite(cv::Mat image, cv::Scalar Low, cv::Scalar High)
{
	cv::Mat input_image;
	image.copyTo(input_image);
	/*
		是一个向量，并且是一个双重向量，
		向量内每个元素保存了一组由连续的Point点构成的点的集合的向量，
		每一组Point点集就是一个轮廓。
		有多少轮廓，向量contours就有多少元素。
	*/
	std::vector<std::vector<cv::Point>> g_vContours;

	/*
		Vec4i是Vec<int,4>的别名，定义了一个“向量内每一个元素包含了4个int型变量”的向量。
		所以从定义上看，hierarchy也是一个向量，向量内每个元素保存了一个包含4个int整型的数组。
		向量hiararchy内的元素和轮廓向量contours内的元素是一一对应的，向量的容量相同。
		hierarchy向量内每一个元素的4个int型变量——hierarchy[i][0] ~hierarchy[i][3]，分别表示第
		i个轮廓的后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号。如果当前轮廓没有对应的后一个
		轮廓、前一个轮廓、父轮廓或内嵌轮廓的话，则hierarchy[i][0] ~hierarchy[i][3]的相应位被设置为
		默认值-1。
	*/
	std::vector<cv::Vec4i> g_vHierarchy;
	double maxarea = 0;
	int maxAreaIdx = 0;
	cv::Mat g_grayImage, hsv, g_cannyMat_output;

	cvtColor(input_image, hsv, cv::COLOR_BGR2HSV);
	blur(hsv, hsv, cv::Size(5, 5));
	inRange(hsv, Low, High, g_grayImage); // 二值化识别颜色

	// 腐蚀操作
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8, 8));
	erode(g_grayImage, g_grayImage, element);

	// 寻找轮廓
	findContours(g_grayImage, g_vContours, g_vHierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// 假设contours是用findContours函数所得出的边缘点集
	cv::RotatedRect box;

	double areaThreshold = 2000;

	for (int index = 0; index < g_vContours.size(); index++)
	{
		double tmparea = fabs(contourArea(g_vContours[index]));

		// 只处理面积大于等于阈值的颜色块
		if (tmparea >= areaThreshold)
		{
			// 计算颜色块的中心坐标
			r = boundingRect(g_vContours[index]);
			center.x = r.x + r.width / 2;
			center.y = (r.y + r.height / 2);

			// 输出坐标
			std::cout << "Center X: " << center.x << ", Center Y: " << center.y << std::endl;

			// 在图像上绘制颜色块
			rectangle(input_image, r, cv::Scalar(0, 255, 0));
			circle(input_image, center, 2, cv::Scalar(255, 0, 0), 2);
		}
	}

	grayimage = g_grayImage;
	out_image = input_image;
}

cv::Point color::show_center()
{
	cv::Point centerPoint(center.x, center.y);
	return centerPoint;
}

void color::show_grayimage(char const *WINDOWNAME)
{
	imshow(WINDOWNAME, grayimage);
	cv::waitKey(1);
}

void color::show_image(char const *WINDOWNAME)
{
	imshow(WINDOWNAME, out_image);
	cv::waitKey(1);
}

void adaptiveThreshold_Color(const cv::Mat &color_img, cv::Mat &adaptive_img)
{
	cv::adaptiveThreshold(color_img, adaptive_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
}

// 回调函数
void onBlue_Hsv(int, void *)
{
	/*
		参数1：输入要处理的图像，可以为单通道或多通道。
		参数2：包含下边界的数组或标量。
		参数3：包含上边界数组或标量。
		参数4：输出图像，与输入图像src 尺寸相同且为CV_8U 类型。
		请注意：该函数输出的dst是一幅二值化之后的图像。
	*/
	inRange(hsv, BLUE_LOWERB(), BLUE_UPPER(), blue_img); // 二值化处理
	// adaptiveThreshold_Color(blue_img, blue_mask);
}
void onGreen_Hsv(int, void *)
{
	inRange(hsv, GREEN_LOWERB(), GREEN_UPPER(), green_img); // 二值化处理
	// adaptiveThreshold_Color(green_img, green_mask);
}
void onRed_Hsv(int, void *)
{
	// inRange(hsv, RED_LOWERB(), RED_UPPER(), red_mask); // 二值化处理
	inRange(hsv, RED2_LOWERB(), RED2_UPPER(), red2_mask); // 二值化处理
	// cv::bitwise_or(red_mask, red2_mask, red_img);
	// adaptiveThreshold_Color(red2_mask, red_adap);
}
void onRed1_Hsv(int, void *)
{
	inRange(hsv, RED1_LOWERB(), RED1_UPPER(), red1_img); // 二值化处理
}
void onGreen1_Hsv(int, void *)
{
	inRange(hsv, GREEN1_LOWERB(), GREEN1_UPPER(), green1_img); // 二值化处理
}
void onBlue1_Hsv(int, void *)
{
	inRange(hsv, BLUE1_LOWERB(), BLUE1_UPPER(), blue1_img); // 二值化处理
}

void color::color_detect()
{

	// cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
	// cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);

	// cuda::GpuMat imgGpu;
	// vector<cuda::GpuMat> gpuMats;
	// cuda::GpuMat mat;

	// 创建窗口
	namedWindow("imgOriginal", 0);
	namedWindow("COLOR_THRESHOLD_RED", 0);
	namedWindow("COLOR_THRESHOLD_GREEN", 0);
	namedWindow("COLOR_THRESHOLD_BLUE", 0);
	// namedWindow("COLOR_THRESHOLD_RED1", 0);
	// namedWindow("COLOR_THRESHOLD_GREEN1", 0);
	// namedWindow("COLOR_THRESHOLD_BLUE1", 0);

	resizeWindow("imgOriginal", 500, 500);
	resizeWindow("COLOR_THRESHOLD_RED", 500, 500);
	resizeWindow("COLOR_THRESHOLD_GREEN", 500, 500);
	resizeWindow("COLOR_THRESHOLD_BLUE", 500, 500);
	// resizeWindow("COLOR_THRESHOLD_RED1", 500, 500);
	// resizeWindow("COLOR_THRESHOLD_GREEN1", 500, 500);
	// resizeWindow("COLOR_THRESHOLD_BLUE1", 500, 500);

	if (!cap.isOpened()) // if not success, exit program
	{
		std::cout << "Cannot open the video" << std::endl;
	}
	bool bSuccess = cap.read(frame); // read a new frame from video
	if (!bSuccess)					 // if not success, break loop
	{
		std::cout << "Cannot read a frame from video stream" << std::endl;
	}

	imshow("imgOriginal", frame);

	// cvCvtColor(...),是Opencv里的颜色空间转换函数，可以实现RGB颜色向HSV,HSI等颜色空间的转换
	// CV_BGR2HSV将图片从RGB空间转换为HSV空间。
	cvtColor(frame, hsv, COLOR_BGR2HSV);

	/*
		形参一、trackbarname：滑动空间的名称；
		形参二、winname：滑动空间用于依附的图像窗口的名称；
		形参三、value：初始化阈值；
		形参四、count：滑动控件的刻度范围；
		形参五、TrackbarCallback是回调函数，其定义如下：
			typedef void (CV_CDECL *TrackbarCallback)(int pos, void* userdata);
	*/
	// 红物料
	createTrackbar("LH", "COLOR_THRESHOLD_RED", &RED_H_L, 180, onRed_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_RED", &RED_H_H, 180, onRed_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_RED", &RED_S_L, 255, onRed_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_RED", &RED_S_H, 255, onRed_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_RED", &RED_V_L, 255, onRed_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_RED", &RED_V_H, 255, onRed_Hsv);

	createTrackbar("LH", "COLOR_THRESHOLD_RED2", &RED2_H_L, 180, onRed_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_RED2", &RED2_H_H, 180, onRed_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_RED2", &RED2_S_L, 255, onRed_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_RED2", &RED2_S_H, 255, onRed_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_RED2", &RED2_V_L, 255, onRed_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_RED2", &RED2_V_H, 255, onRed_Hsv);
	// 绿物料
	createTrackbar("LH", "COLOR_THRESHOLD_GREEN", &GREEN_H_L, 180, onGreen_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_GREEN", &GREEN_H_H, 180, onGreen_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_GREEN", &GREEN_S_L, 255, onGreen_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_GREEN", &GREEN_S_H, 255, onGreen_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_GREEN", &GREEN_V_L, 255, onGreen_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_GREEN", &GREEN_V_H, 255, onGreen_Hsv);
	// 蓝物料
	createTrackbar("LH", "COLOR_THRESHOLD_BLUE", &BLUE_H_L, 180, onBlue_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_BLUE", &BLUE_H_H, 180, onBlue_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_BLUE", &BLUE_S_L, 255, onBlue_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_BLUE", &BLUE_S_H, 255, onBlue_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_BLUE", &BLUE_V_L, 255, onBlue_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_BLUE", &BLUE_V_H, 255, onBlue_Hsv);
	// 红色环
	createTrackbar("LH", "COLOR_THRESHOLD_RED1", &RED1_H_L, 180, onRed1_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_RED1", &RED1_H_H, 180, onRed1_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_RED1", &RED1_S_L, 255, onRed1_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_RED1", &RED1_S_H, 255, onRed1_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_RED1", &RED1_V_L, 255, onRed1_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_RED1", &RED1_V_H, 255, onRed1_Hsv);
	// 绿色环
	createTrackbar("LH", "COLOR_THRESHOLD_GREEN1", &GREEN1_H_L, 180, onGreen1_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_GREEN1", &GREEN1_H_H, 180, onGreen1_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_GREEN1", &GREEN1_S_L, 255, onGreen1_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_GREEN1", &GREEN1_S_H, 255, onGreen1_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_GREEN1", &GREEN1_V_L, 255, onGreen1_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_GREEN1", &GREEN1_V_H, 255, onGreen1_Hsv);
	// 蓝色环
	createTrackbar("LH", "COLOR_THRESHOLD_BLUE1", &BLUE1_H_L, 180, onBlue1_Hsv);
	createTrackbar("HH", "COLOR_THRESHOLD_BLUE1", &BLUE1_H_H, 180, onBlue1_Hsv);
	createTrackbar("LS", "COLOR_THRESHOLD_BLUE1", &BLUE1_S_L, 255, onBlue1_Hsv);
	createTrackbar("HS", "COLOR_THRESHOLD_BLUE1", &BLUE1_S_H, 255, onBlue1_Hsv);
	createTrackbar("LV", "COLOR_THRESHOLD_BLUE1", &BLUE1_V_L, 255, onBlue1_Hsv);
	createTrackbar("HV", "COLOR_THRESHOLD_BLUE1", &BLUE1_V_H, 255, onBlue1_Hsv);

	// vector<Mat> hsvSplit; //创建向量容器，存放HSV的三通道数据
}

int min_r=60;//色环最短半径
int max_r=75;//色环最大半径
void color::color_ring(cv::Mat in_image)
{
	// 灰度图像
        cv::Mat gray;
        cv::cvtColor(in_image, gray, cv::COLOR_BGR2GRAY);

        //中值滤波
        cv::Mat median_img;
        cv::medianBlur(gray,median_img,7);

        // 高斯模糊
        // cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);

        //Canny边缘检测
        cv::Mat dst;
        cv::Canny(median_img,dst,50,100);

        // 霍夫变换检测圆
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(dst, circles, cv::HOUGH_GRADIENT, 13, 1000, 150, 1.5, min_r, max_r);
        //霍夫圆检测，8位输入，三维数组输出，霍夫梯度，尺度，半径最短间距，canny低阈值，圆心累加阈值，最短半径，最长半径

 int totalX = 0, totalY = 0;

        // 绘制检测到的圆并输出坐标
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // // 绘制圆的中心
            // cv::circle(dst, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
            // // 绘制圆的轮廓
            cv::circle(dst, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);
            // 输出圆心坐标
            // std::cout << "center: (" << center.x << ", " << center.y << ")" << std::endl;
            // 累加圆心坐标
            totalX += center.x;
            totalY += center.y;
        }

        // 计算平均圆心坐标
        if (!circles.empty()) {
            int avgX = totalX / circles.size();
            int avgY = totalY / circles.size();
            std::cout << "平均圆心坐标: (" << avgX << ", " << avgY << ")" << std::endl;
            // 在图像上绘制平均圆心与轮廓
            cv::circle(dst, cv::Point(avgX, avgY), 5, cv::Scalar(255, 0, 0), -1, 8, 0);
         
        }

        // 显示结果
        cv::imshow("frame", frame);//原图
        cv::imshow("gray",median_img);//中值滤波
        cv::imshow("dst", dst);//Canny
}

void color::ma_color()
{

	auto start = getTickCount();

	bool bSuccess = cap.read(frame); // read a new frame from video
	if (!bSuccess)					 // if not success, break loop
	{
		std::cout << "Cannot read a frame from video stream" << std::endl;
	}

	imgGpu.upload(frame);

	cvtColor(frame, hsv, COLOR_BGR2HSV);
	split(hsv, hsvSplit);					// 分类原图像的HSV三通道
	equalizeHist(hsvSplit[2], hsvSplit[2]); // 对HSV的亮度通道进行直方图均衡
	merge(hsvSplit, hsv);					// 合并三种通道

	color gxcolor;
	// onRed_Hsv(0, 0);
	// onGreen_Hsv(0, 0);
	// onBlue_Hsv(0, 0);
	// onRed1_Hsv(0,0);
	// onGreen1_Hsv(0,0);
	// onBlue1_Hsv(0,0);

	switch (detect_color)
	{
	case 0:
	{
		onRed_Hsv(0, 0);
		onGreen_Hsv(0, 0);
		onBlue_Hsv(0, 0);
		gxcolor.color_recognite(frame, RED_LOWERB(), RED_UPPER());
		gxcolor.color_recognite(frame, GREEN_LOWERB(), GREEN_UPPER());
		gxcolor.color_recognite(frame, BLUE_LOWERB(), BLUE_UPPER());
		cv::Point centerPoint = gxcolor.show_center();
		detect_centerX = centerPoint.x;
		detect_centerY = centerPoint.y;
		std::cout << "(" << detect_centerX << "," << detect_centerY << ")" << std::endl;

		cv::circle(blue_mask, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		cv::circle(red_mask, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		cv::circle(green_mask, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);

		imshow("COLOR_THRESHOLD_RED", red_mask);
		imshow("COLOR_THRESHOLD_GREEN", green_mask);
		imshow("COLOR_THRESHOLD_BLUE", blue_mask);

		break;
	}
	case 1:
	{
		onRed_Hsv(0, 0);
		gxcolor.color_recognite(frame, RED_LOWERB(), RED_UPPER());
		cv::Point centerPoint = gxcolor.show_center();
		int detect_centerX = centerPoint.x;
		int detect_centerY = centerPoint.y;
		std::cout << "(" << detect_centerX << "," << detect_centerY << ")" << std::endl;
		cv::circle(red2_mask, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);

		// cv::circle(red_img, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		// imshow("red_img", red_img);
		imshow("COLOR_THRESHOLD_RED", red_adap);
		break;
	}
	case 2:
	{
		onGreen_Hsv(0, 0);
		gxcolor.color_recognite(frame, GREEN_LOWERB(), GREEN_UPPER());
		cv::Point centerPoint = gxcolor.show_center();
		int detect_centerX = centerPoint.x;
		int detect_centerY = centerPoint.y;
		std::cout << "(" << detect_centerX << "," << detect_centerY << ")" << std::endl;
		// cv::circle(green_img, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		// imshow("COLOR_THRESHOLD_GREEN", green_img);
		cv::circle(green_mask, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		imshow("COLOR_THRESHOLD_GREEN", green_mask);
		break;
	}
	case 3:
	{
		onBlue_Hsv(0, 0);
		gxcolor.color_recognite(frame, BLUE_LOWERB(), BLUE_UPPER());
		cv::Point centerPoint = gxcolor.show_center();
		int detect_centerX = centerPoint.x;
		int detect_centerY = centerPoint.y;
		std::cout << "(" << detect_centerX << "," << detect_centerY << ")" << std::endl;
		// cv::circle(blue_img, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		// imshow("COLOR_THRESHOLD_BLUE", blue_img);
		cv::circle(blue_mask, Point(detect_centerX, detect_centerY), 20, cv::Scalar(0, 0, 255), 2, 8);
		imshow("COLOR_THRESHOLD_BLUE", blue_mask);
		break;
	}
	case 4:
	{
		color_ring(frame);
	}
	}

	
	imgGpu.download(frame);

	auto end = getTickCount();
	auto totalTime = (end - start) / getTickFrequency();
	auto fps = 1 / totalTime;

	putText(frame, "FPS: " + to_string(int(fps)), Point(50, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2, false);
	imshow("Image", frame);

	imshow("imgOriginal", hsv);

	waitKey(30);
}
