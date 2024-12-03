#include"total.h"
#include"decisionTree.h"
#include<qmessagebox.h>
#pragma execution_character_set("utf-8")

//ID3�㷨����������
void total::ID3CreateTree() {
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
    this->detree=this->tree->createID3(tree->demoDatas, this->detree);
    QMessageBox::information(nullptr, "result", "Create successfully");
    drawTree();
}

//c45�㷨����������
void total::C45CreateTree() {
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

//Ԥ����
void total::predict() {
    string test[4];
    test[0] = ui.WeatherLineEdit->text().toStdString();
    test[1] = ui.TemperatureLineEdit->text().toStdString();
    test[2] = ui.HumidityLineEdit->text().toStdString();
    test[3] = ui.WindLineEdit->text().toStdString();
    string result = this->tree->predictResult(this->detree, test);
    if (result == "false")
        QMessageBox::information(nullptr, "result", "null");
    else
        QMessageBox::information(nullptr, "result", "dateResult:  " + QString::fromStdString(result));
}

//���ƾ�����
void total::drawTree() {
    this->scene = new QGraphicsScene;
    QString name = "������";
    QGraphicsTextItem* textItem = scene->addText("[" + name + "]");
    textItem->setPos(0-20, -200-10); // ���ø��ڵ�λ��
    treeNode* tree = this->detree;
    int depth = 1;
    QPen pen;
    pen.setColor(Qt::blue);
    for (int i = 0; i < tree->childs.size(); i++) {
        int childX = -200+ i * 200; // ���Ƶ�һ��ڵ��ˮƽ���
        int childY = -100; // ���Ƶ�һ��ڵ�Ĵ�ֱ���
        //���ƽڵ������
        QGraphicsLineItem* lineItem = scene->addLine(0, -200, childX, childY);
        lineItem->setPen(pen);
        drawTreeRecursive(tree->childs[i], childX, childY, depth);
    }
    ui.treeView->setScene(scene);
    ui.treeView->show();
}
void total::drawTreeRecursive(treeNode* node, int x, int y, int depth) {
    if (!node) {
        return;
    }
    QPen pen;
    pen.setColor(Qt::blue);
    //���Ƶ�ǰ�ڵ�
    QString name = QString::fromStdString(node->value);
    QGraphicsTextItem* textItem = scene->addText("[" + name + "]");
    textItem->setPos(x-20, y-10);

    //���Ƶ�ǰ�ڵ�ĺ��ӽڵ�
    int numChilds = node->childs.size();
    int offsetX = 200 / (depth+1)-15 ;// �����ڵ�֮���ˮƽ���
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
