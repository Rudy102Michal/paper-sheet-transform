#include "MyApplication.h"
#include <iostream>
#include <algorithm>

MyApplication::MyApplication()
{
	interpreter = new KeyInterpreter(10);
}

MyApplication::MyApplication(std::string image_file_path) : image_name(image_file_path)
{
	interpreter = new KeyInterpreter(10);
}


MyApplication::~MyApplication()
{
	delete interpreter;
}

int MyApplication::run()
{
	try {
		loadImageData();

		cv::Mat image = orig_img.clone();

		// Original
		auto a_show_original = [this]() -> void {
			cv::imshow("Original", orig_img);
		};
		interpreter->setSingleMapping(int('o'), a_show_original);
		
		// Edges
		detectEdges();
		auto a_show_edges = [this]() -> void {
			cv::imshow("Edges", edge_img);
		};
		interpreter->setSingleMapping(int('e'), a_show_edges);
		
		// Contours
		cont_img = image.clone();
		std::vector<cv::Point> the_contour = handleContours();
		auto a_show_contours = [this]() -> void {
			cv::imshow("Contoured", cont_img);
		};
		interpreter->setSingleMapping(int('c'), a_show_contours);

		// Corners
		std::vector<cv::Point> corners = calculateCorners(the_contour);
		int i = 0;
		for (auto p : corners)
		{
			cv::circle(cont_img, p, 15, cv::Scalar(200.0, 0.0, 0.0), -1);
			cv::putText(cont_img, std::to_string(++i), p, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar::all(255), 3, 8);
		}
		
		// Perspective and ratio
		transformed_img = calculatePerspectiveTransform(image, corners);

		// Rotation
		cv::imshow("Done?", transformed_img);
		auto a_rotate_done = [this]() -> void {
			cv::transpose(transformed_img, transformed_img);
			cv::flip(transformed_img, transformed_img, 1);
			cv::imshow("Done?", transformed_img);
		};
		interpreter->setSingleMapping(int('r'), a_rotate_done);
		
		while (true)
		{
			int keycode = interpreter->next();
			if (keycode == 27)					// Escape key
				break;
		}
		cv::destroyAllWindows();
	}
	catch (ApplicationException ex)
	{
		std::cerr << ex.what();
		std::cin.get();
		return ex.code;
	}
	return 0;
}

cv::Mat MyApplication::calculatePerspectiveTransform(const cv::Mat & img, std::vector<cv::Point>& corners)
{	
	std::vector<cv::Point2f> ordered_corners = sortCornersInOrderAndConvert(corners);;

	auto distance = [](const cv::Point & a, const cv::Point & b) -> double {
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	};

	float height = static_cast<float>(MAX(distance(ordered_corners[0], ordered_corners[3]), distance(ordered_corners[1], ordered_corners[2])));
	float width = static_cast<float>(MAX(distance(ordered_corners[0], ordered_corners[1]), distance(ordered_corners[2], ordered_corners[3])));

	const double root_two = 1.41421356237;

	if (height > width)
		width = height / root_two;
	else
		height = width / root_two;

	std::vector<cv::Point2f> dest;
	dest.push_back(cv::Point2f(0.0f, 0.0f));
	dest.push_back(cv::Point2f(width - 1.0f, 0.0f));
	dest.push_back(cv::Point2f(width - 1.0f, height - 1.0f));
	dest.push_back(cv::Point2f(0.0f, height - 1.0f));
	cv::Mat result;
	cv::Mat persp;
	try {
		persp = cv::getPerspectiveTransform(ordered_corners, dest);
	}
	catch (cv::Exception e)
	{
		std::cerr << e.what() << std::endl;
	}
	cv::warpPerspective(img, result, persp, cv::Size(width, height));
	return result;
}

void MyApplication::loadImageData()
{
	orig_img = cv::imread(image_name.c_str());			// CV_LOAD_IMAGE_COLOR for OpenCV < 3.0
	if (orig_img.empty()) {
		throw ApplicationException("File load error.", 1);
	}
}

void MyApplication::detectEdges()
{
	cv::Mat tmp1, tmp2;
	cv::cvtColor(orig_img, tmp1, cv::COLOR_BGR2GRAY); // Older: cv::CV_BGR2GRAY
	cv::GaussianBlur(tmp1, tmp2, cv::Size(5, 5), 0);
	cv::Canny(tmp2, edge_img, 120.0, 200.0);
}

std::vector<cv::Point> MyApplication::calculateCorners(std::vector<cv::Point> & contour)
{
	std::vector<cv::Point> corners;

	cv::approxPolyDP(contour, corners, 5.0, true);

	if (corners.size() != 4)
		throw ApplicationException("Failed to find 4 corners for the page.", 2);

	return corners;
}

std::vector<cv::Point> MyApplication::handleContours()
{
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(edge_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	cv::drawContours(cont_img, contours, 0, cv::Scalar(0, 200, 100), 5);
	return contours[0];
}

std::vector<cv::Point2f> MyApplication::sortCornersInOrderAndConvert(std::vector<cv::Point>& corners)
{
	cv::Point top_left = corners[0];
	for (int i = 1; i < 4; ++i)
	{
		if (corners[i].x + corners[i].y < top_left.x + top_left.y)
			top_left = corners[i];
	}

	std::sort(corners.begin(), corners.end(), [](const cv::Point &a, const cv::Point &b) -> bool {
		return a.y < b.y;
	});

	if (corners[0].x > corners[1].x)
		std::swap(corners[0], corners[1]);

	if (corners[2].x < corners[3].x)
		std::swap(corners[2], corners[3]);

	while (corners[0] != top_left)
	{
		corners.push_back(corners[0]);
		corners.erase(corners.begin());
	}

	std::vector<cv::Point2f> result;
	for (auto p : corners)
		result.push_back(static_cast<cv::Point2f>(p));

	return result;
}
