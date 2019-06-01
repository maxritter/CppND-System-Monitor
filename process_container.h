#pragma once

#include <vector>

#include "process.h"

class process_container
{
	std::vector<process> list_;

public:
	process_container();
	void refresh_list();
	std::string print_list();
	std::vector<std::vector<std::string>> get_list();
};

inline process_container::process_container()
{
	this->refresh_list();
}

inline void process_container::refresh_list() {
	auto pid_list = process_parser::get_pid_list();
	this->list_.clear();

	for (auto i = 0; i < pid_list.size(); i++) 
	{
		process proc(pid_list[i]);
		this->list_.push_back(proc);
	}
}

inline std::string process_container::print_list() 
{
	std::string result = "";

	for (auto i = 0; i < this->list_.size(); i++) 
	{
		result += this->list_[i].get_process();
	}

	return result;
}

inline std::vector<std::vector<std::string> > process_container::get_list() 
{
	std::vector<std::vector<std::string>> values;
	std::vector<std::string> stringifiedList;

	for (auto i = 0; i < list_.size(); i++) 
	{
		stringifiedList.push_back(list_[i].get_process());
	}

	int last_index = 0;

	for (auto i = 0; i < stringifiedList.size(); i++) 
	{
		if (i % 10 == 0 && i > 0) 
		{
			std::vector<std::string>  sub(&stringifiedList[i - 10], &stringifiedList[i]);
			values.push_back(sub);
			last_index = i;
		}

		if (i == (list_.size() - 1) && (i - last_index) < 10) 
		{
			std::vector<std::string> sub(&stringifiedList[last_index], &stringifiedList[i + 1]);
			values.push_back(sub);
		}
	}

	return values;
}
