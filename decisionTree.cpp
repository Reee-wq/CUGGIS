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
		//csv��ÿ�е�����
		vector<string>csvLine;
		istringstream iss(lineData);
		string csvData;
		while (getline(iss, csvData, ','))
			csvLine.push_back(csvData);
		demoDatas.push_back(csvLine);
	}
}

//Ԥ����
string decisionTree::predictResult(treeNode* treeRoot,string*test) {
	//����һ���ڵ�ΪҶ�ڵ㣬��Ԥ����
	if (treeRoot->childs[0]->feature=="Date") {
		return treeRoot->childs[0]->value;
	}
	//Ѱ����һ���ڵ���������������ݼ��е�λ��
	int featureIndex;
	for (int j = 0; j < this->demoDatas[0].size(); j++) {
		if (treeRoot->childs[0]->feature == demoDatas[0][j]) {
			featureIndex = j;
			break;
		}
	}
	//Ѱ�ҵ�ǰ�ڵ��з��ϲ��������������ӽڵ�
	for (int i = 0; i < treeRoot->childs.size(); i++) {
		if (treeRoot->childs[i]->value == test[featureIndex]) {
			return predictResult(treeRoot->childs[i], test);
		}
	}
	return "false";
}

treeNode* decisionTree::createID3(vector<vector<string>>datas, treeNode* treeRoot) {
	//���ڵ�
	if (treeRoot == NULL)
		treeRoot = new treeNode();
	//ֻ��һ������ʱ��ֻʣ��һ������"Լ��"ʱ������������ѡ����ִ�������Լ������ΪҶ�ڵ������ֵ
	if (datas[0].size() == 1) {
		treeNode* childNode = new treeNode{ "Date",this->mostResult(datas)};
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//������Ϣ��
	double inforEntropy = calEntropy(datas,datas[0].size()-1);
	//��Ϣ��Ϊ0��ʾ���ֻ��һ�֣�����ǰ�ڵ�ΪҶ�ڵ�
	if (inforEntropy == 0) {
		treeNode* childNode = new treeNode{ "Date" };
		if (datas[1][datas[0].size()-1] == "yes")
			childNode->value = "yes";
		else
			childNode->value = "no";
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//������Ϣ�������ֵ�������Ӧ������λ��
	double bestCrease = 0;
	int bestFeatureIndex;
	for (int i = 0; i < datas[0].size() - 1; i++) {
		double crease = inforEntropy - calConEntropy(datas, i);
		if (crease > bestCrease) {
			bestCrease = crease;
			bestFeatureIndex = i;
		}
	}
	//�����ڵ�
	//ͳ�Ƶ�ǰ�������������п���ȡֵ
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][bestFeatureIndex]);
	//Ϊÿ������ֵ�����ӽڵ�
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		vector<vector<string>>newdatas = splitdemoDatas(datas, bestFeatureIndex, *it);
		//�����ӽڵ㣬����¼����������ֵ
		treeNode* childNode = new treeNode{ datas[0][bestFeatureIndex] ,*it };
		//�ݹ鹹����ǰ�ڵ���ӽڵ�
		createID3(newdatas, childNode);
		treeRoot->childs.push_back(childNode);
	}
	return treeRoot;
}

