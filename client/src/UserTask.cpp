#include "../include/UserTask.h"
#include "../include/ConnectionHandler.h"
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;
using std::cin;
using std::mutex;
using std::pair;
using std::string;

UserTask::UserTask(StompProtocol &_stompProtocol, mutex &newMutex) : connectHandler("", 0), stompProtocol(_stompProtocol), shouldTerminate(false), mutex_(newMutex), connected(false),write(false) {}

void UserTask::socketRun()
{
    while (!connected)
    {
        std::lock_guard<std::mutex> lock(mutex_);
    }
    while (!shouldTerminate)
    {
        string answer;
        if (!connectHandler.getLine(answer))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            shouldTerminate = true;
        }
        const char delimiter = '\n';
        vector<string> out;
        split_str(answer, delimiter, out);
        if (out.at(0).compare("CONNECTED") == 0)
        {
            std::cout << "Login successful \n"
                      << std::endl;
        }
        if (out.at(0).compare("RECEIPT") == 0)
        {
            bool ifDisconnect = stompProtocol.clientResult(answer);
            if (!ifDisconnect)
            {
                std::cout << "Logout client \n"
                          << std::endl;
                shouldTerminate = true;
                break;
            }
        }
        if (out.at(0).compare("MESSAGE") == 0)
        {
            bool ifDisconnect = stompProtocol.clientResult(answer);
            if (!ifDisconnect)
            {
                std::cout << "Logout client \n"
                          << std::endl;
                shouldTerminate = true;
                break;
            }
        }
        if (out.at(0).compare("ERROR") == 0)
        {
            bool ifDisconnect = stompProtocol.clientResult(answer);
            if (!ifDisconnect)
            {
                std::cout << "Disconnected. Exiting...\n"
                          << std::endl;
                shouldTerminate = true;
                break;
            }
        }
    }
    connected = false;
    connectHandler.close();
}

void UserTask::keyboardRun()
{
    const short bufsize = 1024;
    char buf[bufsize];
    cin.getline(buf, bufsize);
    string line(buf);
    const char delimiter = ' ';
    vector<string> out;
    split_str(line, delimiter, out);
    if (out.at(0).compare("login") != 0)
    {
        std::cout << "Logging must be correcly for connecting \n"
                  << std::endl;
        string msgDisconnect;
        string receiptId_ = std::to_string(0);
        msgDisconnect = "DISCONNECT";
        msgDisconnect.push_back('\n');
        msgDisconnect = msgDisconnect + "receipt :" + receiptId_;
        msgDisconnect.push_back('\n');
        msgDisconnect.push_back('\n');
        if (connectHandler.sendLine(msgDisconnect))
        {
            std::cout << "Cannot connect to server \n"
                      << std::endl;
            shouldTerminate = true;
        }
    }
    else
    {
        string api = out.at(1);
        string api_temp = api;
        const char delimiter2 = ':';
        vector<string> out2;
        split_str(api, delimiter2, out2);
        string host = out2.at(0);
        int portInt = std::stoi(out2.at(1));
        short port = (short)portInt;
        connectHandler.setHost(host);
        connectHandler.setPort(port);
        if (!connectHandler.connect())
        {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        }
        connected = true;
        string msg = "CONNECT";
        msg.push_back('\n');
        msg = msg + "accept - version :1.2";
        msg.push_back('\n');
        msg = msg + "host : stomp . cs . bgu . ac . il";
        msg.push_back('\n');
        msg = msg + "login :" + out.at(2);
        msg.push_back('\n');
        msg = msg + "passcode :" + out.at(3);
        msg.push_back('\n');
        msg.push_back('\n');
        if (!connectHandler.sendLine(msg))
        {
            std::cout << "Cannot connect to server \n"
                      << std::endl;
            shouldTerminate = true;
        }
        stompProtocol.updateUserName(out.at(2));
    }
    while (!shouldTerminate)
    {
        const short bufsize = 1024;
        char buf1[bufsize];
        cin.getline(buf1, bufsize);
        if (shouldTerminate)
        {
            break;
        }
        string line1(buf1);
        const char delimiter = ' ';
        vector<string> out5;
        split_str(line1, delimiter, out5);
        if (out5.at(0).compare("logout") == 0)
        {
            string toSend = stompProtocol.processToServer(line1);
            if (!connectHandler.sendLine(toSend))
            {
                std::cout << "Cannot connect to server \n"
                          << std::endl;
                shouldTerminate = true;
            }
            shouldTerminate = true;
            connected = false;
            break;
        }
        else if (out5.at(0).compare("report") == 0)
        {
            string path = out5.at(1);
            names_and_events nae = parseEventsFile(path);
            write=true;
            for (const Event &_event : nae.events)
            {
                boost::algorithm::to_lower(nae.team_a_name);
                boost::algorithm::to_lower(nae.team_b_name);
                string toSend = stompProtocol.processToServerEvent(nae.team_a_name, nae.team_b_name, _event);
                if (!connectHandler.sendLine(toSend))
                {
                    std::cout << "Cannot connect to server \n"
                              << std::endl;
                    shouldTerminate = true;
                }
            }
            write=false;
            std::cout << "All reports were sent \n"
                      << std::endl;
        }
        else if (out5.at(0).compare("summary") == 0)
        {
            while(write){
                std::lock_guard<std::mutex> lock(mutex_);
            }
            if (!stompProtocol.isSummaryExisted(out5.at(2), out5.at(1)))
            {
                std::cout << "Summary was not found :( ) \n"
                          << std::endl;
            }
            else
            {
                Summary summary = stompProtocol.getSummaryPerUserGame(out5.at(2), out5.at(1));
                createSummaryFile(out5.at(1), out5.at(3), summary);
                std::cout << "Summary created \n"
                          << std::endl;
            }
        }
        else
        {
            string toSend = stompProtocol.processToServer(line1);
            if (!connectHandler.sendLine(toSend))
            {
                std::cout << "Cannot connect to server \n"
                          << std::endl;
                shouldTerminate = true;
            }
        }
    }
}

