#ifndef HGRAPH_H
#define HGRAPH_H
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
using namespace std;

extern vector<string> split(const string &, char);

string writeToHyperGraph(const vector<string> &sum_vec, string traceName,
                         int RSm, int &hyperGraphVNums);

void generatorHyperGraph(const vector<string> &sum_vec, string IOfile,
                         int RSm, int RSn, int &hyperGraphVNums, string &hyperGraph);

struct KahyparConfig {
    string exePath;
    string epsilon;// Imbalance parameter epsilon
    string partitioningMode;
    string objective;
    string configPath;

    KahyparConfig(string p1, string p2, string p3, string p4, string p5)
        : exePath(p1), epsilon(p2), partitioningMode(p3), objective(p4), configPath(p5) {}
};
void hyperGraphpartition(KahyparConfig kc, string hyperGraph,
                         int hyperGraphVNums, int RS_m, string &partitionFile);
#endif