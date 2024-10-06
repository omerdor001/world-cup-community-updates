#include <algorithm>
#include "../include/StompProtocol.h"
#include "../include/UserTask.h"
#include <boost/algorithm/string.hpp>
using std::pair;
using std::string;

StompProtocol::StompProtocol() : receiptId(0), subId(0), userName(""), summaries(), games(), receipts(), gamesBeforeReceipts() {}

void StompProtocol::split_str(string const &str, const char delim, vector<string> &out)
{
    std::stringstream s(str);
    string str2;
    while (std::getline(s, str2, delim))
    {
        out.push_back(str2);
    }
}

void StompProtocol::split_str_dots(string const &str, vector<string> &out)
{
    std::stringstream s(str);
    string str2;
    while (std::getline(s, str2, ':'))
    {
        out.push_back(str2);
    }
}

string StompProtocol::processToServer(string msg)
{
    const char delimiter = ' ';
    vector<string> out;
    split_str(msg, delimiter, out);
    if (out.at(0).compare("join") == 0)
    {
        string msg;
        string _game = out.at(1);
        string _subId = std::to_string(subId);
        string receiptId_ = std::to_string(receiptId);
        msg = "SUBSCRIBE";
        msg.push_back('\n');
        msg = msg + "destination :/" + _game;
        msg.push_back('\n');
        msg = msg + "id :" + _subId;
        msg.push_back('\n');
        msg = msg + "receipt :";
        msg = msg + receiptId_;
        msg.push_back('\n');
        msg.push_back('\n');
        pair<string, string> p1(receiptId_, "SUBSCRIBE");
        receipts.insert(p1);
        pair<int, string> p2(subId, _game);
        pair<int, pair<int, string>> p3(receiptId, p2);
        gamesBeforeReceipts.insert(p3);
        return msg;
    }
    else if (out.at(0).compare("exit") == 0)
    {
        string msg;
        string receiptId_ = std::to_string(receiptId);
        string _game = out.at(1);
        int pSubId = getSubIDFGame(_game);
        string _subId = std::to_string(pSubId);
        msg = "UNSUBSCRIBE";
        msg.push_back('\n');
        msg = msg + "id :" + _subId;
        msg.push_back('\n');
        msg = msg + "receipt :";
        msg = msg + receiptId_;
        msg.push_back('\n');
        msg.push_back('\n');
        pair<string, string> p1(receiptId_, "UNSUBSCRIBE");
        receipts.insert(p1);
        string temp = "leave_channel";
        pair<int, string> p2(pSubId, temp);
        pair<int, pair<int, string>> p3(receiptId, p2);
        gamesBeforeReceipts.insert(p3);
        return msg;
    }
    else if (out.at(0).compare("logout") == 0)
    {
        string msg;
        string receiptId_ = std::to_string(receiptId);
        msg = "DISCONNECT";
        msg.push_back('\n');
        msg = msg + "receipt :" + receiptId_;
        msg.push_back('\n');
        msg.push_back('\n');
        pair<string, string> p1(receiptId_, "DISCONNECT");
        receipts.insert(p1);
        return msg;
    }
    else
    {
        std::cout << "Unvalid Command \n"
                  << std::endl;
        return nullptr;
    }
}

string StompProtocol::processToServerEvent(string team_a_name, string team_b_name, Event event)
{
    string msg;
    string gameName = team_a_name + "_" + team_b_name;
    msg = "SEND";
    msg.push_back('\n');
    msg = msg + "destination :/" + gameName;
    msg.push_back('\n');
    msg.push_back('\n');
    msg = msg + "user: " + userName;
    msg.push_back('\n');
    msg = msg + "team a: " + team_a_name;
    msg.push_back('\n');
    msg = msg + "team b: " + team_b_name;
    msg.push_back('\n');
    msg = msg + "event name: " + event.get_name();
    msg.push_back('\n');
    msg = msg + "time: " + std::to_string(event.get_time());
    msg.push_back('\n');
    msg = msg + "general game updates:";
    if (event.get_game_updates().size() > 0)
    {

        if ((event.get_game_updates().count("active")) && (event.get_game_updates().at("active").size() > 0))
        {
            msg.push_back('\n');
            msg = msg + "active: " + event.get_game_updates().at("active");
        }
        if ((event.get_game_updates().count("before halftime")) && (event.get_game_updates().at("before halftime").size() > 0))
        {
            msg.push_back('\n');
            msg = msg + "before halftime: " + event.get_game_updates().at("before halftime");
        }
    }
    msg.push_back('\n');
    msg = msg + "team a updates:";
    if (event.get_team_a_updates().size() > 0)
    {
        if ((event.get_team_a_updates().count("goals")) && (event.get_team_a_updates().at("goals").size() > 0))
        {
            msg.push_back('\n');
            msg = msg + "goals: " + event.get_team_a_updates().at("goals");
        }
        if ((event.get_team_a_updates().count("possession")) && (event.get_team_a_updates().at("possession").size() > 0))
        {
            msg.push_back('\n');
            msg = msg + "possession: " + event.get_team_a_updates().at("possession");
        }
    }
    msg.push_back('\n');
    msg = msg + "team b updates:";
    if (event.get_team_b_updates().size() > 0)
    {
        if ((event.get_team_b_updates().count("goals")) && (event.get_team_b_updates().at("goals").size() > 0))
        {
            msg.push_back('\n');
            msg = msg + "goals: " + event.get_team_b_updates().at("goals");
        }
        if ((event.get_team_b_updates().count("possession")) && (event.get_team_b_updates().at("possession").size() > 0))
        {
            msg.push_back('\n');
            msg = msg + "possession: " + event.get_team_b_updates().at("possession");
        }
    }
    msg.push_back('\n');
    msg = msg + "description:";
    msg.push_back('\n');
    msg = msg + event.get_discription();
    return msg;
}

