#include "../include/Summary.h"
#include <iostream>
using std::pair;
using std::string;

Summary::Summary() : updates(), team_a_updates(), team_b_updates(), summaryEvents()
{
    buildUpdates();
    buildUpdatesTeamA();
    buildUpdatesTeamB();
}

void Summary::buildUpdates()
{
    pair<string, string> p1("active", "false");
    updates.insert(p1);
    pair<string, string> p2("before halftime", "false");
    updates.insert(p2);
}

void Summary::buildUpdatesTeamA()
{
    pair<string, string> p1("goals", "0");
    team_a_updates.insert(p1);
    pair<string, string> p2("possession", "0%");
    team_a_updates.insert(p2);
}

void Summary::buildUpdatesTeamB()
{
    pair<string, string> p1("goals", "0");
    team_b_updates.insert(p1);
    pair<string, string> p2("possession", "0%");
    team_b_updates.insert(p2);
}

void Summary::addSummaryEvent(string eventName, string time, string description)
{
    map<string, string> summaryEvent;
    pair<string, string> p1("event name", eventName);
    summaryEvent.insert(p1);
    pair<string, string> p2("time", time);
    summaryEvent.insert(p2);
    pair<string, string> p3("description", description);
    summaryEvent.insert(p3);
    summaryEvents.push_back(summaryEvent);
}

void Summary::addToUpdatesTeamA_goals(string goals)
{
    team_a_updates.at("goals") = goals;
}

void Summary::addToUpdatesTeamA_possession(string possession)
{
    team_a_updates.at("possession") = possession;
}

void Summary::addToUpdatesTeamB_goals(string goals)
{
    team_b_updates.at("goals") = goals;
}

void Summary::addToUpdatesTeamB_possession(string possession)
{
    team_b_updates.at("possession") = possession;
}

void Summary::addToUpdates_active(string active)
{
    updates.at("active") = active;
}

void Summary::addToUpdates_before_halftime(string before_halftime)
{
    updates.at("before halftime") = before_halftime;
}

string Summary::getActive()
{
    return updates.at("active");
}

string Summary::getBeforeHalfTime()
{
    return updates.at("before halftime");
}

string Summary::getTeamA_goals()
{
    return team_a_updates.at("goals");
}

string Summary::getTeamA_possession()
{
    return team_a_updates.at("possession");
}

string Summary::getTeamB_goals()
{
    return team_b_updates.at("goals");
}

string Summary::getTeamB_possession()
{
    return team_b_updates.at("possession");
}

vector<map<string, string>> Summary::getsummaryEvents()
{
    return summaryEvents;
}
