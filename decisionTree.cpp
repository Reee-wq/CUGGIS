#include "decisionTree.h"
#include<fstream>
#include<sstream>
#include<cmath>
#include<map>
#include<set>
decisionTree::decisionTree() {
	
}

void decisionTree::readData(string filePath) {
	ifstream in(filePath);
	string lineData;
	while (getline(in, lineData)) {
		//csv中每行的数据
		vector<string>csvLine;
		istringstream iss(lineData);
		string csvData;
		while (getline(iss, csvData, ','))
			csvLine.push_back(csvData);
		demoDatas.push_back(csvLine);
	}
}

//预测结果
string decisionTree::predictResult(treeNode* treeRoot,string*test) {
	//当下一个节点为叶节点，即预测结果
	if (treeRoot->childs[0]->feature=="Date") {
		return treeRoot->childs[0]->value;
	}
	//寻找下一个节点的特征在样本数据集中的位置
	int featureIndex;
	for (int j = 0; j < this->demoDatas[0].size(); j++) {
		if (treeRoot->childs[0]->feature == demoDatas[0][j]) {
			featureIndex = j;
			break;
		}
	}
	//寻找当前节点中符合测试数据条件的子节点
	for (int i = 0; i < treeRoot->childs.size(); i++) {
		if (treeRoot->childs[i]->value == test[featureIndex]) {
			return predictResult(treeRoot->childs[i], test);
		}
	}
	return "false";
}

treeNode* decisionTree::createID3(vector<vector<string>>datas, treeNode* treeRoot) {
	//树节点
	if (treeRoot == NULL)
		treeRoot = new treeNode();
	//只有一个特征时即只剩下一个特征"约会"时，结束构建，选择出现次数最多的约会结果作为叶节点的特征值
	if (datas[0].size() == 1) {
		treeNode* childNode = new treeNode{ "Date",this->mostResult(datas)};
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//计算信息熵
	double inforEntropy = calEntropy(datas,datas[0].size()-1);
	//信息熵为0表示结果只有一种，即当前节点为叶节点
	if (inforEntropy == 0) {
		treeNode* childNode = new treeNode{ "Date" };
		if (datas[1][datas[0].size()-1] == "yes")
			childNode->value = "yes";
		else
			childNode->value = "no";
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//计算信息增益最大值，及其对应的特征位置
	double bestCrease = 0;
	int bestFeatureIndex;
	for (int i = 0; i < datas[0].size() - 1; i++) {
		double crease = inforEntropy - calConEntropy(datas, i);
		if (crease > bestCrease) {
			bestCrease = crease;
			bestFeatureIndex = i;
		}
	}
	//构建节点
	//统计当前最优特征的所有可能取值
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][bestFeatureIndex]);
	//为每个特征值构建子节点
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		vector<vector<string>>newdatas = splitdemoDatas(datas, bestFeatureIndex, *it);
		//构建子节点，并记录特征与特征值
		treeNode* childNode = new treeNode{ datas[0][bestFeatureIndex] ,*it };
		//递归构建当前节点的子节点
		createID3(newdatas, childNode);
		treeRoot->childs.push_back(childNode);
	}
	return treeRoot;
}

treeNode* decisionTree::createC45(vector<vector<string>>datas, treeNode* treeRoot) {
	//树节点
	if (treeRoot == NULL)
		treeRoot = new treeNode();
	//只有一个特征时即只剩下一个特征"约会"时，结束构建，选择出现次数最多的约会结果作为叶节点的特征值
	if (datas[0].size() == 1) {
		treeNode* childNode = new treeNode{ "Date",this->mostResult(datas) };
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//计算信息熵,datas[0].size()-1表示特征索引，当前为“Date”
	double inforEntropy = calEntropy(datas,datas[0].size()-1);
	//信息熵为0表示结果只有一种，即当前节点为叶节点
	if (inforEntropy == 0) {
		treeNode* childNode = new treeNode{ "Date" };
		if (datas[1][datas[0].size() - 1] == "yes")
			childNode->value = "yes";
		else
			childNode->value = "no";
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//计算“信息增益率”最大值，及其对应的特征位置
	double bestCreasePro = 0;
	int bestFeatureIndex;
	for (int i = 0; i < datas[0].size() - 1; i++) {
		//信息增益值
		double crease = inforEntropy - calConEntropy(datas, i);
		//分裂信息
		double splitInfor = calEntropy(datas,i);
		double creasePro = crease / splitInfor;
		if (creasePro > bestCreasePro) {
			bestCreasePro = crease;
			bestFeatureIndex = i;
		}
	}
	//构建节点
	//统计当前最优特征的所有可能取值
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][bestFeatureIndex]);
	//为每个特征值构建子节点
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		vector<vector<string>>newdatas = splitdemoDatas(datas, bestFeatureIndex, *it);
		//构建子节点，并记录特征与特征值
		treeNode* childNode = new treeNode{ datas[0][bestFeatureIndex] ,*it };
		//递归构建当前节点的子节点
		createC45(newdatas, childNode);
		treeRoot->childs.push_back(childNode);
	}
	return treeRoot;
}

//计算信息熵
double decisionTree::calEntropy(vector<vector<string>>datas, int featureIndex) {
	//统计样本数据内当前特征的各特征值种类
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][featureIndex]);
	//特征值只有一种，表示信息熵为0
	if (allFeatureValue.size() == 1)
		return  0;
	//计算信息熵
	double inforEntropy = 0;
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		//当前特征值出现次数
		int num = 0;
		for (int i = 1; i < datas.size(); i++) {
			if (datas[i][featureIndex] == *it)
				num++;
		}
		//出现概率
		double Prob = (double)num / (double)(datas.size() - 1);
		inforEntropy -= (Prob * (log(Prob) / log(2)));
	}
	return inforEntropy;
}

//划分样本数据集,即删除一个特征之后的样本数据集
vector<vector<string>> decisionTree::splitdemoDatas(vector<vector<string>>originDatas, int featureIndex, string featureValue) {
	//划分后的样本数据集
	vector<vector<string>>newDataSet;
	for (int i = 0; i < originDatas.size(); i++) {
		//将符合特征值的样本数据，删除当前特征后存入新数据集newDataSet内
		if (originDatas[i][featureIndex] == featureValue|| i==0) {
			//将样本数据集内当前特征之前的数据存入新数据集内
			vector<string>newData(originDatas[i].begin(), originDatas[i].begin()+featureIndex);
			//将样本数据集内当前特征之后的数据存入新数据集内
			newData.insert(newData.end(), originDatas[i].begin() + featureIndex + 1, originDatas[i].end());
			newDataSet.push_back(newData);
		}
	}
	return newDataSet;
}

//计算经验条件熵
double decisionTree::calConEntropy(vector<vector<string>>datas,int featureIndex) {
	//统计当前特征的所有可能取值
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][featureIndex]);
	//计算当前特征的信息熵
	double inforEntropy = 0;
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		vector<vector<string>>newdatas = splitdemoDatas(datas, featureIndex, *it);
		double prob = (double)(newdatas.size() - 1) / (double)(datas.size() - 1);
		inforEntropy += (prob * calEntropy(newdatas, newdatas[0].size() - 1));
	}
	return inforEntropy;
}

//将出现次数多的作为结果
string decisionTree::mostResult(vector<vector<string>>datas) {
	int yesNum=0, noNum = 0;
	for (int i = 1; i < datas.size(); i++) {
		if (datas[i][0] == "yes")
			yesNum++;
		else
			noNum++;
	}
	if (yesNum > noNum)
		return "yes";
	else
		return "no";
}