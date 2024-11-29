#include "SymbolModify.h"
#include<QMessageBox>
#include<QgsVectorLayer.h>
#include <qgsrenderer.h>
#include <qgssymbol.h>
#include <qgsmapcanvas.h>
#include<qgssinglesymbolrenderer.h>



SymbolModify::SymbolModify(QWidget* parent, QgsMapLayer* layer)
	: QMainWindow(parent), m_curMapLayer(layer)  // �ڹ��캯���г�ʼ�� m_curMapLayer
{
	ui.setupUi(this);
	QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);
	if(vectorLayer){
	QgsWkbTypes::GeometryType geomType = vectorLayer->geometryType();
    mStyle = StyleManager::getStyle(); //��ȡ��ʽ
    // ȷ�� mModel ����ʼ��
    mModel = new QgsStyleProxyModel(mStyle);

    // ����ͼ���С�������С 
    double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10; // ͼ���С
    ui.listView->setIconSize(QSize(static_cast<int>(iconSize * 0.3), static_cast<int>(iconSize * 0.3)));
    ui.listView->setGridSize(QSize(static_cast<int>(iconSize * 0.35), static_cast<int>(iconSize * 0.45)));
    mModel->addDesiredIconSize(ui.listView->iconSize()); // ����ͼ���С

    // ����ģ�͵���ͼ
    ui.listView->setModel(mModel);
    ui.listView->setSelectionBehavior(QAbstractItemView::SelectItems);  // ��ѡ
    ui.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // ��ѡ
    ui.listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // ������༭
    ui.listView->setWordWrap(true);  // �Զ�����

    // ���÷������͹�����
    mModel->setSymbolTypeFilterEnabled(false);  // ���÷������͹�����

    // ���÷�������
    QVector<QgsSymbol::SymbolType> allSymbolTypes = {
        QgsSymbol::SymbolType::Marker,
        QgsSymbol::SymbolType::Line,
        QgsSymbol::SymbolType::Fill
    };
    int type = 0;
    if (geomType == QgsWkbTypes::PointGeometry) {
        type = 0;
    }
    else if (geomType == QgsWkbTypes::LineGeometry) {
        type = 1;
    }
    else if (geomType == QgsWkbTypes::PolygonGeometry) {
        type = 2;
    }
    mModel->setEntityFilterEnabled(true); // �������͹�����
    mModel->setSymbolTypeFilterEnabled(true); // �������͹�����
    mModel->setSymbolType(static_cast<QgsSymbol::SymbolType>(type)); // ���÷�������
	}

    connect(ui.buttonModify, &QPushButton::clicked, this, &SymbolModify::on_buttonModify_click);

}

SymbolModify::~SymbolModify()
{}

void SymbolModify::on_buttonModify_click() {
    if (m_curMapLayer == nullptr)
        return;
    if (QgsMapLayerType::VectorLayer != m_curMapLayer->type())
        return;

    // ��ȡ��ǰѡ�еķ�����
    QModelIndexList selectedIndexes = ui.listView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "û��ѡ�����", "��ѡ��һ�����Ž����޸ģ�");
        return;
    }
    // ��ȡ��һ��ѡ�е�����
    QModelIndex index = selectedIndexes.first();
    QVariant data = mModel->data(index);
    QString symbolName = data.toString();

    // ��ȡ���Ŷ���
    QgsSymbol* symbol = mStyle->symbol(symbolName);

    QgsVectorLayer* vecLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);
    QgsWkbTypes::GeometryType geomType = vecLayer->geometryType(); // ��ȡ��������
    if (vecLayer) {
        // ����������Ⱦ��
        QgsSingleSymbolRenderer* symRenderer = nullptr;

        if (geomType == QgsWkbTypes::PointGeometry) {
            QgsMarkerSymbol* sym = dynamic_cast<QgsMarkerSymbol*>(symbol);
            if (sym) {
                symRenderer = new QgsSingleSymbolRenderer(sym);
            }
        }
        else if (geomType == QgsWkbTypes::LineGeometry) {
            QgsLineSymbol* sym = dynamic_cast<QgsLineSymbol*>(symbol);
            if (sym) {
                symRenderer = new QgsSingleSymbolRenderer(sym);
            }
        }
        else if (geomType == QgsWkbTypes::PolygonGeometry) {
            QgsFillSymbol* sym = dynamic_cast<QgsFillSymbol*>(symbol);
            if (sym) {
                symRenderer = new QgsSingleSymbolRenderer(sym);
            }
        }

        if (symRenderer) {
            // ������Ⱦ����ͼ��
            vecLayer->setRenderer(symRenderer);
            // ˢ��ͼ��
            vecLayer->triggerRepaint();
            // �رնԻ���
            close();
        }

    }

}