bool StompProtocol::clientResult(string msg)
{
    const char delimiter = '\n';
    vector<string> out;
    split_str(msg, delimiter, out);
    if (out.at(0).compare("RECEIPT") == 0)
    {
        const char delimiter1 = ':';
        vector<string> out1;
        split_str(out.at(1), delimiter1, out1);
        if (receipts.at(out1.at(1)).empty())
        {
            std::cout << "Receipt failed \n"
                      << std::endl;
            return true;
        }
        else if (receipts[out1.at(1)].compare("SUBSCRIBE") == 0)
        {
            if (gamesBeforeReceipts.empty())
            {
                std::cout << "Receipt failed \n"
                          << std::endl;
                return true;
            }
            else if (!gamesBeforeReceipts.empty() & (gamesBeforeReceipts.at(std::stoi(out1.at(1))).first < 0 || gamesBeforeReceipts.at(std::stoi(out1.at(1))).second.empty()))
            {
                std::cout << "Receipt failed \n"
                          << std::endl;
                return true;
            }
            else
            {
                addSubGame(gamesBeforeReceipts.at(std::stoi(out1.at(1))).first, gamesBeforeReceipts.at(std::stoi(out1.at(1))).second);
                receiptId = receiptId + 1;
                subId = subId + 1;
                std::cout << "Subscription succeeded \n"
                          << std::endl;
                return true;
            }
        }
        else if (receipts.at(out1.at(1)).compare("UNSUBSCRIBE") == 0)
        {
            if (gamesBeforeReceipts.empty())
            {
                std::cout << "Receipt failed \n"
                          << std::endl;
                return true;
            }
            else if (!gamesBeforeReceipts.empty() & (gamesBeforeReceipts.at(std::stoi(out1.at(1))).first < 0 || gamesBeforeReceipts.at(std::stoi(out1.at(1))).second.empty()))
            {
                std::cout << "Receipt failed \n"
                          << std::endl;
                return true;
            }
            else
            {
                removeSubGame(gamesBeforeReceipts.at(std::stoi(out1.at(1))).first);
                receiptId = receiptId + 1;
                std::cout << "Exit succeeded \n"
                          << std::endl;
                return true;
            }
        }
        else if (receipts.at(out1.at(1)).compare("DISCONNECT") == 0)
        {
            removeAllSummaries();
            removeAllSubscriptions();
            receiptId = receiptId + 1;
            return false;
        }
        else
            return true;
    }
    else if (out.at(0).compare("MESSAGE") == 0)
    {
        vector<string> out1;
        const char delimiter = '/';
        split_str(out.at(3), delimiter, out1);
        vector<string> out2;
        split_str_dots(out.at(8), out2);
        string username = out2.at(1);
        string gameName = out1.at(1);
        boost::trim(username);
        boost::trim(gameName);
        if (!isSummaryExisted(username, gameName))
        {
            Summary summary;
            addSummary(gameName, username, summary);
        }
        pair<string, string> p1(username, gameName);
        vector<string> out3;
        split_str_dots(out.at(11), out3);
        string eventName = out3.at(1);
        vector<string> out4;
        split_str_dots(out.at(12), out4);
        string time = out4.at(1);
        int numOfLines = out.size();
        string description = out.at(numOfLines - 1);
        summaries.at(p1).addSummaryEvent(eventName, time, description);
        vector<string> reports(out.begin() + 13, out.end() - 2);
        auto it1 = std::find(reports.begin(), reports.end(), "team a updates:");
        auto index1 = std::distance(reports.begin(), it1);
        auto it2 = std::find(reports.begin(), reports.end(), "team b updates:");
        auto index2 = std::distance(reports.begin(), it2);
        int size1 = reports.size() - index1;
        int size2 = reports.size() - index2;
        vector<string> reports_all(reports.begin() + 1, reports.end() - size1);
        vector<string> reports_teamA(reports.begin() + index1 + 1, reports.end() - size2);
        vector<string> reports_teamB(reports.begin() + index2 + 1, reports.end());
        if (reports_all.size() == 2)
        {
            vector<string> out1;
            split_str_dots(reports_all.at(0), out1);
            summaries.at(p1).addToUpdates_active(out1.at(1));
            vector<string> out2;
            split_str_dots(reports_all.at(1), out2);
            summaries.at(p1).addToUpdates_before_halftime(out2.at(1));
        }
        else if (reports_all.size() == 1)
        {
            vector<string> out1;
            split_str_dots(reports_all.at(0), out1);
            if (out1.at(0) == "active")
            {
                summaries.at(p1).addToUpdates_active(out1.at(1));
            }
            else
            {
                summaries.at(p1).addToUpdates_before_halftime(out1.at(1));
            }
        }
        else
        {
        }

        if (reports_teamA.size() == 2)
        {
            vector<string> out3;
            split_str_dots(reports_teamA.at(0), out3);
            summaries.at(p1).addToUpdatesTeamA_goals(out3.at(1));
            vector<string> out4;
            split_str_dots(reports_teamA.at(1), out4);
            summaries.at(p1).addToUpdatesTeamA_possession(out4.at(1));
        }
        else if (reports_teamA.size() == 1)
        {
            vector<string> out3;
            split_str_dots(reports_teamA.at(0), out3);
            if (out3.at(0) == "goals")
            {
                summaries.at(p1).addToUpdatesTeamA_goals(out3.at(1));
            }
            else
            {
                summaries.at(p1).addToUpdatesTeamA_possession(out3.at(1));
            }
        }
        else
        {
        }

        if (reports_teamB.size() == 2)
        {
            vector<string> out3;
            split_str_dots(reports_teamB.at(0), out3);
            summaries.at(p1).addToUpdatesTeamB_goals(out3.at(1));
            vector<string> out4;
            split_str_dots(reports_teamB.at(1), out4);
            summaries.at(p1).addToUpdatesTeamB_possession(out4.at(1));
        }
        else if (reports_teamB.size() == 1)
        {
            vector<string> out3;
            split_str_dots(reports_teamB.at(0), out3);
            if (out3.at(0) == "goals")
            {
                summaries.at(p1).addToUpdatesTeamB_goals(out3.at(1));
            }
            else
            {
                summaries.at(p1).addToUpdatesTeamB_possession(out3.at(1));
            }
        }
        else
        {}
        return true;
    }
    else if (out.at(0).compare("ERROR") == 0)
    {
        string errorMessage = out.at(out.size()-1);
        std::cout << "Error: " + msg + "\n"
                  << std::endl;
        return false;
    }
    else
        return true;
}


