#include <stdlib.h>
#include <iostream>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/UserTask.h"

using std::cin;
using std::mutex;
using std::string;
using std::thread;
using std::vector;


int main(int argc, char *argv[])
{
	mutex mutex_;
	StompProtocol stompProtocol;
	UserTask userTask(stompProtocol,mutex_);
	std::thread th_keyboard(&UserTask::keyboardRun, &userTask);
    std::thread th_socket(&UserTask::socketRun, &userTask);
    th_keyboard.join();
    th_socket.join();
	return 0;
}