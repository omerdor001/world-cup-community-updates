#pragma once
#include "../include/Summary.h"
#include "ConnectionHandler.h"

using std::pair;
using std::string;

class StompProtocol
{
public:
    StompProtocol();
    string processToServer(string msg);
    string processToServerEvent(string team_a_name, string team_b_name, Event event);
    bool clientResult(string msg);
    void addSummary(string gameName, string sendUsername, Summary summary);
    void addSubGame(int subId, string game);
    void removeSubGame(int subId);
    bool isSubExist(string game);
    bool isSummaryExisted(string username,string game);
    Summary getSummaryPerUserGame(string username, string gameName);
    std::string getUsername();
    vector<string> getAllValuesGames();
    vector<Summary> getAllValuesSummaries();
    int getSubIDFGame(string gameName);
    vector<int> getAllKeysGames();
    vector<pair<string, string>> getAllKeysSummaries();
    void updateUserName(string username);
    void removeAllSummaries();
    void removeAllSubscriptions();

private:
    int receiptId;
    int subId;
    string userName;
    std::map<pair<string, string>, Summary> summaries; //username,gameName
    std::map<int, string> games;
    std::map<string, string> receipts;
    std::map<int, pair<int, string>> gamesBeforeReceipts;
    void split_str(string const &str, const char delim, vector<string> &out);
    void split_str_dots(string const &str, vector<string> &out);
};
