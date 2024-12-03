#pragma once
#include<vector>
#include<string>
using namespace std;
//�������ڵ�
struct treeNode {
	//��ǰ�ڵ������
	string feature;
	//��ǰ�ڵ������ֵ
	string value;
	//���ӽڵ�
	vector<treeNode*>childs;
};

class decisionTree
{
private:
	//������Ϣ��
	double calEntropy(vector<vector<string>>datas,int featureIndex);
	//�����������ݼ�
	vector<vector<string>>splitdemoDatas(vector<vector<string>>originDatas,int featureAxis,string featureValue);
	//���㾭��������
	double calConEntropy(vector<vector<string>>datas,int featureIndex);
	//������ִ�������Լ����
	string mostResult(vector<vector<string>>datas);

public:
	//�������ݼ���
	vector<vector<string>> demoDatas;

	decisionTree();
	//��ȡ��������
	void readData(string filePath);
	//ID3�㷨����������
	treeNode*createID3(vector<vector<string>>datas, treeNode* treeRoot);
	//C4.5�㷨����������
	treeNode* createC45(vector<vector<string>>datas, treeNode* treeRoot);
	//Ԥ��Լ����
	string predictResult(treeNode* treeRoot, string* test);

};

