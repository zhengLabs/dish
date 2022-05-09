#include <analysis.h>

void getEachTimestampVec(const vector<string> &sum_vec,
                         int time_stamp,
                         vector<vector<string>> &eachTimestampVec) {
    vector<string> vec;
    auto it = sum_vec.begin();
    int i = 0;
    while (it + time_stamp < sum_vec.end()) {
        eachTimestampVec.emplace_back(vector<string>(it, it + time_stamp));
        it = it + time_stamp;
    }
    if (it < sum_vec.end()) {
        eachTimestampVec.emplace_back(vector<string>(it, sum_vec.end()));
    }
}

void processIOfile(string IOfile,
                   int time_stamp,
                   vector<string> &sumVec,
                   vector<string> &readVec,
                   vector<vector<string>> &eachTimestampVec) {
    char divider = ',';
    ifstream input(IOfile);
    string line;

    while (getline(input, line)) {
        vector<string> tmp = split(line, divider);
        if (tmp.size() < 5) continue;
        if (tmp[3] == "Write") sumVec.push_back(tmp[4]);
        if (tmp[3] == "Read") readVec.push_back(tmp[4]);
    }
    getEachTimestampVec(sumVec, time_stamp, eachTimestampVec);
    cout << "-------------------------------I/O file loaded!"
         << "-------------------------------" << endl;
}


vector<vector<string>> getRandomStripe(const vector<string> &sum_vec) {
    //原始随机条带组织方法
    unordered_set<string> blockSet;
    vector<vector<string>> stripeRandom;
    vector<string> tempVec;

    for (auto elem : sum_vec) {//获得所有数据块集合
        if (blockSet.find(elem) == blockSet.end()) {
            blockSet.insert(elem);
        }
    }
    auto it = blockSet.begin();
    while (it != blockSet.end()) {//将数据块”随机“分配到不同条带中
        tempVec.push_back(*it);
        if (tempVec.size() == RS_m) {
            stripeRandom.push_back(tempVec);
            tempVec.clear();
        }
        it++;
    }
    if (!tempVec.empty()) {
        stripeRandom.push_back(tempVec);
    }

    return stripeRandom;
}

