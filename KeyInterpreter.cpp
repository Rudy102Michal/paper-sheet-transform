#include "KeyInterpreter.h"



KeyInterpreter::KeyInterpreter() : ms_interval(10)
{
}

KeyInterpreter::KeyInterpreter(int read_interval) : ms_interval(read_interval)
{
}

KeyInterpreter::KeyInterpreter(std::unordered_map<int, key_callback> mapping) : key_mapping(mapping), ms_interval(10)
{
}

KeyInterpreter::KeyInterpreter(int read_interval, std::unordered_map<int, key_callback> mapping) : ms_interval(read_interval), key_mapping(mapping)
{
}


KeyInterpreter::~KeyInterpreter()
{
}

void KeyInterpreter::setSingleMapping(int key, key_callback action)
{
	key_mapping[key] = action;
}

void KeyInterpreter::setAllMappings(std::unordered_map<int, key_callback> mapping)
{
	key_mapping = mapping;
}

int KeyInterpreter::next()
{
	int keycode = cv::waitKey(ms_interval);
	if (keycode >= 0)
	{
		key_callback action;
		try {
			action = key_mapping.at(keycode);
		}
		catch (std::out_of_range)
		{
			return keycode;
		}

		action();
	}

	return keycode;
}
