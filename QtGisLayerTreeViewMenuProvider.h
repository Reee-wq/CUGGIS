#pragma once
#pragma execution_character_set("utf-8")
#ifndef QTGISLAYERTREEVIEWMENUPROVIDER_H
#define QTGISLAYERTREEVIEWMENUPROVIDER_H
#include <QObject>
#include "qgslayertreeview.h"
#include "qgslayertreemodel.h"
#include "LabelControl.h"
#include "CUGGIS.h"
class QAction;
class QgsMapCanvas;

class QtGisLayerTreeViewMenuProvider :
    public QObject,public QgsLayerTreeViewMenuProvider
{
    Q_OBJECT
public:
    QtGisLayerTreeViewMenuProvider(QgsLayerTreeView* layerTreeView, QgsMapCanvas* mapCanvas);

    ~QtGisLayerTreeViewMenuProvider();

    virtual QMenu* createContextMenu() override;
    struct StLabelShowInfo
    {
        bool bshow;        //是否显示标签
        QString name;    //显示的标签字段
    };
    QMap<QgsVectorLayer*, StLabelShowInfo> m_mapLabelshowInfo;  //记录每个图层上一次的标签显示信息

  

private:
    QgsMapCanvas* mMapCanvas;
    QgsMapLayer* m_curMapLayer;  // 当前选中的地图图层
    QgsLayerTreeView* mView;
    LabelControl* m_dlgLabelctrl{ nullptr };
private slots:
    void slot_labelctrlChange(int id, bool bchange, QString name);
    void slot_labelShowAction();
    void slot_OpenAttributeAction();
signals:
    void sig_labelctrlBtnClicked(int, bool, QString);
    
};
#endif // QTGISLAYERTREEVIEWMENUPROVIDER_H
