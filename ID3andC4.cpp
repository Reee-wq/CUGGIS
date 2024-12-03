#include "ID3andC4.h"
#include "TreeNodeItem.h"
#include "CUGGIS.h"
#include "require.h"

#include <iostream>
#include <string>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>

ID3andC4::ID3andC4(QWidget *parent)
	: QWidget(parent), pTree(new Node())
{
	
	ui.setupUi(this);
	// ��ʼ��QPlainTextEdit��Ϊ�ն���ʾ
	plainTextEdit = ui.plainTextEdit;
	//weatherLineEdit= ui.weatherLineEdit;
	// ȷ�����ؼ��Ĳ��������ã�û��layout����scene��view��ӵ�m_ID3andC4�ᱨ��
	if (!layout()) {
		QVBoxLayout* layout = new QVBoxLayout(this);
		setLayout(layout);
	}
	//buttonҪ�ֶ���
	connect(ui.predictButton, &QPushButton::clicked, this, &ID3andC4::on_predictButton_triggered);

	
}

ID3andC4::~ID3andC4()
{
	
}

std::string predict(TreeRoot root, const Watermelon& testData) {
	if (!root) return "";

	// Ҷ�ڵ㣬����Ԥ����
	if (root->childs.empty()) {
		return root->attribute;
	}

	 // ���ݵ�ǰ�ڵ��������ѡ������
    // �Ƚ� Watermelon �еĶ�Ӧ�ֶ�
	// ���ݵ�ǰ�ڵ��������ѡ������
	if (root->attribute == "Weather") {
		qDebug() << "Weather: " << QString::fromStdString(root->edgeValue) << endl;
		if (testData.Weather == root->edgeValue) {
			return predict(root->childs[0], testData);  // ������ڵ�ֻ��һ���ӽڵ�
		}
	}
	else if (root->attribute == "Temperature") {
		if (testData.Temperature == root->edgeValue) {
			return predict(root->childs[0], testData);
		}
	}
	else if (root->attribute == "Humidity") {
		if (testData.Humidity == root->edgeValue) {
			return predict(root->childs[0], testData);
		}
	}
	else if (root->attribute == "Wind") {
		if (testData.Wind == root->edgeValue) {
			return predict(root->childs[0], testData);
		}
	}


	return "Unknown";  // ���û��ƥ����ӽڵ㣬����δ֪
}
// ID3�����㷨 -- ���Ͼ�����
TreeRoot TreeGenerate(TreeRoot pTree,
	std::vector<Watermelon> datas,					// ѵ����
	std::vector<std::string> attributes,				// ���Լ�
	std::map<std::string, std::vector<std::string>> map_attr)
{
	if (belongs_same_label(datas, "yes")) {
		// All samples are positive.
		pTree->attribute = "yes";
		return pTree;
	}
	else if (belongs_same_label(datas, "no")) {
		// All samples are negative.
		pTree->attribute = "no";
		return pTree;
	}
	else {
		if (attributes.empty()) {
			pTree->attribute = majority_of_category(datas);
			return pTree;
		}
		else {
			std::pair<std::string, std::vector<std::string>> optimal_attrs = \
				optimal_attribute(datas, attributes, map_attr);
			pTree->attribute = optimal_attrs.first;
			for (auto aptimal_attr : optimal_attrs.second) {
				Node* new_node = new Node();
				new_node->edgeValue = aptimal_attr;

				std::vector<Watermelon> new_datas = remain_watermelon_datas(
					datas, aptimal_attr, optimal_attrs.first);
				if (new_datas.empty()) {
					new_node->attribute = majority_of_category(datas);
					// return pTree;
				}
				else {
					std::vector<std::string> new_attributes;
					for (auto train_attribute : attributes) {
						if (train_attribute.compare(optimal_attrs.first)) {
							new_attributes.push_back(train_attribute);
						}
					}
					TreeGenerate(new_node, new_datas, new_attributes, map_attr);
				}

				pTree->childs.push_back(new_node);
			}
		}
	}

	return pTree;
}
void ID3andC4::on_predictButton_triggered()
{
	// ��ȡ�û�����Ĳ�������
	Watermelon testData;
	testData.Weather = ui.weatherLineEdit->text().toStdString();
	qDebug() << "Weather: " << ui.weatherLineEdit->text() <<endl;
	testData.Temperature = ui.temperatureLineEdit->text().toStdString();
	std::cout << "Temperature: " << testData.Temperature << std::endl;
	testData.Humidity = ui.humidityLineEdit->text().toStdString();
	testData.Wind = ui.windLineEdit->text().toStdString();

	std::vector<Watermelon> datas;
	std::vector<std::string> attributes;
	std::vector<std::string> train_attributes;

	train_attributes.push_back("Weather");
	train_attributes.push_back("Temperature");
	train_attributes.push_back("Humidity");
	train_attributes.push_back("Wind");
	//train_attributes.push_back("Date");


	// load the datas and attributes..
	//
	load_file(datas, attributes, "date.txt");

	std::map<std::string, std::vector<std::string>> map_attr;

	// match attribute set..
	//
	match_properties(datas, attributes, map_attr);



	pTree = TreeGenerate(pTree, datas, train_attributes, map_attr);
	
	// ����Ԥ�⺯��CUGGIS::instance()->m_ID3andC4->
	std::string result = predict(pTree, testData);

	// ��Ԥ������ʾ����һ��lineEdit
	ui.resultLineEdit->setText(QString::fromStdString(result));
}




