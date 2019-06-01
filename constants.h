#pragma once

#include <string>

using namespace std;

enum cpu_states 
{
	s_user = 1,
	s_nice,
	s_system,
	s_idle,
	s_iowait,
	s_irq,
	s_softirq,
	s_steal,
	s_guest,
	s_guest_nice
};

class path 
{
public:
	static string base_path() 
	{
		return "/proc/";
	}

	static string cmd_path() 
	{
		return "/cmdline";
	}

	static string status_path() 
	{
		return "/status";
	}

	static string stat_path() 
	{
		return "stat";
	}

	static string up_time_path() 
	{
		return "uptime";
	}

	static string mem_info_path() 
	{
		return "meminfo";
	}

	static string version_path() 
	{
		return "version";
	}
};
