//
// Created by 七斗米 on 2021/5/20.
//

#ifndef ANALYSISTEST_H
#define ANALYSISTEST_H
#include "fstream"
#include "iostream"
#include "map"
#include "set"
#include "stack"
#include "unordered_set"
#include "vector"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

using namespace std;
extern int RS_m;
extern vector<string> split(const string &, char);


void processIOfile(string IOfile, vector<string> &sumVec, vector<string> &readVec) {
    char divider = ',';
    ifstream input(IOfile);
    string line;

    while (getline(input, line)) {
        vector<string> tmp = split(line, divider);
        sumVec.push_back(tmp[4]);
        if (tmp.size() >= 4 && tmp[3] == "Read") {
            readVec.push_back(tmp[4]);
        }
    }
    cout << "------------------------------------I/O file loaded!------------------------------------" << endl;
    
}

vector<vector<string>> get_vec_each_timestamp(const vector<string> &sum_vec,
                                              int time_stamp) {
    //每个时间戳内的数据块
    vector<vector<string>> vec_each_timestamp;
    vector<string> vec;
    auto it = sum_vec.begin();
    int i = 0;
    while (it != sum_vec.end()) {
        vec.push_back(*it);
        if (++i > time_stamp) {
            i = 0;
            vec_each_timestamp.push_back(vec);
            vec.clear();
        }
        it++;
    }
    if (!vec.empty()) {
        vec_each_timestamp.push_back(vec);
    }
    return vec_each_timestamp;
}

vector<vector<string>> getNormalStripe(const vector<string> &sum_vec) {
    //原始随机条带组织方法
    unordered_set<string> blockSet;
    vector<vector<string>> stripeRandom;
    vector<string> tempStringVec;
    for (auto elem : sum_vec) {//获得所有数据块集合
        if (blockSet.find(elem) == blockSet.end()) {
            blockSet.insert(elem);
        }
    }
    auto it = blockSet.begin();
    while (it != blockSet.end()) {//将数据块”随机“分配到不同条带中
        tempStringVec.push_back(*it);
        if (tempStringVec.size() == RS_m) {
            stripeRandom.push_back(tempStringVec);
            tempStringVec.clear();
        }
        it++;
    }
    return stripeRandom;
}

int getInfectStripeNums(const vector<vector<string>> &vec_each_timestamp,
                        const vector<vector<string>> &stripeString) {
    int io_count = 0;
    for (int i = 0; i < vec_each_timestamp.size(); ++i) {
        vector<bool> flag(stripeString.size());
        //在每个时间间隔的访问序列里
        set<string> tempSet;
        for (string item : vec_each_timestamp[i]) {
            //对于一个时间间隔内访问的每个 数据块
            if (tempSet.find(item) == tempSet.end()) {
                tempSet.insert(item);
            }
        }
        for (int j = 0; j < stripeString.size(); ++j) {
            for (string item : stripeString[j]) {
                if (tempSet.find(item) != tempSet.end()) {
                    flag[j] = 1;
                    break;
                }
            }
        }
        for (int i = 0; i < flag.size(); ++i) {
            if (flag[i]) io_count++;
        }
    }
    cout << "small-write涉及到的条带:" << io_count << endl;
    return io_count * RS_m;
}

void addEndStripe(vector<vector<string>> &finalStripe,
                  vector<string> &sum_second) {
    unordered_set<string> blockSet;
    unordered_set<string> blockSet_before;
    for (auto elem : finalStripe) {
        for (auto item : elem) {
            if (blockSet.find(item) == blockSet.end()) {
                blockSet_before.insert(item);
            }
        }
    }

    for (auto elem : sum_second) {//获得第二部分所有数据块集合
        if (blockSet.find(elem) == blockSet.end() &&
            blockSet_before.find(elem) == blockSet_before.end()) {
            blockSet.insert(elem);
        }
    }
    auto it = blockSet.begin();
    vector<string> temp;
    while (finalStripe.back().size() != RS_m) {
        finalStripe.back().push_back(*it);
        it++;
    }
    while (it != blockSet.end()) {
        temp.push_back(*it);
        if (temp.size() == RS_m) {
            finalStripe.push_back(temp);
            temp.clear();
        }
        it++;
    }
    if (!temp.empty()) {
        finalStripe.push_back(temp);
    }
}

