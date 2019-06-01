# Linux System Monitor

This project implements a process manager for the Linux operating system, similar to htop. Linux stores information about those processes in a couple of files. The program reads those files, extracts the required values, formats them and uses Ncurses to display them inside the console.


## Usage:

1. Clone repository
```
git clone https://github.com/udacity/CppND-System-Monitor
```
2. Follow along with the lesson and make sure you complete the `ProcessParser` and `Process` classes before attempting to run the project.

3. Install `ncurses` package
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
4. Compile and run
```
g++ -std="c++17" main.cpp -lncurses
./a.out
```
5. In case of error that looks like the following: 
```
root@77e30fca8a01:/home/workspace/CppND-Object-Oriented# ./a.out
*** %n in writable segment detected ***
                                      Aborted (core dumped)
```
just keep trying `./a.out` and it should work eventually!
