#include "CUGGIS.h"
// ��Ӹ�ͼ�����ͼ��
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
// ��д mouseDoubleClickEvent ������˫���¼�
void CUGGIS::mouseDoubleClickEvent(QMouseEvent* event) {
    // ֱ��ʹ�� treeView ���źŲ�
    QMainWindow::mouseDoubleClickEvent(event); // ���������¼�
}

void CUGGIS::onTreeViewDoubleClick(const QModelIndex& index) {
    if (index.isValid()) {
        QStandardItem* item = treeModel->itemFromIndex(index);
        QAction* action = qvariant_cast<QAction*>(item->data(Qt::UserRole));

        if (action) {
            qDebug() << "Double click detected on action:" << action->text();
            action->trigger(); // ִ�� QAction
        }
        else {
            qDebug() << "No action found for the selected item.";
        }
    }
    else {
        qDebug() << "Invalid index. No action triggered.";
    }
}


// ������ͼ��Ĵ���
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

