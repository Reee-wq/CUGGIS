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
        bool bshow;        //�Ƿ���ʾ��ǩ
        QString name;    //��ʾ�ı�ǩ�ֶ�
    };
    QMap<QgsVectorLayer*, StLabelShowInfo> m_mapLabelshowInfo;  //��¼ÿ��ͼ����һ�εı�ǩ��ʾ��Ϣ

  

private:
    QgsMapCanvas* mMapCanvas;
    QgsMapLayer* m_curMapLayer;  // ��ǰѡ�еĵ�ͼͼ��
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