void StompProtocol::addSummary(string gameName, string sendUsername, Summary summary)
{
    pair<string, string> p1(sendUsername, gameName);
    pair<pair<string, string>, Summary> p2(p1, summary);
    summaries.insert(p2);
}

void StompProtocol::addSubGame(int subId, string game)
{
    pair<int, string> p1(subId, game);
    games.insert(p1);
}

void StompProtocol::removeSubGame(int subId)
{
    games.erase(subId);
}

bool StompProtocol::isSubExist(string game)
{
    for (const string &_game : getAllValuesGames())
    {
        if (_game == game)
            return true;
    }
    return false;
}

bool StompProtocol::isSummaryExisted(string username, string gameName)
{
    pair<string, string> p1(username, gameName);
    for (const pair<string, string> &_p : getAllKeysSummaries())
    {
        if (_p.first.compare(p1.first) == 0 && _p.second.compare(p1.second) == 0)
            return true;
        else
            return false;
    }
    return false;
}

vector<int> StompProtocol::getAllKeysGames()
{
    std::vector<int> keys;
    for (std::map<int, string>::iterator it = games.begin(); it != games.end(); ++it)
    {
        keys.push_back(it->first);
    }
    return keys;
}

vector<pair<string, string>> StompProtocol::getAllKeysSummaries()
{
    vector<pair<string, string>> keys;
    for (std::map<pair<string, string>, Summary>::iterator it = summaries.begin(); it != summaries.end(); ++it)
    {
        keys.push_back(it->first);
    }
    return keys;
}

vector<string> StompProtocol::getAllValuesGames()
{
    std::vector<string> values;
    for (std::map<int, string>::iterator it = games.begin(); it != games.end(); ++it)
    {
        values.push_back(it->second);
    }
    return values;
}

Summary StompProtocol::getSummaryPerUserGame(string username, string gameName)
{
    pair<string, string> p1(username, gameName);
    return summaries.at(p1);
}

int StompProtocol::getSubIDFGame(string gameName)
{
    for (const int &_subId : getAllKeysGames())
    {
        if (games.at(_subId).compare(gameName)==0)
            return _subId;
    }
    return -1;
}

void StompProtocol::updateUserName(string username)
{
    userName = username;
}

void StompProtocol::removeAllSubscriptions()
{
    if (!games.empty())
    {
        games.clear();
    }
}

void StompProtocol::removeAllSummaries()
{
    if (summaries.empty())
    {
        summaries.clear();
    }
}
