#ifndef ANALYSISTEST_H
#define ANALYSISTEST_H
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <unistd.h>
#include <unordered_set>
#include <vector>

using namespace std;
extern int RS_m;
extern int RS_n;
extern vector<string> split(const string &, char);

void getEachTimestampVec(const vector<string> &sum_vec, int time_stamp, vector<vector<string>> &vec_each_timestamp);

void processIOfile(string IOfile, int time_stamp, vector<string> &sumVec, vector<string> &readVec, vector<vector<string>> &vec_each_timestamp);


vector<vector<string>> getStripeRandom(const vector<string> &sum_vec);

int getIOCost(const vector<vector<string>> &vec_each_timestamp, const vector<vector<string>> &stripes, const map<string, int> &blockStripeMap);

void addEndStripe(vector<vector<string>> &finalStripe, vector<string> &sum_second);

void analysisBSO(const vector<string> &sum_vec, const vector<string> &read_vec, const vector<vector<string>> &vec_each_timestamp);

void addWeightToGraph(vector<vector<int>> &matrixWeight, vector<int> &infectVec);

void updateMaxStack(stack<pair<int, int>> &maxStack, vector<vector<int>> &matrix, int i, vector<bool> &flag);

void push_back_stripe(vector<string> &resultTemp, vector<vector<string>> &finalStripe, string block, int index, vector<bool> &flag, stack<pair<int, int>> &maxStack, vector<vector<int>> &matrixWeight);

void analysisCASO(const vector<string> &sum_vec, const vector<string> &read_vec, const vector<vector<string>> &vec_each_timestamp);


void analysisDISH(const vector<string> &sum_vec, const vector<int> &ghVec, int n, const vector<string> &read_vec, const vector<vector<string>> &vec_each_timestamp);

#endif// ANALYSISTEST_H
