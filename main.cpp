#include <thread>
#include <string>
#include <vector>
#include <ncurses.h>

#include "util.h"
#include "sys_info.h"
#include "process_container.h"

using namespace std;

char* get_c_string(const std::string& str) 
{
	const auto cstr = new char[str.length() + 1];
	std::strcpy(cstr, str.c_str());
	return cstr;
}

void write_sys_info_to_console(sys_info sys, WINDOW* sys_win) 
{
	sys.set_attributes();

	mvwprintw(sys_win, 2, 2, get_c_string(("OS: " + sys.get_os_name())));
	mvwprintw(sys_win, 3, 2, get_c_string(("Kernel version: " + sys.get_kernel_version())));
	mvwprintw(sys_win, 4, 2, get_c_string("CPU: "));
	wattron(sys_win, COLOR_PAIR(1));
	wprintw(sys_win, get_c_string(util::get_progress_bar(sys.get_cpu_percent())));
	wattroff(sys_win, COLOR_PAIR(1));
	mvwprintw(sys_win, 5, 2, get_c_string(("Other cores:")));
	wattron(sys_win, COLOR_PAIR(1));

	auto val = sys.get_cores_stats();
	for (auto i = 0; i < val.size(); i++) 
	{
		mvwprintw(sys_win, (6 + i), 2, get_c_string(val[i]));
	}

	wattroff(sys_win, COLOR_PAIR(1));
	mvwprintw(sys_win, 10, 2, get_c_string(("Memory: ")));
	wattron(sys_win, COLOR_PAIR(1));
	wprintw(sys_win, get_c_string(util::get_progress_bar(sys.get_mem_percent())));
	wattroff(sys_win, COLOR_PAIR(1));
	mvwprintw(sys_win, 11, 2, get_c_string(("Total Processes:" + sys.get_total_proc())));
	mvwprintw(sys_win, 12, 2, get_c_string(("Running Processes:" + sys.get_running_proc())));
	mvwprintw(sys_win, 13, 2, get_c_string(("Up Time: " + util::convert_to_time(sys.get_up_time()))));
	wrefresh(sys_win);
}

void get_process_list_to_console(std::vector<string> processes, WINDOW* win) 
{
	wattron(win, COLOR_PAIR(2));
	mvwprintw(win, 1, 2, "PID:");
	mvwprintw(win, 1, 9, "User:");
	mvwprintw(win, 1, 16, "CPU[%%]:");
	mvwprintw(win, 1, 26, "RAM[MB]:");
	mvwprintw(win, 1, 35, "Uptime:");
	mvwprintw(win, 1, 44, "CMD:");
	wattroff(win, COLOR_PAIR(2));

	for (int i = 0; i < processes.size(); i++) 
	{
		mvwprintw(win, 2 + i, 2, get_c_string(processes[i]));
	}
}
void print_main(const sys_info sys, process_container procs) 
{
	initscr();	
	noecho(); 
	cbreak(); 
	start_color(); 

	int y_max, x_max;
	getmaxyx(stdscr, y_max, x_max);
	WINDOW* sys_win = newwin(17, x_max - 1, 0, 0);
	WINDOW* proc_win = newwin(15, x_max - 1, 18, 0);

	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	int counter = 0;

	while (true) 
	{
		box(sys_win, 0, 0);
		box(proc_win, 0, 0);
		procs.refresh_list();
		std::vector<std::vector<std::string>> processes = procs.get_list();
		write_sys_info_to_console(sys, sys_win);
		get_process_list_to_console(processes[counter], proc_win);
		wrefresh(sys_win);
		wrefresh(proc_win);
		refresh();
		sleep(1);

		if (counter == (processes.size() - 1)) 
		{
			counter = 0;
		}
		else 
		{
			counter++;
		}
	}

	endwin();
}

int main(int argc, char* argv[])
{
	const process_container procs;
	const sys_info sys;

	print_main(sys, procs);

	return 0;
}
