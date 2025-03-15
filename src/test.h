#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
#include <vector>
#include <cmath>
#include <numeric>
using namespace cv;
using namespace std;

class ChipDetect_algo
{
	double distance(Point pt1, Point pt2)
	{
		return pow(pow(pt1.x -pt2.x,2)+ pow(pt1.y +-pt2.y, 2), 0.5);
	}
    float calculateStandardDeviation( std::vector<float> data) 
    {
        if (data.empty()) {
            return 0.0f;
        }
        sort(data.begin(), data.end());
        data.pop_back();
        swap(data[0], data[data.size()-1]);
        data.pop_back();
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        float mean = sum / data.size();
        cout << "平均值：" << mean << endl;
        float squared_diff_sum = 0.0f;
        for (float num : data) {
            float diff = num - mean;
            squared_diff_sum += diff * diff;
        }
        float variance = squared_diff_sum / data.size();
        if (mean < 900)
        {
            return  std::sqrt(variance) + 5000;
        }
        return std::sqrt(variance);
    }
    bool chipNG_algo(Rect detectroi,Mat binaryimg,int i);
  
public:
	void detecting(Mat& img, int binarynum = 180);

};
