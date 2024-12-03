#include"total.h"
#include"decisionTree.h"
#include<qmessagebox.h>
#pragma execution_character_set("utf-8")

//ID3算法构建决策树
void total::ID3CreateTree() {
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Select input file"), "", tr("Text Files (*.csv)")
    );
    if (filename.isEmpty()) {
        return;
    }
    //读取数据
	this->tree = new decisionTree;
	this->tree->readData(filename.toLocal8Bit().constData());
    this->detree = NULL;
    this->detree=this->tree->createID3(tree->demoDatas, this->detree);
    QMessageBox::information(nullptr, "result", "Create successfully");
    drawTree();
}

//c45算法构建决策树
void total::C45CreateTree() {
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Select input file"), "", tr("Text Files (*.csv)")
    );
    if (filename.isEmpty()) {
        return;
    }
    //读取数据
    this->tree = new decisionTree;
    this->tree->readData(filename.toLocal8Bit().constData());
    this->detree = NULL;
    this->detree = this->tree->createC45(tree->demoDatas, this->detree);
    QMessageBox::information(nullptr, "result", "Create successfully");
    drawTree();
}

//预测结果
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

//绘制决策树
void total::drawTree() {
    this->scene = new QGraphicsScene;
    QString name = "决策树";
    QGraphicsTextItem* textItem = scene->addText("[" + name + "]");
    textItem->setPos(0-20, -200-10); // 设置根节点位置
    treeNode* tree = this->detree;
    int depth = 1;
    QPen pen;
    pen.setColor(Qt::blue);
    for (int i = 0; i < tree->childs.size(); i++) {
        int childX = -200+ i * 200; // 控制第一层节点的水平间隔
        int childY = -100; // 控制第一层节点的垂直间隔
        //绘制节点间连线
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
    //绘制当前节点
    QString name = QString::fromStdString(node->value);
    QGraphicsTextItem* textItem = scene->addText("[" + name + "]");
    textItem->setPos(x-20, y-10);

    //绘制当前节点的孩子节点
    int numChilds = node->childs.size();
    int offsetX = 200 / (depth+1)-15 ;// 调整节点之间的水平间隔
    // 计算下一层节点的起始横坐标
    int startX = x - ((numChilds - 1) * offsetX) / 2;
    for (int i = 0; i < numChilds; i++) {
        int childX = startX + i * offsetX;
        int childY = y + 100; // 控制垂直间隔
         //绘制节点间连线
        QGraphicsLineItem* lineItem = scene->addLine(x, y, childX, childY);
        lineItem->setPen(pen);
        // 递归绘制子节点
        drawTreeRecursive(node->childs[i], childX, childY, depth + 1);
    }
}
