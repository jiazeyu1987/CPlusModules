#pragma once

#include <QtWidgets/QWidget>
#include <QColorDialog>
#include "ui_NaviWidgetStructure.h"
class NaviWidgetStructure : public QWidget
{
	Q_OBJECT

public:
	QString m_Name;
	QString m_Path;
	void SetName(QString);
	void SetPath(QString, QString);
	void SelectedStyle(bool selected);
public:
	NaviWidgetStructure(QWidget* parent = Q_NULLPTR);
private:
    Ui::NaviWidgetStructure ui;
	
};
