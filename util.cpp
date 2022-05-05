#include "util.h"

bool judgeParam(int argc, char *argv[], int &RS_m, int &RS_n, std::string &IOfile) {
    if (argc != 4) {
        cout << "ERROR : Required Parameter IO sequence file, RS m ,RS n, time_stamp"
             << endl;
        return false;
    }
    IOfile = std::string(argv[1]);
    RS_m = atoi(argv[2]);
    RS_n = atoi(argv[3]);


    if (!std::__fs::filesystem::exists(IOfile)) {
        std::cout << "ERROR : file " << IOfile << " not exist ! " << std::endl;
        return false;
    }

    return true;
}

vector<string> split(const string &str, char c) {
    string::size_type pos1, pos2;
    vector<string> vec;
    pos1 = 0;
    pos2 = str.find(c);

    while (string::npos != pos2) {
        if (pos1 != pos2) vec.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + 1;
        pos2 = str.find(c, pos1);
    }

    if (pos1 != str.length()) {
        vec.push_back(str.substr(pos1));
    }
    return vec;
}


int processpartitionFile(string partitionFile, vector<int> &hgVec) {
    int n = 0;
    string line;
    ifstream hGraph(partitionFile);
    if (hGraph) {
        while (getline(hGraph, line)) {
            if (line != " ") {
                hgVec.push_back(stoi(line));
                if (stoi(line) > n) {
                    n = stoi(line);
                }
            }
        }
        hGraph.close();
    }

    return n + 1;// 分区从0开始，所以+1
}