void analysisBSO(vector<string> &sum_vec, vector<string> &read_vec,
                 double analysisRatio, int time_stamp) {//对Base方法的条带进行性能评估
    int sum_analysis_end = sum_vec.size() * analysisRatio;
    vector<string> sum_first(sum_vec.begin(),
                             sum_vec.begin() + sum_analysis_end - 1);
    vector<string> sum_second(sum_vec.begin() + sum_analysis_end,
                              sum_vec.end() - 1);

    vector<vector<string>> vec_each_timestamp =
            get_vec_each_timestamp(sum_second, time_stamp);

    vector<vector<string>> finalStripe = getNormalStripe(sum_first);
    addEndStripe(finalStripe, sum_second);

    cout << "------------------------------------BSO------------------------------------" << endl;
    cout << "BSO stripeString size:" << finalStripe.size() << endl;

    cout << "总共需要的IO次数:"
         << getInfectStripeNums(vec_each_timestamp, finalStripe) << endl;
}

void addWeight(vector<vector<int>> &matrixWeight, vector<int> &infectVec) {
    for (int i = 0; i < infectVec.size(); ++i) {
        for (int j = i + 1; j < infectVec.size(); ++j) {
            matrixWeight[infectVec[i]][infectVec[j]]++;
        }
    }
}
void updateMaxStack(stack<pair<int, int>> &maxStack,
                    vector<vector<int>> &matrix, int i, vector<bool> &flag) {
    for (int j = 0; j < matrix[i].size(); ++j) {
        if (!flag[j] && matrix[i][j] > 0) {
            if (maxStack.empty()) {
                maxStack.push({j, matrix[i][j]});
            } else {
                if (matrix[i][j] > maxStack.top().second) {
                    maxStack.push({i, matrix[i][j]});
                } else {
                    stack<pair<int, int>> tempStack;
                    while (matrix[i][j] > maxStack.top().second) {
                        tempStack.push(maxStack.top());
                        maxStack.pop();
                    }
                    maxStack.push({j, matrix[i][j]});
                    while (!tempStack.empty()) {
                        maxStack.push(tempStack.top());
                        tempStack.pop();
                    }
                }
            }
        }
    }
}
void push_back_stripe(vector<string> &resultTemp,
                      vector<vector<string>> &finalStripe, string block,
                      int index, vector<bool> &flag,
                      stack<pair<int, int>> &maxStack,
                      vector<vector<int>> &matrixWeight) {
    if (!flag[index]) {
        resultTemp.push_back(block);
        flag[index] = true;
        updateMaxStack(maxStack, matrixWeight, index, flag);
    } else {
        if (!maxStack.empty()) {
            maxStack.pop();
        }
    }
}

