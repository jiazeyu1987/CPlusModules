#pragma once
 
#include <QtWidgets/QWidget>
#include "qSlicerFilterLeftModuleExport.h"
#include "ui_RegisterItemWidget.h"
#include "vtkMRMLScalarVolumeNode.h"
class Q_SLICER_QTMODULES_FILTERLEFT_EXPORT RegisterItemWidget : public QWidget
{
    Q_OBJECT

public:
	int state;
	QString image_path;
	QLabel* label_friend;
	void SetNode(vtkMRMLScalarVolumeNode*);
    RegisterItemWidget(QWidget *parent = Q_NULLPTR);
	void SetName(QString name);
	void SetImage(QString path);
	void SetFrontImage(QString path);
	void SetState(int state);
	void mouseReleaseEvent(QMouseEvent *event);
Q_SIGNALS:
protected slots:
	void onToggled(bool bChecked);
	void dosomething();

private:
    Ui::RegisterItemWidgetClass ui;
	vtkMRMLScalarVolumeNode* m_Node;
private:
	QString m_name;
};