void CUGGIS::on_actionID3_triggered()
{
	m_ID3andC4 = new ID3andC4();
	
	std::vector<Watermelon> datas;
	std::vector<std::string> attributes;
	std::vector<std::string> train_attributes;

	train_attributes.push_back("Weather");
	train_attributes.push_back("Temperature");
	train_attributes.push_back("Humidity");
	train_attributes.push_back("Wind");
	//train_attributes.push_back("Date");
	

	// load the datas and attributes..
	//
	load_file(datas, attributes, "date.txt");

	std::map<std::string, std::vector<std::string>> map_attr;

	// match attribute set..
	//
	match_properties(datas, attributes, map_attr);

	

	m_ID3andC4->pTree = TreeGenerate(m_ID3andC4->pTree, datas, train_attributes, map_attr);
	// ������Ƿ�Ϊ��
	if (m_ID3andC4->pTree == nullptr) {
		std::cerr << "Error: pTree is nullptr!" << std::endl;
		return;  // �����Ϊ�գ��˳�����
	}
	// ����GraphicsScene��View
	QGraphicsScene* scene = new QGraphicsScene();
	QGraphicsView* view = new QGraphicsView(scene);

	m_ID3andC4->layout()->addWidget(view);  // ��QGraphicsView��ӵ�������
	view->setFixedSize(1000, 550);

	//// ���ӻ�������
	visualizeTree(scene, m_ID3andC4->pTree, 0, 0, 0, 100);
	view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	// �����ɵľ������ַ�����ʾ��ID3andC4 Widget�е�QPlainTextEdit��
	std::string treeString = print_tree(m_ID3andC4->pTree, 0);
	m_ID3andC4->plainTextEdit->setPlainText(QString::fromStdString(treeString));


	m_ID3andC4->show();
}

void CUGGIS::drawTree()
{
	this->scene = new QGraphicsScene;
	QString name = "������";
	QGraphicsTextItem* textItem = scene->addText("[" + name + "]");
	textItem->setPos(0 - 20, -200 - 10); // ���ø��ڵ�λ��
	treeNode* tree = this->detree;
	int depth = 1;
	QPen pen;
	pen.setColor(Qt::blue);
	for (int i = 0; i < tree->childs.size(); i++) {
		int childX = -200 + i * 200; // ���Ƶ�һ��ڵ��ˮƽ���
		int childY = -100; // ���Ƶ�һ��ڵ�Ĵ�ֱ���
		//���ƽڵ������
		QGraphicsLineItem* lineItem = scene->addLine(0, -200, childX, childY);
		lineItem->setPen(pen);
		drawTreeRecursive(tree->childs[i], childX, childY, depth);
	}
	ui.jctreeView->setScene(scene);
	ui.jctreeView->show();
}

void CUGGIS::drawTreeRecursive(treeNode* node, int x, int y, int depth)
{
	if (!node) {
		return;
	}
	QPen pen;
	pen.setColor(Qt::blue);
	//���Ƶ�ǰ�ڵ�
	QString name = QString::fromStdString(node->value);
	QGraphicsTextItem* textItem = scene->addText("[" + name + "]");
	textItem->setPos(x - 20, y - 10);

	//���Ƶ�ǰ�ڵ�ĺ��ӽڵ�
	int numChilds = node->childs.size();
	int offsetX = 200 / (depth + 1) - 15;// �����ڵ�֮���ˮƽ���
	// ������һ��ڵ����ʼ������
	int startX = x - ((numChilds - 1) * offsetX) / 2;
	for (int i = 0; i < numChilds; i++) {
		int childX = startX + i * offsetX;
		int childY = y + 100; // ���ƴ�ֱ���
		 //���ƽڵ������
		QGraphicsLineItem* lineItem = scene->addLine(x, y, childX, childY);
		lineItem->setPen(pen);
		// �ݹ�����ӽڵ�
		drawTreeRecursive(node->childs[i], childX, childY, depth + 1);
	}
}

void CUGGIS::visualizeTree(QGraphicsScene* scene, TreeRoot root, int x, int y, int level, int offset)
{
	if (!root) return;

	// ����һ���ڵ�
	QString text = QString::fromStdString(root->attribute);  // ���ڵ��������ʾΪ�ı�
	TreeNodeItem* nodeItem = new TreeNodeItem(text, QRectF(x, y, 80, 40));  // �����ڵ�
	scene->addItem(nodeItem);

	// �ݹ�����ӽڵ�
	if (!root->childs.empty()) {
		int childOffset = 100;
		for (Node* child : root->childs) {
			scene->addLine(x + 50, y + 50, x + childOffset, y + 100, QPen(Qt::black));  // ������
			QString edgeText = QString::fromStdString(child->edgeValue);  // ��ȡ�ӽڵ�ı�ֵ
			QGraphicsTextItem* edgeLabel = new QGraphicsTextItem(edgeText);
			edgeLabel->setPos((x + childOffset) / 2 + 20, (y + 100) / 2);  // ����λ��
			scene->addItem(edgeLabel);
			visualizeTree(scene, child, x + childOffset, y + 100, level + 1, childOffset);
			childOffset += 150;  // �����ӽڵ�λ��
		}
	}
}

void CUGGIS::on_actionC45_triggered()
{
	QString filename = QFileDialog::getOpenFileName(
		this, tr("Select input file"), "", tr("Text Files (*.csv)")
	);
	if (filename.isEmpty()) {
		return;
	}
	//��ȡ����
	this->tree = new decisionTree;
	this->tree->readData(filename.toLocal8Bit().constData());
	this->detree = NULL;
	this->detree = this->tree->createC45(tree->demoDatas, this->detree);
	QMessageBox::information(nullptr, "result", "Create successfully");
	drawTree();
}