void UserTask::split_str(string const &str, const char delim, vector<string> &out)
{
    std::stringstream s(str);
    string str2;
    while (std::getline(s, str2, delim))
    {
        out.push_back(str2);
    }
}

bool UserTask::createSummaryFile(string gameName, string fileName, Summary summary)
{
    std::fstream file;
    file.open(fileName, std::ofstream::out);
    if (!file)
    {
        return false;
    }
    const char delimiter = '_';
    vector<string> out;
    split_str(gameName, delimiter, out);
    string tofile = "";
    tofile = out.at(0) + " vs " + out.at(1);
    tofile.push_back('\n');
    tofile = tofile + "Game stats:";
    tofile.push_back('\n');
    tofile = tofile + "General stats:";
    tofile.push_back('\n');
    string active = summary.getActive();
    boost::trim(active);
    tofile = tofile + "active: " + active;
    tofile.push_back('\n');
    string beforeHalfTime = summary.getBeforeHalfTime();
    boost::trim(beforeHalfTime);
    tofile = tofile + "before halftime: " + beforeHalfTime;
    tofile.push_back('\n');
    tofile = tofile + out.at(0) + " stats:";
    tofile.push_back('\n');
    string teamA_goals = summary.getTeamA_goals();
    boost::trim(teamA_goals);
    tofile = tofile + "goals: " + teamA_goals;
    tofile.push_back('\n');
    string teamA_possession = summary.getTeamA_possession();
    boost::trim(teamA_possession);
    tofile = tofile + "possession: " + teamA_possession;
    tofile.push_back('\n');
    tofile = tofile + out.at(1) + " stats:";
    tofile.push_back('\n');
    string teamB_goals = summary.getTeamB_goals();
    boost::trim(teamB_goals);
    tofile = tofile + "goals: " + teamB_goals;
    tofile.push_back('\n');
    string teamB_possession = summary.getTeamB_possession();
    boost::trim(teamB_possession);
    tofile = tofile + "possession: " + teamB_possession;
    tofile.push_back('\n');
    tofile = tofile + "Game event reports:";
    tofile.push_back('\n');
    for (const map<string, string> &_eventMap : summary.getsummaryEvents())
    {
        string time = _eventMap.at("time");
        boost::trim(time);
        string eventName = _eventMap.at("event name");
        boost::trim(eventName);
        string description = _eventMap.at("description");
        boost::trim(description);
        tofile = tofile + time + " - " + eventName + ":";
        tofile.push_back('\n');
        tofile.push_back('\n');
        tofile = tofile + description;
        tofile.push_back('\n');
        tofile.push_back('\n');
        tofile.push_back('\n');
        tofile.push_back('\n');
    }
    file << tofile;
    file.close();
    return true;
}
