#pragma once
#include<vector>
#include<string>
using namespace std;
//决策树节点
struct treeNode {
	//当前节点的特征
	string feature;
	//当前节点的特征值
	string value;
	//孩子节点
	vector<treeNode*>childs;
};

class decisionTree
{
private:
	//计算信息熵
	double calEntropy(vector<vector<string>>datas,int featureIndex);
	//划分样本数据集
	vector<vector<string>>splitdemoDatas(vector<vector<string>>originDatas,int featureAxis,string featureValue);
	//计算经验条件熵
	double calConEntropy(vector<vector<string>>datas,int featureIndex);
	//计算出现次数最多的约会结果
	string mostResult(vector<vector<string>>datas);

public:
	//样本数据集合
	vector<vector<string>> demoDatas;

	decisionTree();
	//读取样本数据
	void readData(string filePath);
	//ID3算法构建决策树
	treeNode*createID3(vector<vector<string>>datas, treeNode* treeRoot);
	//C4.5算法构建决策树
	treeNode* createC45(vector<vector<string>>datas, treeNode* treeRoot);
	//预测约会结果
	string predictResult(treeNode* treeRoot, string* test);

};

