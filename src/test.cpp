#include"标头.h"
using namespace cv;
using namespace std;

bool  ChipDetect_algo::chipNG_algo(Rect detectroi,Mat binaryimg,int i)
{
	Mat stats, centroids;
	vector<float>areas;
	Mat labels = Mat::zeros(binaryimg.size(), CV_32S);
	Mat temp1;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	morphologyEx(binaryimg, temp1, MORPH_OPEN, kernel);
	int labalsnum = connectedComponentsWithStats(~temp1(detectroi), labels, stats, centroids, 8, CV_32S, CCL_DEFAULT);
//	imshow(to_string(i), ~temp1(detectroi));
	int num = 0;
	for (int j = 0; j < labalsnum; j++)
	{

		if (stats.at<int>(j, CC_STAT_AREA) > 16)
		{
			areas.push_back((float)stats.at<int>(j, CC_STAT_AREA));
			num++;
			//cout << stats.at<int>(j, CC_STAT_AREA) << endl;
		}
	}
	
	float sd = this->calculateStandardDeviation(areas);
	cout << "连通个数：" << num << endl;
	cout << "方差：" << sd << endl;
	if (sd > 4000 || num < 25)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void ChipDetect_algo::detecting(Mat img, int binarynum )
{
	Mat gray, binaryimg;
	//GaussianBlur(img, img, Size(3, 3), 0);
	cvtColor(img, gray, COLOR_BGR2GRAY);
	threshold(gray, binaryimg, binarynum, 255, THRESH_BINARY);
	/*Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	morphologyEx(binaryimg, binaryimg,MORPH_CLOSE,kernel);*/
	Mat temp;
	resize(binaryimg, temp, Size(binaryimg.cols / 4, binaryimg.rows / 4));
	imshow("testb", temp);
	imwrite("C:/Users/040120012093/Desktop/thrshold/3.bmp", binaryimg);
	vector<vector<Point>>contours;
	findContours(binaryimg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Rect roi; vector<Rect>rois;
	for (int i = 0; i < contours.size(); i++)
	{
		roi = boundingRect(contours[i]);
		float rate = (float)roi.width / roi.height;
		if (rate > 2.2 || rate < 1.1||roi.x<img.cols*0.25|| roi.x > img.cols * 0.75||roi.area()<900000||roi.area()>1200000)continue;
		rois.push_back(roi);
	}
	for (int i = 0; i < rois.size(); i++)
	{
		rectangle(img, rois[i], Scalar(0, 255, 255), 4);
	}
	
	bool chipNG = false;
	
	for (int i = 0; i < rois.size(); i++)
	{
		cout << rois[i].x << "," << rois[i].y << endl;
		int x = rois[i].x + rois[i].width * 0.35;
		int y = rois[i].y + rois[i].height * 0.36;

		Rect detectroi = Rect(Point(x, y), Size(rois[i].width * 0.3, rois[i].height * 0.28));
		rectangle(img, detectroi, Scalar(0, 0, 255), 4, 8);
		chipNG = this->chipNG_algo(detectroi, binaryimg, i);
		Rect detectroi_left; bool leftNG = true;
		for (int j = detectroi.width; j < binaryimg.cols * 0.25; j += detectroi.width)
		{
			detectroi_left = Rect(Point(j, detectroi.y), detectroi.size());
			cout << "debug" << endl;
			leftNG = this->chipNG_algo(detectroi_left, binaryimg, i);
			if (leftNG == false)
			{
				rectangle(img, detectroi_left, Scalar(0, 0, 255), 4);
				break;
			}
		}
		Rect detectroi_right; bool rightNG = true;
		for (int k = binaryimg.cols - detectroi.width * 2; k > binaryimg.cols * 0.7; k -= detectroi.width)
		{
			detectroi_right = Rect(Point(k, detectroi.y), detectroi.size());
			cout << "debug" << endl;
			rightNG = this->chipNG_algo(detectroi_right, binaryimg, i);
			if (rightNG == false)
			{
				rectangle(img, detectroi_right, Scalar(0, 0, 255), 4);
				break;
			}
		}

		if (leftNG == false)
		{
			int startx = detectroi_left.x; int starty = detectroi_left.y - 25;
			int blackline = 0;
			for (int col = startx; col < startx + 0.5 * rois[i].width; col++)
			{
				int blackpts = 0; int pts = 0;
				for (int row = starty; row > rois[i].y + 0.1 * rois[i].height; row--)
				{
					pts++;
					int pv = binaryimg.at<uchar>(row, col);
					if (pv == 0)blackpts++;
				}
				if ((float)blackpts / pts > 0.8)blackline++;
			}
			if (blackline < 5)
			{
				leftNG = true;
				cout << "upng" << endl;
				cout << blackline << endl;
			}
		}
		if (leftNG == false)
		{
			int startx = detectroi_left.x; int starty = detectroi_left.y - 25 + detectroi_left.height;
			int blackline = 0;
			for (int col = startx; col < startx + 0.5 * rois[i].width; col++)
			{
				int blackpts = 0; int pts = 0;
				for (int row = starty; row < rois[i].y + 0.9 * rois[i].height; row++)
				{
					pts++;
					int pv = binaryimg.at<uchar>(row, col);
					if (pv == 0)blackpts++;
				}
				if ((float)blackpts / pts > 0.8)blackline++;
			}
			if (blackline < 5)
			{
				leftNG = true;
				cout << "downng" << endl;
				cout << blackline << endl;
			}
		}


		if (chipNG == false)
		{
			int startx = detectroi.x; int starty = detectroi.y - 25;
			int blackline = 0;
			for (int col = startx; col < startx + 0.5 * rois[i].width; col++)
			{
				int blackpts = 0; int pts = 0;
				for (int row = starty; row > rois[i].y + 0.1 * rois[i].height; row--)
				{
					pts++;
					int pv = binaryimg.at<uchar>(row, col);
					if (pv == 0)blackpts++;
				}
				if ((float)blackpts / pts > 0.8)blackline++;
			}
			if (blackline < 5)
			{
				chipNG = true;
				cout << "upng" << endl;
				cout << blackline << endl;
			}
		}
		if (chipNG == false)
		{
			int startx = detectroi.x; int starty = detectroi.y - 25 + detectroi.height;
			int blackline = 0;
			for (int col = startx; col < startx + 0.5 * rois[i].width; col++)
			{
				int blackpts = 0; int pts = 0;
				for (int row = starty; row < rois[i].y + 0.9 * rois[i].height; row++)
				{
					pts++;
					int pv = binaryimg.at<uchar>(row, col);
					if (pv == 0)blackpts++;
				}
				if ((float)blackpts / pts > 0.8)blackline++;
			}
			if (blackline < 5)
			{
				chipNG = true;
				cout << "downng" << endl;
				cout << blackline << endl;
			}
		}


		if (rightNG == false)
		{
			int startx = detectroi_right.x; int starty = detectroi_right.y - 25;
			int blackline = 0;
			for (int col = startx; col < startx + 0.5 * rois[i].width; col++)
			{
				int blackpts = 0; int pts = 0;
				for (int row = starty; row > rois[i].y + 0.1 * rois[i].height; row--)
				{
					pts++;
					int pv = binaryimg.at<uchar>(row, col);
					if (pv == 0)blackpts++;
				}
				if ((float)blackpts / pts > 0.8)blackline++;
			}
			if (blackline < 5)
			{
				rightNG = true;
				cout << "upng" << endl;
				cout << blackline << endl;
			}

		}
		if (rightNG == false)
		{
			int startx = detectroi_right.x; int starty = detectroi.y - 25 + detectroi_right.height;
			int blackline = 0;
			for (int col = startx; col < startx + 0.5 * rois[i].width; col++)
			{
				int blackpts = 0; int pts = 0;
				for (int row = starty; row < rois[i].y + 0.9 * rois[i].height; row++)
				{
					pts++;
					int pv = binaryimg.at<uchar>(row, col);
					if (pv == 0)blackpts++;
				}
				if ((float)blackpts / pts > 0.8)blackline++;
			}
			if (blackline < 5)
			{
				{
					rightNG = true;
					cout << "downng" << endl;
					cout << blackline << endl;
				}

			}
			
		}
		cout << to_string(i) << leftNG << endl;
		cout << to_string(i) << chipNG << endl;
		cout << to_string(i) << rightNG << endl;
	}


	resize(img, img, Size(img.cols / 4, img.rows / 4));
	imshow("test", img);

	waitKey(0);
}
