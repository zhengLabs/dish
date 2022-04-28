#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
using namespace std;

int judgeParam(int argc, char *argv[], int &RS_m, int &RS_n, int &time_stamp,
               double &analysisRatio, std::string &IOfile) {
    if (argc != 6) {
        cout << "Required Parameter: IO sequence file, RS m ,RS n ,"
                "Analysis rate, time_stamp"
             << endl;
        return 0;
    }
    IOfile = std::string(argv[1]);
    RS_m = atoi(argv[2]);
    RS_n = atoi(argv[3]);
    time_stamp = atoi(argv[5]);
    analysisRatio = atof(argv[4]);

    std::ifstream file(IOfile);
    if (!file.good()) {
        std::cout << "IO sequence file not exist" << std::endl;
        return 0;
    }
    if (analysisRatio < 0.2 || analysisRatio > 1) {
        std::cout << "Analysis rate between 0.2 and 1 (including 0.2, 1)"
                  << std::endl;
        return 0;
    }

    if (time_stamp < 1) {
        std::cout << "time_stamp should greater than 0" << std::endl;
        return 0;
    }

    return 1;
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


struct KahyparConfig {
    string exePath;
    string epsilon;// Imbalance parameter epsilon
    string partitioningMode;
    string objective;
    string configPath;

    KahyparConfig(string p1, string p2, string p3, string p4, string p5)
        : exePath(p1), epsilon(p2), partitioningMode(p3), objective(p4), configPath(p5) {}
};
string hyperGraphPartation(KahyparConfig kc, string hyperGraph, int hyperGraphVNums, int RS_m) {
    pid_t pid;
    int st;
    if ((pid = fork()) == 0) {
        execl(kc.exePath.c_str(), "kahypar",
              "-h", hyperGraph.c_str(),
              "-k", to_string(hyperGraphVNums / RS_m).c_str(),
              "-e", kc.epsilon.c_str(),
              "-o", kc.objective.c_str(),
              "-m", kc.partitioningMode.c_str(),
              "-p", kc.configPath.c_str(),
              "-w", "1",
              nullptr);
        exit(1);
    } else if (pid < 0) {
        cout << "fork hahypar error!" << endl;
    }

    int ret = wait(&st);//等待子进程结束
    if (ret == -1) {
        cout << " kahypar partation error!" << endl;
    }

    string partationFile = hyperGraph + ".part" + to_string(hyperGraphVNums / RS_m) + ".epsilon0.03.seed-1.KaHypar";
    cout << "HyperGraph partation file generator successful ! "
         << endl
         << partationFile << endl;
    return partationFile;
}