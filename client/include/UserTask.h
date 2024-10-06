#pragma once
#include "Summary.h"
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include <map>

using std::vector;
using std::string;
using std::mutex;


class UserTask {
public:
UserTask(StompProtocol& _stompProtocol,mutex& newMutex);
void split_str(string const &str, const char delim, vector<string> &out);
void socketRun();
void keyboardRun();
bool createSummaryFile(string gameName,string fileName,Summary summary);



private:
ConnectionHandler connectHandler;
StompProtocol& stompProtocol;
bool shouldTerminate;
mutex& mutex_;
bool connected;
bool write;


};
