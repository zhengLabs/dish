//
// Created by 七斗米 on 2021/5/20.
//

#include "analysis.h"
#include "hgraph.h"
#include "unistd.h"
#include "util.h"
int RS_m;
int RS_n;

auto kc = KahyparConfig("../kahypar.exe", "0.3", "direct", "cut", "../kahypar/config/cut_kKaHyPar_sea20.ini");

int main(int argc, char *argv[]) {

    int time_stamp;
    double analysisRatio;
    int hyperGraphVNums = 0;
    string IOfile;

    // 参数判断
    if (!judgeParam(argc, argv, RS_m, RS_n, time_stamp, analysisRatio, IOfile))
        exit(-1);

    // 生成超图
    string hyperGraph = generatorHyperGraph(IOfile, RS_m, analysisRatio, hyperGraphVNums);
    if (hyperGraph == "") {
        exit(-1);
    }

    // 超图分区
    string partationFile = hyperGraphPartation(kc, hyperGraph, hyperGraphVNums, RS_m);


    // 预处理访问序列
    vector<string> sumVec; // 访问序列中的LBA集合
    vector<string> readVec;// 访问序列中read请求的LBA集合
    processIOfile(IOfile, sumVec, readVec);


    // // 不同条带组织算法
    // cout << "analysisRatio :" << analysisRatio << endl;
    // analysisBSO(sumVec, readVec, analysisRatio, time_stamp);
    // analysisCASO(sumVec, readVec, analysisRatio, time_stamp);

    // // DISH
    // vector<int> hgVec;
    // int n = 0;// n是分区数，由于是从分区0开始，所以传入DISH的数就是n+1

    // string line2;
    // if (hGraph) {
    //     while (getline(hGraph, line2)) {
    //         if (line2 != " ") {
    //             hgVec.push_back(stoi(line2));
    //             if (stoi(line2) > n) {
    //                 n = stoi(line2);
    //             }
    //         }
    //     }
    //     cout << "被分割之后的超图处理完毕！" << endl;
    // } else {
    //     cout << "no such hyperGraph partation file" << endl;
    // }

    // // DISH 超图分割算法
    // analysisDISH(sumVec, hgVec, n + 1, readVec, analysisRatio,
    //              time_stamp);

    // input.close();
    // hGraph.close();
    return 0;
}