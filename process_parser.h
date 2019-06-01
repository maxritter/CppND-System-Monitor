#pragma once

#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"

using namespace std;

class process_parser 
{
	std::ifstream stream_;

public:
	static string get_cmd(string pid);
	static vector<string> get_pid_list();
	static std::string get_vm_size(const string& pid);
	static std::string get_cpu_percent(string pid);
	static long int get_sys_up_time();
	static std::string get_proc_up_time(const string& pid);
	static string get_proc_user(string pid);
	static vector<string> get_sys_cpu_percent(string core_number = "");
	static float get_sys_ram_percent();
	static string get_sys_kernel_version();
	static float get_sys_active_cpu_time(vector<string> values);
	static float get_sys_idle_cpu_time(vector<string>values);
	static int get_number_of_cores();
	static int get_total_threads();
	static int get_total_number_of_processes();
	static int get_number_of_running_processes();
	static string get_os_name();
	static std::string print_cpu_stats(std::vector<std::string> values1, std::vector<std::string> values2);
	static bool is_pid_existing(string pid);
};

inline string process_parser::get_cmd(string pid)
{
	string line;

	auto stream = util::get_stream((path::base_path() + pid + path::cmd_path()));

	std::getline(stream, line);
	return line;
}

inline vector<string> process_parser::get_pid_list()
{
	DIR* dir;
	vector<string> container;

	/* Check if we can open the proc dir */
	if (!((dir = opendir("/proc"))))
	{
		throw std::runtime_error(std::strerror(errno));
	}

	/* Iterate through all possible PID folders inside proc */
	while (dirent * dirp = readdir(dir)) 
	{
		/* Is this a directory */
		if (dirp->d_type != DT_DIR)
		{
			continue;
		}
			
		/* Is every character of the name a digit */
		if (all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](const char c) { return std::isdigit(c); })) 
		{
			container.push_back(dirp->d_name);
		}
	}

	/* Validating process of directory closing */
	if (closedir(dir))
	{
		throw std::runtime_error(std::strerror(errno));
	}
		
	return container;
}

inline std::string process_parser::get_vm_size(const string& pid)
{
	/* Markers name */
	std::string name = "VmData";

	/* Other variables */
	std::string line, value;
	float result;

	/* Open stream for the specific file */
	auto stream = util::get_stream(path::base_path() + pid + path::status_path());

	/* Go through all lines of the file */
	while (!std::getline(stream, line))
	{
		/* Do a search for our maker name */
		if (line.compare(0, name.size(), name) == 0)
		{
			/* Construct a vector of string values sliced by white spaces */
			std::istringstream buf(line);
			std::istream_iterator<string> beg(buf), end;
			std::vector<string> values(beg, end);

			/* Conversion of second value from KB to MB, then use result */
			result = stof(values[1]) / 1024.0;
			break;
		}
	}

	return to_string(result);
}

inline std::string process_parser::get_cpu_percent(string pid)
{
	string line, value;
	float result;

	/* Construct our stat path */
	auto stream = util::get_stream((path::base_path() + pid + "/" + path::stat_path()));

	/* Read the whole file into one line */
	getline(stream, line);

	/* Create a sliced string vector */
	const auto& str = line;
	istringstream buf(str);
	istream_iterator<string> beg(buf), end;
	vector<string> values(beg, end);

	/* Parse all the required information */
	float utime = stof(get_proc_up_time(pid));
	float stime = stof(values[14]);
	float cutime = stof(values[15]);
	float cstime = stof(values[16]);
	float starttime = stof(values[21]);
	float uptime = get_sys_up_time();
	float freq = sysconf(_SC_CLK_TCK);
	float total_time = utime + stime + cutime + cstime;
	float seconds = uptime - (starttime / freq);

	/* Calculate our cpu usage in percentage and convert back to string */
	result = 100.0 * ((total_time / freq) / seconds);
	return to_string(result);
}

inline long process_parser::get_sys_up_time()
{
	string line;

	auto stream = util::get_stream(path::base_path() + path::up_time_path());
	getline(stream, line);
	istringstream buf(line);
	istream_iterator<string> beg(buf), end;
	vector<string> values(beg, end);

	return stoi(values[0]);
}

inline std::string process_parser::get_proc_up_time(const string& pid)
{
	string line, value;

	/* Construct our stat path */
	auto stream = util::get_stream(path::base_path() + pid + "/" + path::stat_path());

	/* Create a sliced string vector */
	getline(stream, line);
	const auto& str = line;
	istringstream buf(str);
	istream_iterator<string> beg(buf), end;
	vector<string> values(beg, end); 

	/* Calculate process up time and convert back to string */
	float proc_up_time = float(stof(values[13]) / sysconf(_SC_CLK_TCK));
	return to_string(proc_up_time);
}

