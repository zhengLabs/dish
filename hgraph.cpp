#include "hgraph.h"

string writeToHyperGraph(const vector<string> &sum_vec, string traceName,
                         int RSm, int RSn, int &hyperGraphVNums) {
    map<string, int> blockMap;// Mapping vertex and vertexID
    int num = 1;
    vector<vector<int>> edgeVec;
    vector<int> tempVec;
    set<string> tempSet;

    /* Get each hyperedges, vertexs
     * For example access to streams 1 2 2 3 4 1
     * The hyperedges formed can be 1 2 3 4
     */
    for (const string &item : sum_vec) {
        if (blockMap.find(item) == blockMap.end()) {//顶点集
            blockMap.insert({item, num++});
        }

        if (tempSet.find(item) == tempSet.end()) {
            tempVec.push_back(blockMap.at(item));
            tempSet.insert(item);
        }

        if (tempVec.size() == RSm) {//超边集
            edgeVec.push_back(tempVec);
            tempVec.clear();
            tempSet.clear();
        }
    }

    int blockSize = blockMap.size();
    hyperGraphVNums = blockSize;
    int edgeNums = edgeVec.size();
    vector<int> edgeWeightVec(edgeNums, 0);

    // Set hypergraph weights
    int end = static_cast<int>(2 * RSm);
    int start = 0;
    while (end < sum_vec.size()) {
        set<int> temp;
        for (int j = start; j <= end; j++) temp.insert(blockMap[sum_vec[j]]);
        for (size_t i = 0; i < edgeNums; i++) {// for all hyperedges
            // Gets the number of  vertexs from start to end, AS edge weight
            int count = 0;
            for (size_t j = 0; j < edgeVec[i].size(); j++) {
                count += static_cast<int>(temp.find(edgeVec[i][j]) != temp.end());
            }
            //对于每一个时间窗口的数据块子集，在所有超边中出现的个数之和作为超边权重
            edgeWeightVec[i] += ((count - 1) * 2) > 0 ? (count - 1) * 2 : 0;
        }
        start = end + 1;
        end += RSm;
    }

    if (!std::__fs::filesystem::exists("./hypergraphDir/"))
        std::__fs::filesystem::create_directories("./hypergraphDir/");

    ofstream outfile;
    string filename = "./hypergraphDir/" + traceName + "_" +
                      to_string(RSm) + "_" + to_string(RSn);
    outfile.open(filename);
    if (!outfile) {
        return "";
    }
    outfile << edgeNums << " " << blockSize << " " << 1 << endl;
    for (int i = 0; i < edgeNums; i++) {
        outfile << edgeWeightVec[i];
        for (int elem : edgeVec[i]) {
            outfile << " " << elem;
        }
        outfile << endl;
    }

    cout << "create hypergraph file : " << filename << endl;

    outfile.close();
    return filename;
}

/*
 * The output of generatorHyperGraph is a hypergraph file，
 * use the hMetis format for the input hypergraph file as well as the partition output file..
 * */
void generatorHyperGraph(const vector<string> &sum_vec, string IOfile,
                         int RSm, int RSn, int &hyperGraphVNums, string &hyperGraph) {

    string traceName = split(IOfile, '/').back();
    hyperGraph = writeToHyperGraph(sum_vec, traceName, RSm, RSn, hyperGraphVNums);
}


/*
 * The output of hyperGraphpartition is a partition file.
 * The partition file of a hyperGraph with |V| vertices, consists of |V| lines
 * with a single number per line. The ith line of the file contains the
 * partition number that the ith vertex belongs to.
 * */

void hyperGraphpartition(KahyparConfig kc, string hyperGraph,
                         int hyperGraphVNums, int RS_m, string &partitionFile) {
    partitionFile = hyperGraph + ".part" + to_string(hyperGraphVNums / RS_m) +
                    ".epsilon" + kc.epsilon + ".seed-1.KaHypar";
    pid_t pid;
    int st;
    if ((pid = fork()) == 0) {
        int fd;
        string logName = partitionFile + ".log";
        fd = open(logName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            cout << "ERROR: open " << logName << " failed !" << endl;
            exit(-1);
        }
        cout << "create hypergraph partition log file : " + logName << endl;
        dup2(fd, STDOUT_FILENO);// Redirect stdout to the file pointed to by fd
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

    int ret = wait(&st);// Waiting for a child process to finish
    if (ret == -1) {
        cout << " kahypar partition error!" << endl;
    }

    if (!std::__fs::filesystem::exists(partitionFile)) {
        partitionFile = "";// Failed to create hypergraph partition file
    }
}