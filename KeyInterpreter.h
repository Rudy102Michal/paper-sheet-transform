#ifndef H_KEYINTERPRETER
#define H_KEYINTERPRETER

#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <functional>

//typedef void(*key_callback)();
typedef std::function<void()> key_callback;

class KeyInterpreter
{
private:
	std::unordered_map<int, key_callback> key_mapping;
	int ms_interval;

public:
	KeyInterpreter();
	KeyInterpreter(int read_interval);
	KeyInterpreter(std::unordered_map<int, key_callback> mapping);
	KeyInterpreter(int read_interval, std::unordered_map<int, key_callback> mapping);
	virtual ~KeyInterpreter();

	void setSingleMapping(int key, key_callback action);
	void setAllMappings(std::unordered_map<int, key_callback> mapping);
	int next();
};

#endif
