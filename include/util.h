#ifndef UTIL_DISH_H
#define UTIL_DISH_H
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

bool judgeParam(int argc, char *argv[], int &RS_m, int &RS_n, std::string &IOfile);

vector<string> split(const string &str, char c);

int processpartitionFile(string partitionFile, vector<int> &hgVec);

#endif