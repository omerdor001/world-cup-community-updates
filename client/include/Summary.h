#pragma once
#include "../include/event.h"
#include <iostream>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;


class Summary {
public:
Summary();
void buildUpdates();
void buildUpdatesTeamA();
void buildUpdatesTeamB();
void addToUpdatesTeamA_goals(string goals);
void addToUpdatesTeamA_possession(string possession);
void addToUpdatesTeamB_goals(string goals);
void addToUpdatesTeamB_possession(string possession);
void addToUpdates_active(string active);
void addToUpdates_before_halftime(string before_halftime);
void addSummaryEvent(string eventName,string time,string description);
string getActive();
string getBeforeHalfTime();
string getTeamA_goals();
string getTeamA_possession();
string getTeamB_goals();
string getTeamB_possession();
vector<map<string, string>> getsummaryEvents();


private:
map<string, string> updates;
map<string, string> team_a_updates;
map<string, string> team_b_updates;
vector<map<string, string>> summaryEvents;

};