treeNode* decisionTree::createC45(vector<vector<string>>datas, treeNode* treeRoot) {
	//���ڵ�
	if (treeRoot == NULL)
		treeRoot = new treeNode();
	//ֻ��һ������ʱ��ֻʣ��һ������"Լ��"ʱ������������ѡ����ִ�������Լ������ΪҶ�ڵ������ֵ
	if (datas[0].size() == 1) {
		treeNode* childNode = new treeNode{ "Date",this->mostResult(datas) };
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//������Ϣ��,datas[0].size()-1��ʾ������������ǰΪ��Date��
	double inforEntropy = calEntropy(datas,datas[0].size()-1);
	//��Ϣ��Ϊ0��ʾ���ֻ��һ�֣�����ǰ�ڵ�ΪҶ�ڵ�
	if (inforEntropy == 0) {
		treeNode* childNode = new treeNode{ "Date" };
		if (datas[1][datas[0].size() - 1] == "yes")
			childNode->value = "yes";
		else
			childNode->value = "no";
		treeRoot->childs.push_back(childNode);
		return treeRoot;
	}
	//���㡰��Ϣ�����ʡ����ֵ�������Ӧ������λ��
	double bestCreasePro = 0;
	int bestFeatureIndex;
	for (int i = 0; i < datas[0].size() - 1; i++) {
		//��Ϣ����ֵ
		double crease = inforEntropy - calConEntropy(datas, i);
		//������Ϣ
		double splitInfor = calEntropy(datas,i);
		double creasePro = crease / splitInfor;
		if (creasePro > bestCreasePro) {
			bestCreasePro = crease;
			bestFeatureIndex = i;
		}
	}
	//�����ڵ�
	//ͳ�Ƶ�ǰ�������������п���ȡֵ
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][bestFeatureIndex]);
	//Ϊÿ������ֵ�����ӽڵ�
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		vector<vector<string>>newdatas = splitdemoDatas(datas, bestFeatureIndex, *it);
		//�����ӽڵ㣬����¼����������ֵ
		treeNode* childNode = new treeNode{ datas[0][bestFeatureIndex] ,*it };
		//�ݹ鹹����ǰ�ڵ���ӽڵ�
		createC45(newdatas, childNode);
		treeRoot->childs.push_back(childNode);
	}
	return treeRoot;
}

//������Ϣ��
double decisionTree::calEntropy(vector<vector<string>>datas, int featureIndex) {
	//ͳ�����������ڵ�ǰ�����ĸ�����ֵ����
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][featureIndex]);
	//����ֵֻ��һ�֣���ʾ��Ϣ��Ϊ0
	if (allFeatureValue.size() == 1)
		return  0;
	//������Ϣ��
	double inforEntropy = 0;
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		//��ǰ����ֵ���ִ���
		int num = 0;
		for (int i = 1; i < datas.size(); i++) {
			if (datas[i][featureIndex] == *it)
				num++;
		}
		//���ָ���
		double Prob = (double)num / (double)(datas.size() - 1);
		inforEntropy -= (Prob * (log(Prob) / log(2)));
	}
	return inforEntropy;
}

//�����������ݼ�,��ɾ��һ������֮����������ݼ�
vector<vector<string>> decisionTree::splitdemoDatas(vector<vector<string>>originDatas, int featureIndex, string featureValue) {
	//���ֺ���������ݼ�
	vector<vector<string>>newDataSet;
	for (int i = 0; i < originDatas.size(); i++) {
		//����������ֵ���������ݣ�ɾ����ǰ��������������ݼ�newDataSet��
		if (originDatas[i][featureIndex] == featureValue|| i==0) {
			//���������ݼ��ڵ�ǰ����֮ǰ�����ݴ��������ݼ���
			vector<string>newData(originDatas[i].begin(), originDatas[i].begin()+featureIndex);
			//���������ݼ��ڵ�ǰ����֮������ݴ��������ݼ���
			newData.insert(newData.end(), originDatas[i].begin() + featureIndex + 1, originDatas[i].end());
			newDataSet.push_back(newData);
		}
	}
	return newDataSet;
}

//���㾭��������
double decisionTree::calConEntropy(vector<vector<string>>datas,int featureIndex) {
	//ͳ�Ƶ�ǰ���������п���ȡֵ
	set<string> allFeatureValue;
	for (int j = 1; j < datas.size(); j++)
		allFeatureValue.insert(datas[j][featureIndex]);
	//���㵱ǰ��������Ϣ��
	double inforEntropy = 0;
	set<string>::iterator it;
	for (it = allFeatureValue.begin(); it != allFeatureValue.end(); it++) {
		vector<vector<string>>newdatas = splitdemoDatas(datas, featureIndex, *it);
		double prob = (double)(newdatas.size() - 1) / (double)(datas.size() - 1);
		inforEntropy += (prob * calEntropy(newdatas, newdatas[0].size() - 1));
	}
	return inforEntropy;
}

//�����ִ��������Ϊ���
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