void analysisCASO(vector<string> &sum_vec, vector<string> &read_vec,
                  double analysisRatio, int time_stamp) {
    int sum_analysis_end = sum_vec.size() * analysisRatio;
    vector<string> sum_first(sum_vec.begin(),
                             sum_vec.begin() + sum_analysis_end - 1);
    vector<string> sum_second(sum_vec.begin() + sum_analysis_end,
                              sum_vec.end() - 1);
    vector<vector<string>> coarseStripe;
    vector<string> tempVec;
    //从访问序列中获取数据块集
    map<string, int> blockMap;
    vector<string> blockVec;
    int i_block = 0;

    for (const string &elem : sum_first) {
        tempVec.push_back(elem);
        if (blockMap.find(elem) == blockMap.end()) {
            blockVec.push_back(elem);
            blockMap.insert({elem, i_block++});
        }
        if (tempVec.size() == RS_m) {
            coarseStripe.push_back(tempVec);
            tempVec.clear();
        }
    }
    //权重集，vecWeight[0]的大小代表数据块0~1之间的权重
    vector<vector<int>> matrixWeight(blockVec.size(),
                                     vector<int>(blockVec.size()));

    //处理过的条带
    vector<vector<string>> nextStripe;
    vector<int> intTempVec;
    set<string> stringTempSet;
    for (const string elem : sum_first) {//对于访问序列

        // 生成无向有权图
        if (stringTempSet.find(elem) == stringTempSet.end()) {
            intTempVec.push_back(blockMap.at(elem));
            stringTempSet.insert(elem);
        }
        if (intTempVec.size() == RS_m) {
            addWeight(matrixWeight, intTempVec);
            intTempVec.clear();
            stringTempSet.clear();
        }
    }

    //贪心分割获得最终条带
    vector<vector<string>> finalStripe;
    vector<bool> flag(blockVec.size());
    vector<string> resultTemp;
    /*
     * 循环所有数据块，对每个数据块
     *      将数据块加入临时stripe中，更新最大栈
     *      从最大栈中取出元素加入临时stripe
     *      再更新最大栈，从最大栈中取出元素放入
     *    直到临时stripe的size ==
     * block_and_parity_nums，将临时stripe放入finalStripe中，在清空临时stripe
     *
     * */
    for (int i = 0; i < blockVec.size(); ++i) {
        if (flag[i]) continue;
        stack<pair<int, int>> maxStack;//{i,weight}
        push_back_stripe(resultTemp, finalStripe, blockVec[i], i, flag,
                         maxStack, matrixWeight);
        //获取最大权重的键值对
        while (maxStack.size() > 0 &&
               resultTemp.size() < RS_m) {//最大栈中有元素

            push_back_stripe(
                    resultTemp, finalStripe, blockVec[maxStack.top().first],
                    maxStack.top().first, flag, maxStack, matrixWeight);
        }
        if (resultTemp.size() >= RS_m) {
            finalStripe.push_back(resultTemp);
            resultTemp.clear();
        }
    }
    addEndStripe(finalStripe, sum_second);

    cout << "-------------------CASO---------------------" << endl;

    cout << "CASO finalStripe size:" << finalStripe.size() << endl;

    vector<vector<string>> vec_each_timestamp =
            get_vec_each_timestamp(sum_second, time_stamp);
    cout << "ioCountDISH:"
         << getInfectStripeNums(vec_each_timestamp, finalStripe) << endl;
}
//----------------分割线-----------------
void analysisDISH(vector<string> &sum_vec, const vector<int> &ghVec, int n,
                  vector<string> &read_vec, double analysisRatio,
                  int time_stamp) {
    int sum_analysis_end = sum_vec.size() * analysisRatio;
    vector<string> sum_first(sum_vec.begin(),
                             sum_vec.begin() + sum_analysis_end - 1);
    vector<string> sum_second(sum_vec.begin() + sum_analysis_end,
                              sum_vec.end() - 1);

    vector<vector<int>> coarseStripe(n);
    //被分割的超图中，第1行的值v 是指数据块1 数据第v个分区
    for (int i = 1; i < ghVec.size(); ++i) {
        coarseStripe[ghVec[i]].push_back(i);
    }
    //将条带整理到每一个条带的数据块数量相同
    vector<int> tempVec;
    for (vector<int> &elem : coarseStripe) {
        while (elem.size() > RS_m) {
            tempVec.push_back(elem.back());
            elem.pop_back();
        }
    }
    int index = 0;
    for (vector<int> &elem : coarseStripe) {
        if (index >= tempVec.size()) break;
        while (elem.size() < RS_m) {
            if (index >= tempVec.size()) break;
            elem.push_back(tempVec[index++]);
        }
    }
    //将条带中的数字映射为数据块
    map<string, int> blockMap;
    int num = 1;
    //获取映射关系
    for (const string &item : sum_first) {
        if (blockMap.find(item) == blockMap.end()) {//顶点集
            blockMap.insert({item, num++});
        }
    }
    //获取 数字->数据块编号 之间的映射关系
    map<int, string> reverseMap;
    auto map_it = blockMap.cbegin();
    while (map_it != blockMap.cend()) {
        reverseMap.insert({map_it->second, map_it->first});
        map_it++;
    }

    //恢复之后的原数据条带

    vector<vector<string>> finalStripe;
    vector<string> tempVecStr;
    for (vector<int> elem : coarseStripe) {
        for (int item : elem) {
            tempVecStr.push_back(reverseMap[item]);
        }
        finalStripe.push_back(tempVecStr);
        tempVecStr.clear();
    }
    addEndStripe(finalStripe, sum_second);

    cout << "---------------DISH----------------" << endl;
    cout << "DISH final stripe size:" << finalStripe.size() << endl;

    vector<vector<string>> vec_each_timestamp =
            get_vec_each_timestamp(sum_second, time_stamp);

    cout << "ioCountDISH:"
         << getInfectStripeNums(vec_each_timestamp, finalStripe) << endl;
}

#endif// ANALYSISTEST_H