int getIOCost(const vector<vector<string>> &eachTimestampVec,
              const vector<vector<string>> &stripes) {
    int io_cost = 0;
    map<string, int> blockStripeMap;
    for (size_t i = 0; i < stripes.size(); i++) {
        for (size_t j = 0; j < stripes[i].size(); j++) {
            blockStripeMap.insert({stripes[i][j], i});
        }
    }
    for (int i = 0; i < eachTimestampVec.size(); ++i) {

        map<int, set<string>> stripeBlockTimes;
        for (size_t j = 0; j < eachTimestampVec[i].size(); j++) {
            string curBlock = eachTimestampVec[i][j];
            if (blockStripeMap.find(curBlock) == blockStripeMap.end()) {
                cout << "WARNING: block " << curBlock << " not find !" << endl;
                continue;
            }
            int curStripe = blockStripeMap.at(curBlock);
            if (stripeBlockTimes.find(curStripe) == stripeBlockTimes.end()) {
                stripeBlockTimes.insert({curStripe, {}});
            }

            stripeBlockTimes[curStripe].insert(curBlock);
        }

        auto it = stripeBlockTimes.begin();
        while (it != stripeBlockTimes.end()) {
            io_cost += 2 * (RS_n + (it->second).size());
            it++;
        }
    }

    int tsize = 0;
    for (size_t i = 0; i < stripes.size(); i++) {
        tsize += stripes[i].size();
    }
    cout << "blocksize : " << tsize << endl;
    return io_cost;
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

void analysisBSO(const vector<string> &sum_vec,
                 const vector<string> &read_vec,
                 const vector<vector<string>> &eachTimestampVec) {

    vector<vector<string>> stripeRandom = getRandomStripe(sum_vec);

    cout << "------------------------------------"
         << "BSO------------------------------------" << endl
         << "BSO stripe size:" << stripeRandom.size() << endl;

    int cost = getIOCost(eachTimestampVec, stripeRandom);
    cout << "IOs involved in data updates : "
         << cost << endl;
}

void addWeightToGraph(vector<vector<int>> &matrixWeight, vector<int> &infectVec) {
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

void analysisCASO(const vector<string> &sum_vec,
                  const vector<string> &read_vec,
                  const vector<vector<string>> &eachTimestampVec) {

    map<string, int> blockToInt;
    vector<string> blockVec;
    vector<int> sumIntVec;
    int i_block = 0;

    for (const string &elem : sum_vec) {//从访问序列中获取数据块集
        if (blockToInt.find(elem) == blockToInt.end()) {
            blockVec.push_back(elem);
            blockToInt.insert({elem, i_block++});
        }
        sumIntVec.push_back(blockToInt[elem]);
    }

    vector<vector<int>> matrixWeight(blockVec.size(),//权重集
                                     vector<int>(blockVec.size()));

    addWeightToGraph(matrixWeight, sumIntVec);

    //贪心分割获得最终条带
    vector<vector<string>> finalStripe;
    vector<bool> flag(blockVec.size());
    vector<string> resultTemp;
    /*
     * 循环所有数据块，对每个数据块
     *      将数据块加入临时stripe中，更新最大栈
     *      从最大栈中取出元素加入临时stripe
     *      再更新最大栈，从最大栈中取出元素放入
     *    直到临时stripe的size == block_and_parity_nums
     *      将临时stripe放入finalStripe中，在清空临时stripe
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
    if (!resultTemp.empty()) {
        finalStripe.emplace_back(resultTemp);
    }

    // addEndStripe(finalStripe, sum_second);

    cout << "------------------------------------CASO"
         << "-----------------------------------" << endl
         << "CASO stripe size:" << finalStripe.size() << endl;

    int cost = getIOCost(eachTimestampVec, finalStripe);
    cout << "IOs involved in data updates : "
         << cost << endl;
}


void analysisDISH(const vector<string> &sum_vec,
                  const vector<int> &ghVec,
                  int n,
                  const vector<string> &read_vec,
                  const vector<vector<string>> &eachTimestampVec) {

    vector<vector<int>> coarseStripe(n);
    // 超图分区文件中，第1行的值v 是指数据块1 数据第v个分区
    for (int i = 0; i < ghVec.size(); i++) {
        coarseStripe[ghVec[i]].push_back(i + 1);
    }

    // 将条带整理到每一个条带的数据块数量相同
    vector<int> tempVec;
    for (vector<int> &elemVec : coarseStripe) {
        while (elemVec.size() > RS_m) {
            tempVec.push_back(elemVec.back());
            elemVec.pop_back();
        }
    }

    for (vector<int> &elemVec : coarseStripe) {
        while (elemVec.size() < RS_m && !tempVec.empty()) {
            elemVec.emplace_back(tempVec.back());
            tempVec.pop_back();
        }
    }
    auto it = tempVec.begin();
    while (it != tempVec.end()) {
        if (coarseStripe.back().size() >= RS_m) {
            coarseStripe.emplace_back();
        }
        coarseStripe.back().push_back(*it);
        it++;
    }

    // 将条带中的数字映射为数据块
    map<string, int> blockMap;
    int num = 1;
    //获取映射关系
    for (const string &item : sum_vec) {
        if (blockMap.find(item) == blockMap.end()) {//顶点集
            blockMap.insert({item, num++});
        }
    }
    // 获取 数字->数据块编号 之间的映射关系
    map<int, string> reverseMap;
    auto map_it = blockMap.cbegin();
    while (map_it != blockMap.cend()) {
        reverseMap.insert({map_it->second, map_it->first});
        map_it++;
    }

    // 恢复之后的原数据条带
    vector<vector<string>> finalStripe;
    vector<string> tempVecStr;
    for (vector<int> elem : coarseStripe) {
        for (int item : elem) {
            tempVecStr.push_back(reverseMap[item]);
        }
        finalStripe.push_back(tempVecStr);
        tempVecStr.clear();
    }
    // addEndStripe(finalStripe, sum_second);

    cout << "------------------------------------ DISH"
         << "-----------------------------------" << endl
         << "DISH stripe size:" << finalStripe.size() << endl;

    int cost = getIOCost(eachTimestampVec, finalStripe);
    cout << "IOs involved in data updates : "
         << cost << endl;
}
