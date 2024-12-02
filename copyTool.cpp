#include "copyTool.h"
#include <QgsGeometry.h>
#include <QgsPointXY.h>
#include <QgsProject.h>
#include <QgsFeature.h>
#include <QgsVectorLayer.h>
#include <QgsMapCanvas.h>
#include <QgsMapMouseEvent.h>
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QgsHighlight.h>
#include <QDoubleValidator>


CustomCopyTool::CustomCopyTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mCanvas(canvas), mCtrlPressed(false)
{
    setCursor(Qt::CrossCursor); // 设置十字光标
    qDebug() << "CustomCopyTool activated.";
}

CustomCopyTool::~CustomCopyTool()
{
}

QgsVectorLayer* CustomCopyTool::getFirstVectorLayer()
{
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer; // 返回找到的第一个矢量图层
        }
    }
    return nullptr; // 如果没有矢量图层，返回空指针
}

void CustomCopyTool::canvasPressEvent(QgsMapMouseEvent* event)
{
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        QMessageBox::warning(nullptr, "No Layer", "No vector layers found!");
        return;
    }

    // 左键点击：选择要素
    if (event->button() == Qt::LeftButton) {
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            event->x(), event->y(), QList<QgsMapLayer*>{layer}, QgsMapToolIdentify::TopDownAll
        );

        if (!results.isEmpty()) {
            mSelectedFeatures.clear();
            for (const auto& result : results) {
                mSelectedFeatures.append(result.mFeature);
                qDebug() << "Selected feature ID:" << result.mFeature.id();
            }
            highlightSelectedFeatures(layer);
        }
        else {
            QMessageBox::information(nullptr, "No Feature", "No features found at clicked position!");
            mSelectedFeatures.clear();
        }
    }

    // 右键点击：复制选中的要素
    if (event->button() == Qt::RightButton && mCtrlPressed) {
        qDebug() << "Right-click event triggered.";

        if (mSelectedFeatures.isEmpty()) {
            QMessageBox::information(nullptr, "No Feature Selected", "Please select at least one feature to copy.");
            return;
        }

        double interval;
        int copyCount;
        qDebug() << "Triggering input dialog.";
        if (!getUserInput(interval, copyCount)) {
            qDebug() << "Input dialog canceled.";
            return;
        }
        qDebug() << "Dialog input received: interval =" << interval << ", copyCount =" << copyCount;

        if (!layer->isEditable() && !layer->startEditing()) {
            QMessageBox::warning(nullptr, "Layer Not Editable", "Failed to start editing mode for the layer.");
            return;
        }

        QgsPointXY basePos = mCanvas->getCoordinateTransform()->toMapCoordinates(event->pos());

        for (const QgsFeature& feature : mSelectedFeatures) {
            for (int i = 0; i < copyCount; ++i) {
                QgsGeometry geometry = QgsGeometry::fromWkt(feature.geometry().asWkt());
                geometry.translate(basePos.x() - feature.geometry().centroid().asPoint().x() + i * interval,
                    basePos.y() - feature.geometry().centroid().asPoint().y());

                QgsFeature newFeature = feature;
                newFeature.setGeometry(geometry);

                if (!layer->addFeature(newFeature)) {
                    QMessageBox::warning(nullptr, "Error", "Failed to add the copied feature.");
                    return;
                }
                qDebug() << "Copied feature ID:" << newFeature.id();
            }
        }

        if (layer->commitChanges()) {
            mCanvas->refresh();
            qDebug() << "Features copied successfully.";
        }
        else {
            QMessageBox::warning(nullptr, "Error", "Failed to commit changes.");
            layer->rollBack();
        }
    }
}

void CustomCopyTool::highlightSelectedFeatures(QgsVectorLayer* layer)
{
    // 清除之前的高亮效果
    for (auto highlight : mHighlights) {
        delete highlight;
    }
    mHighlights.clear();

    // 高亮显示选中的要素
    for (const QgsFeature& feature : mSelectedFeatures) {
        QgsHighlight* highlight = new QgsHighlight(mCanvas,  feature.geometry(),layer);
        highlight->setFillColor(Qt::yellow);
        highlight->setOpacity(0.5);
        mHighlights.append(highlight);
    }
}

bool CustomCopyTool::getUserInput(double& interval, int& copyCount)
{
    QDialog dialog;
    dialog.setWindowTitle("Copy Parameters");

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // 输入间隔
    QLabel* intervalLabel = new QLabel("Interval (in map units):");
    QLineEdit* intervalInput = new QLineEdit(&dialog);
    intervalInput->setValidator(new QDoubleValidator(0.0, 1000.0, 2, intervalInput)); // 限制输入为正数
    layout->addWidget(intervalLabel);
    layout->addWidget(intervalInput);

    // 输入复制数量
    QLabel* countLabel = new QLabel("Number of copies:");
    QLineEdit* countInput = new QLineEdit(&dialog);
    countInput->setValidator(new QIntValidator(1, 100, countInput)); // 限制输入为正整数
    layout->addWidget(countLabel);
    layout->addWidget(countInput);

    // 确认和取消按钮
    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        interval = intervalInput->text().toDouble();
        copyCount = countInput->text().toInt();
        return true; // 用户输入了参数
    }
    return false; // 用户取消了输入
}

void CustomCopyTool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = true;
        qDebug() << "Ctrl pressed. Ready for copying.";
    }
}

void CustomCopyTool::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = false;
        qDebug() << "Ctrl released.";
    }
}