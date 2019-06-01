#pragma once

#include <string>
#include <fstream>

/* Classic helper function */

class util 
{
public:
	static std::string convert_to_time(long int input_seconds);
	static std::string get_progress_bar(std::string percent);
	static std::ifstream get_stream(std::string path);
};

inline std::string util::convert_to_time(const long int input_seconds) 
{
	long minutes = input_seconds / 60;
	const long hours = minutes / 60;
	const long seconds = int(input_seconds % 60);
	minutes = int(minutes % 60);
	auto result = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
	return result;
}

/* Constructing string for given percentage
   50 bars is uniformly stretched 0 - 100 %
   meaning: every 2% is one bar(|) */
inline std::string util::get_progress_bar(std::string percent) 
{
	std::string result = "0% ";
	const int size = 50;
	int  boundaries;
	try 
	{
		boundaries = stof(percent) / 100 * size;
	}
	catch (...) 
	{
		boundaries = 0;
	}

	for (auto i = 0; i < size; i++) 
	{
		if (i <= boundaries) 
		{
			result += "|";
		}
		else 
		{
			result += " ";
		}
	}

	result += " " + percent.substr(0, 5) + " / 100%";
	return result;
}

/* Wrapper for creating streams */
inline std::ifstream util::get_stream(const std::string path) 
{
	std::ifstream stream;
	stream.open(path, std::ifstream::in);

	if (!stream && !stream.is_open()) 
	{
		stream.close();
		throw std::runtime_error("Non - existing PID");
	}

	return stream;
}