inline string process_parser::get_proc_user(string pid)
{
	string line;
	string name = "Uid:";
	string result = "";
	auto stream = util::get_stream((path::base_path() + pid + path::status_path()));

	/* Getting UID for user */
	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);

			result = values[1];
			break;
		}
	}

	stream = util::get_stream("/etc/passwd");
	name = ("x:" + result);

	/* Searching for name of the user with selected UID */
	while (std::getline(stream, line)) 
	{
		if (line.find(name) != std::string::npos)
		{
			result = line.substr(0, line.find(":"));
			return result;
		}
	}

	return "";
}

inline vector<string> process_parser::get_sys_cpu_percent(string core_number)
{
	string line;
	string name = "cpu" + core_number;
	string value;

	auto stream = util::get_stream((path::base_path() + path::stat_path()));

	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);
			return values;
		}
	}
	return (vector<string>());
}

inline float process_parser::get_sys_ram_percent()
{
	string line;
	string name1 = "MemAvailable:";
	string name2 = "MemFree:";
	string name3 = "Buffers:";

	string value;

	auto stream = util::get_stream((path::base_path() + path::mem_info_path()));
	float total_mem = 0;
	float free_mem = 0;
	float buffers = 0;

	while (std::getline(stream, line)) 
	{
		if (total_mem != 0 && free_mem != 0)
		{
			break;
		}
			
		if (line.compare(0, name1.size(), name1) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);
			total_mem = stof(values[1]);
		}

		if (line.compare(0, name2.size(), name2) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);
			free_mem = stof(values[1]);
		}

		if (line.compare(0, name3.size(), name3) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);
			buffers = stof(values[1]);
		}
	}

	return float(100.0 * (1 - (free_mem / (total_mem - buffers))));
}

inline string process_parser::get_sys_kernel_version()
{
	string line;
	string name = "Linux version ";

	auto stream = util::get_stream((path::base_path() + path::version_path()));

	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);
			return values[2];
		}
	}

	return "";
}

inline float process_parser::get_sys_active_cpu_time(vector<string> values)
{
	return (stof(values[s_user]) +
		stof(values[s_nice]) +
		stof(values[s_system]) +
		stof(values[s_irq]) +
		stof(values[s_softirq]) +
		stof(values[s_steal]) +
		stof(values[s_guest]) +
		stof(values[s_guest_nice]));
}

inline float process_parser::get_sys_idle_cpu_time(vector<string> values)
{
	return (stof(values[s_idle]) + stof(values[s_iowait]));
}

inline int process_parser::get_number_of_cores()
{
	string line;
	string name = "cpu cores";

	auto stream = util::get_stream((path::base_path() + "cpuinfo"));

	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);

			return stoi(values[3]);
		}
	}

	return 0;
}

inline int process_parser::get_total_threads()
{
	std::string line;

	auto stream = util::get_stream(path::base_path() + "sys/kernel/threads-max");

	std::getline(stream, line);
	return std::stoi(line);
}

inline int process_parser::get_total_number_of_processes()
{
	string line;
	int result = 0;
	string name = "processes";

	auto stream = util::get_stream((path::base_path() + path::stat_path()));

	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);

			result += stoi(values[1]);
			break;
		}
	}

	return result;
}

inline int process_parser::get_number_of_running_processes()
{
	string line;
	int result = 0;
	string name = "procs_running";

	auto stream = util::get_stream((path::base_path() + path::stat_path()));

	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> values(beg, end);

			result += stoi(values[1]);
			break;
		}
	}

	return result;
}

inline string process_parser::get_os_name()
{
	const std::string name = "PRETTY_NAME";
	std::string line;
	std::string result = "";

	auto stream = util::get_stream("/etc/os-release");

	while (std::getline(stream, line)) 
	{
		if (line.compare(0, name.size(), name) == 0) 
		{
			const int start = line.find("=") + 2;
			const int len = line.size() - start - 1;
			result = line.substr(start, len);
			break;
		}
	}

	return result;
}

inline std::string process_parser::print_cpu_stats(const std::vector<std::string> values1, const std::vector<std::string> values2)
{
	const float active_time = get_sys_active_cpu_time(values2) - get_sys_active_cpu_time(values1);
	const float idle_time = get_sys_idle_cpu_time(values2) - get_sys_idle_cpu_time(values1);
	const float total_time = active_time + idle_time;
	const float result = 100.0 * (active_time / total_time);

	return to_string(result);
}

inline bool process_parser::is_pid_existing(string pid)
{
	const auto pid_list = process_parser::get_pid_list();
	return std::find(pid_list.begin(), pid_list.end(), pid) != pid_list.end();
}

