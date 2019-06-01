#pragma once

#include <string>

using namespace std;

class process
{
	string pid_;
	string user_;
	string cmd_;
	string cpu_;
	string mem_;
	string up_time_;

public:
	explicit process(string pid);
	void set_pid(int pid);
	string get_pid()const;
	string get_user() const;
	string get_cmd() const;
	int get_cpu() const;
	int get_mem() const;
	string get_up_time() const;
	auto get_process()->string;
};

inline process::process(const string pid)
	: pid_(pid),
	user_(process_parser::get_proc_user(pid)),
	cmd_(process_parser::get_cmd(pid)),
	cpu_(process_parser::get_cpu_percent(pid)),
	mem_(process_parser::get_vm_size(pid)),
	up_time_(process_parser::get_proc_up_time(pid)) {}

inline void process::set_pid(const int pid)
{
	this->pid_ = std::to_string(pid);
}

inline string process::get_pid() const
{
	return this->pid_;
}

inline string process::get_user() const
{
	return this->user_;
}

inline string process::get_cmd() const
{
	return this->cmd_;
}

inline int process::get_cpu() const
{
	return std::stoi(this->cpu_);
}

inline int process::get_mem() const
{
	return std::stoi(this->mem_);
}

inline string process::get_up_time() const
{
	return this->up_time_;
}

inline string process::get_process()
{
	if (!process_parser::is_pid_existing(this->pid_))
	{
		return "";
	}

	this->mem_ = process_parser::get_vm_size(this->pid_);
	this->up_time_ = process_parser::get_proc_up_time(this->pid_);
	this->cpu_ = process_parser::get_cpu_percent(this->pid_);

	return (this->pid_ + "   " + this->user_
		+ "   " + this->mem_.substr(0, 5)
		+ "     " + this->cpu_.substr(0, 5)
		+ "     " + this->up_time_.substr(0, 5)
		+ "    " + this->cmd_.substr(0, 30)
		+ "...");
}
