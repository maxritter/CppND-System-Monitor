#pragma once

#include <string>
#include <vector>
#include "process_parser.h"

class sys_info
{
	std::vector<std::string> last_cpu_stats_;
	std::vector<std::string> current_cpu_stats_;
	std::vector<std::string> cores_stats_;
	std::vector<std::vector<std::string>> last_cpu_cores_stats_;
	std::vector<std::vector<std::string>> current_cpu_cores_stats_;
	std::string cpu_percent_;
	float mem_percent_;
	std::string os_name_;
	std::string kernel_ver_;
	long up_time_;
	int total_proc_;
	int running_proc_;
	int threads_;

public:

	sys_info();
	void set_attributes();
	void set_last_cpu_measures();
	std::string get_mem_percent() const;
	long get_up_time() const;
	std::string get_threads() const;
	std::string get_total_proc() const;
	std::string get_running_proc() const;
	std::string get_kernel_version() const;
	std::string get_os_name() const;
	std::string get_cpu_percent() const;
	void get_other_cores(int size);
	void set_cpu_cores_stats();
	std::vector<std::string> get_cores_stats() const;
};

inline void sys_info::get_other_cores(const int size) 
{
	/* When number of cores is detected, vectors are modified to fit incoming data */
	this->cores_stats_ = std::vector<std::string>();
	this->cores_stats_.resize(size);
	this->last_cpu_cores_stats_ = std::vector<std::vector<std::string>>();
	this->last_cpu_cores_stats_.resize(size);
	this->current_cpu_cores_stats_ = std::vector<std::vector<std::string>>();
	this->current_cpu_cores_stats_.resize(size);

	for (auto i = 0; i < size; i++) 
	{
		this->last_cpu_cores_stats_[i] = process_parser::get_sys_cpu_percent(to_string(i));
	}
}

inline void sys_info::set_last_cpu_measures() 
{
	this->last_cpu_stats_ = process_parser::get_sys_cpu_percent();
}

inline void sys_info::set_cpu_cores_stats() 
{
	/* Getting data from files (previous data is required) */
	for (auto i = 0; i < this->current_cpu_cores_stats_.size(); i++) 
	{
		this->current_cpu_cores_stats_[i] = process_parser::get_sys_cpu_percent(to_string(i));
	}

	for (auto i = 0; i < this->current_cpu_cores_stats_.size(); i++) 
	{
		/* After acquirement of data we are calculating every core percentage of usage */
		this->cores_stats_[i] = process_parser::print_cpu_stats(this->last_cpu_cores_stats_[i], this->current_cpu_cores_stats_[i]);
	}

	this->last_cpu_cores_stats_ = this->current_cpu_cores_stats_;
}

inline sys_info::sys_info()
{
	/* 	Getting initial info about system 
	Initial data for individual cores is set
	System data is set
	*/
	this->get_other_cores(process_parser::get_number_of_cores());
	this->set_last_cpu_measures();
	this->set_attributes();
	this->os_name_ = process_parser::get_os_name();
	this->kernel_ver_ = process_parser::get_sys_kernel_version();
}

inline void sys_info::set_attributes() 
{
	/* Getting parsed data */
	this->mem_percent_ = process_parser::get_sys_ram_percent();
	this->up_time_ = process_parser::get_sys_up_time();
	this->total_proc_ = process_parser::get_total_number_of_processes();
	this->running_proc_ = process_parser::get_number_of_running_processes();
	this->threads_ = process_parser::get_total_threads();
	this->current_cpu_stats_ = process_parser::get_sys_cpu_percent();
	this->cpu_percent_ = process_parser::print_cpu_stats(this->last_cpu_stats_, this->current_cpu_stats_);
	this->last_cpu_stats_ = this->current_cpu_stats_;
	this->set_cpu_cores_stats();
}

inline std::vector<std::string> sys_info::get_cores_stats() const 
{
	auto result = std::vector<std::string>();

	for (auto i = 0; i < this->cores_stats_.size(); i++) 
	{
		auto temp = ("cpu" + to_string(i) + ": ");
		float check = 0;

		if (!this->cores_stats_[i].empty())
		{
			check = stof(this->cores_stats_[i]);
		}
			
		if (!check || this->cores_stats_[i] == "nan") 
		{
			return std::vector<std::string>();
		}

		temp += util::get_progress_bar(this->cores_stats_[i]);
		result.push_back(temp);
	}

	return result;
}

inline std::string sys_info::get_cpu_percent() const
{
	return this->cpu_percent_;
}

inline std::string sys_info::get_mem_percent() const
{
	return to_string(this->mem_percent_);
}

inline long sys_info::get_up_time() const
{
	return this->up_time_;
}

inline std::string sys_info::get_kernel_version() const
{
	return this->kernel_ver_;
}

inline std::string sys_info::get_total_proc() const
{
	return to_string(this->total_proc_);
}

inline std::string sys_info::get_running_proc() const
{
	return to_string(this->running_proc_);
}

inline std::string sys_info::get_threads() const
{
	return to_string(this->threads_);
}

inline std::string sys_info::get_os_name() const
{
	return this->os_name_;
}
