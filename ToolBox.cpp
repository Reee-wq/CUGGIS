#include "CUGGIS.h"
// 添加父图层和子图层
void CUGGIS::addLayer(const QString& layerName, QAction* action, const QList<QAction*>& subActions) {
    QStandardItem* parentItem = new QStandardItem(layerName);
    parentItem->setData(QVariant::fromValue((QObject*)action), Qt::UserRole);

    treeModel->appendRow(parentItem);

    for (QAction* subAction : subActions) {
        QStandardItem* subItem = new QStandardItem(subAction->text());
        subItem->setData(QVariant::fromValue((QObject*)subAction), Qt::UserRole);
        parentItem->appendRow(subItem);
    }

    qDebug() << "Added layer:" << layerName << "with actions:";
    qDebug() << "Main action:" << action->text();
    for (QAction* subAction : subActions) {
        qDebug() << "Sub action:" << subAction->text();
    }
}
// 重写 mouseDoubleClickEvent 来处理双击事件
void CUGGIS::mouseDoubleClickEvent(QMouseEvent* event) {
    // 直接使用 treeView 的信号槽
    QMainWindow::mouseDoubleClickEvent(event); // 继续传递事件
}

void CUGGIS::onTreeViewDoubleClick(const QModelIndex& index) {
    if (index.isValid()) {
        QStandardItem* item = treeModel->itemFromIndex(index);
        QAction* action = qvariant_cast<QAction*>(item->data(Qt::UserRole));

        if (action) {
            qDebug() << "Double click detected on action:" << action->text();
            action->trigger(); // 执行 QAction
        }
        else {
            qDebug() << "No action found for the selected item.";
        }
    }
    else {
        qDebug() << "Invalid index. No action triggered.";
    }
}


// 处理主图层的触发
void CUGGIS::onAction1Triggered() {
    qDebug() << "Edit activated";
}

void CUGGIS::onAction2Triggered() {
    qDebug() << "Analyze activated";
}
void CUGGIS::onAction3Triggered() {
    qDebug() << "Edit activated";
}

void CUGGIS::onAction4Triggered() {
    qDebug() << "Analyze activated";
}

void CUGGIS::onAction5Triggered() {
    qDebug() << "Edit activated";
}

