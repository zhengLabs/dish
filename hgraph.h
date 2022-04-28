//
// Created by 七斗米 on 2020/12/24.
//
/*
 * generatorHyperGraph输出一个超图文件，超图文件经过超图分割工具，
 * 输出一个超图分区文件，具有V个顶点的超图的分区文件由V行组成，每行有一个编号
 * 文件的第 i 行包含第 i 个顶点所属的分区号
 * The output of hMETIS is a partition file.
 * The partition file of a hyperGraph with |V| vertices, consists of |V| lines
 * with a single number per line. The ith line of the file contains the
 * partition number that the ith vertex belongs to.
 * */
#include "filesystem"
#include "fstream"
#include "iostream"
#include "map"
#include "ostream"
#include "set"
#include "string"
#include "sys/stat.h"
#include "vector"
using namespace std;

extern vector<string> split(const string &, char);


string writeToHyperGraph(const vector<string> &sum_vec, string traceName,
                         double analysisRatio, int RSm, int &hyperGraphVNums) {
    map<string, int> blockMap;//将顶点的string 映射到int类型的ID上
    int num = 1;
    vector<vector<int>> edgeVec;
    vector<int> tempVec;
    set<string> tempSet;

    /* 获取每个超边集、顶点集
     * 访问流存储在sum_vec中，按照访问顺序形成超图的超边
     * 例如访问流1 2 2 3 4 1
     * 形成的边可以是 1 2 3 4
     * blockWeightMap中存放着每个顶点出现在了多少个超边中
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

    // 设置超边权重
    int end = static_cast<int>(1.5 * RSm);
    int start = 0;
    while (end < sum_vec.size()) {
        set<int> temp;
        for (int j = start; j <= end; j++) temp.insert(blockMap[sum_vec[j]]);
        for (size_t i = 0; i < edgeNums; i++) {// 所有超边
            // 判断该超边中顶点出现在 start～end中的出现的个数
            int count = 0;
            for (size_t j = 0; j < edgeVec[i].size(); j++) {
                count += static_cast<int>(temp.find(edgeVec[i][j]) != temp.end());
            }
            edgeWeightVec[i] += (count - 1) * 2;
        }
        start = end + 1;
        end += RSm;
    }

    if (!std::__fs::filesystem::exists("./hypergraphDir/"))
        std::__fs::filesystem::create_directories("./hypergraphDir/");

    ofstream outfile;
    // 打开一个新文件如果不存在则创建，按照超图文件格式写入
    string filename = "./hypergraphDir/" + traceName + "_" +
                      to_string(analysisRatio) + "_" + to_string(RSm) + "_" +
                      to_string(blockSize);
    outfile.open(filename);
    if (!outfile) {
        cout << "HyperGraph created error!" << endl;
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

    cout << "HyperGraph created successfully！ " << filename << endl;

    outfile.close();
    return filename;
}

string generatorHyperGraph(string IOfile, int RSm, double analysisRatio, int &hyperGraphVNums) {
    char divider = ',';
    string traceName = split(IOfile, '/').back();
    string line;
    ifstream input(IOfile);
    if (!input.good()) {
        std::cout << "access I/O sequence file error" << std::endl;
        return "";
    }

    //  processed I/O file
    vector<string> sum_vec;
    while (getline(input, line)) {
        vector<string> tmp = split(line, divider);
        if (tmp.size() < 5) continue;
        sum_vec.push_back(split(line, divider)[4]);
    }

    int idx = sum_vec.size() * analysisRatio;
    input.close();
    return writeToHyperGraph(
            vector<string>(sum_vec.begin(), sum_vec.begin() + idx - 1), traceName,
            analysisRatio, RSm, hyperGraphVNums);
}
