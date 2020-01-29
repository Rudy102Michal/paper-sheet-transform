#ifndef H_MYAPPLICATION
#define H_MYAPPLICATION

#include <vector>
#include <opencv2/opencv.hpp>
#include <string>
#include <exception>
#include "KeyInterpreter.h"

class MyApplication
{
private:
	std::string image_name;
	cv::Mat edge_img, cont_img, orig_img, transformed_img;
	KeyInterpreter * interpreter;

	class ApplicationException : public std::exception
	{
	public:
		int code;
		std::string msg_;
		ApplicationException(const char* msg, int _code) : msg_(msg), code(_code)
		{
		}
		virtual char const *what() const noexcept { return msg_.c_str(); }
	};

public:
	MyApplication();
	MyApplication(std::string image_file_path);
	virtual ~MyApplication();
	
	int run();
	void setImageFileName(std::string image_file_path) { image_name = image_file_path; };

private:
	cv::Mat calculatePerspectiveTransform(const cv::Mat & img, std::vector<cv::Point> & corners);
	void loadImageData();
	void detectEdges();
	std::vector<cv::Point> calculateCorners(std::vector<cv::Point> & contour);
	std::vector<cv::Point> handleContours();
	std::vector<cv::Point2f> sortCornersInOrderAndConvert(std::vector<cv::Point> & corners);
};

#endif
