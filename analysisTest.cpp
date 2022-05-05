#include <analysis.h>
#include <hgraph.h>
#include <unistd.h>
#include <util.h>
int RS_m;
int RS_n;

const string ROOTPATH = "/Users/joseph/work_dir/cpp_dir/dish/";
const int TIMESTAMP = 20;
auto kc = KahyparConfig(ROOTPATH + "kahypar.exe",
                        "0.3",
                        "direct",
                        "cut",
                        ROOTPATH + "cut_kKaHyPar_sea20.ini");

int main(int argc, char *argv[]) {

    int hyperGraphVNums = 0;
    string IOfile;
    vector<string> sumVec; // Logical Block Address
    vector<string> readVec;// LBAs for read requests
    vector<vector<string>> eachTimestampVec;
    string hyperGraph = "";
    string partitionFile = "";

    if (!judgeParam(argc, argv, RS_m, RS_n, IOfile))
        exit(-1);

    processIOfile(IOfile, TIMESTAMP, sumVec, readVec, eachTimestampVec);
    if (sumVec.empty()) {
        cout << "ERROR: process file " << IOfile << " failed !" << endl;
        exit(-1);
    }

    // Generating hypergraphs based on access sequences
    generatorHyperGraph(sumVec, IOfile, RS_m, RS_n, hyperGraphVNums, hyperGraph);
    if (hyperGraph == "") {
        cout << "ERROR: HyperGraph file created faild!" << endl;
        exit(-1);
    }

    // Partitioning for hypergraph
    hyperGraphpartition(kc, hyperGraph, hyperGraphVNums, RS_m, partitionFile);
    if (partitionFile == "") {
        cout << "ERROR: HyperGraph partition file created faild!" << endl;
        exit(-1);
    }
    cout << "create HyperGraph partition file : " << partitionFile << endl;

    // Organising stripe based on hypergraph partition
    vector<int> hgVec;// read processpartitionFile to vector
    int numsOfpartition = processpartitionFile(partitionFile, hgVec);
    if (numsOfpartition == 1) {
        cout << "ERROR: process partition file failed ! " << endl;
        exit(-1);
    };

    analysisDISH(sumVec, hgVec, numsOfpartition, readVec, eachTimestampVec);// DISH
    analysisBSO(sumVec, readVec, eachTimestampVec);                         // BSO Baseline Strip Organisation
    analysisCASO(sumVec, readVec, eachTimestampVec);                        // CASO
    return 0;
}