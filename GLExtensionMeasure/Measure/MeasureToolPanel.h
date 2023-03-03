#pragma once

#include <QDialog>
#include <QButtonGroup>
#include "ui_MeasureToolPanel.h"

class MeasureToolPanel : public QWidget
{
	Q_OBJECT

public:
	MeasureToolPanel(QWidget *parent = Q_NULLPTR);
	~MeasureToolPanel();
	void OnReset();
	void SetLabelEnabled(QLabel* label,QPushButton* button);
private:
	Ui::MeasureToolPanel ui;

signals:
	void drawLineTriggered();
	void drawAreaTriggered();
	void drawAngleTriggered();
	void drawPointTriggered();
	void drawROITriggered();
	void drawPlaneTriggered();
	void drawOpenCurveTriggered();
	void show_off(bool);
	
protected Q_SLOTS:
	void onMeasureLength();
	void onMeasureArea();
	void onMeasureAngle();
	void onMeasurePoint();
	void onMeasurePlane();
	void onMeasureROI();
	void onMeasureOpenCurve();
	void onShow(bool);
};
