#pragma once

#include <QtWidgets/QWidget>
#include <QColorDialog>
#include "ui_BuildPhaseStructrueView.h"
class BuildPhaseStructrueView : public QWidget
{
    Q_OBJECT

public:
	QString m_Name;
	QString m_Path;
	void SetName(QString);
	void SetPath(QString, QString);

public:
	BuildPhaseStructrueView(QWidget *parent = Q_NULLPTR);
private:
    Ui::BuildPhaseStructureViewClass ui;
	
};
