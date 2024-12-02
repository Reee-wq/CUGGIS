#ifndef ADDPOINT_H
#define ADDPOINT_H

#include <QgsMapTool.h>
#include <QgsMapCanvas.h>
#include <QgsVectorLayer.h>
#include <QgsFeature.h>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class AddPoint : public QgsMapTool
{
    Q_OBJECT

public:
    explicit AddPoint(QgsMapCanvas* canvas, QgsVectorLayer* layer, QObject* parent = nullptr);
    ~AddPoint();

    void canvasPressEvent(QgsMapMouseEvent* e) override;
    void undo();
    void redo();

private:
    QgsMapCanvas* m_mapCanvas;
    QgsVectorLayer* m_layer;

    // 弹出属性输入对话框
    QStringList showAttributeInputDialog(const QStringList& fieldNames);
};

#endif // ADDPOINT_H
