#include "MeasureToolPanel.h"
#include "qtimer.h"

MeasureToolPanel::MeasureToolPanel(QWidget *parent)
{
	ui.setupUi(this);

	ui.pBtnLine->setCursor(QCursor(Qt::PointingHandCursor));
	ui.pBtnAngle->setCursor(QCursor(Qt::PointingHandCursor));
	ui.pBtnArea->setCursor(QCursor(Qt::PointingHandCursor));

	/*QButtonGroup *btnMeasureGroup = new QButtonGroup();
	btnMeasureGroup->addButton(ui.pBtnLine);
	btnMeasureGroup->addButton(ui.pBtnAngle);
	btnMeasureGroup->addButton(ui.pBtnArea);
	btnMeasureGroup->setExclusive(true);*/

	connect(ui.pBtnLine, SIGNAL(clicked()), this, SLOT(onMeasureLength()));
	connect(ui.pBtnArea, SIGNAL(clicked()), this, SLOT(onMeasureArea()));
	connect(ui.pBtnAngle, SIGNAL(clicked()), this, SLOT(onMeasureAngle()));
	connect(ui.pBtnPoint, SIGNAL(clicked()), this, SLOT(onMeasurePoint()));
	connect(ui.pBtnPlane, SIGNAL(clicked()), this, SLOT(onMeasurePlane()));
	connect(ui.PBtnOpenCurve, SIGNAL(clicked()), this, SLOT(onMeasureOpenCurve()));
	connect(ui.PBtnClose, SIGNAL(toggled(bool)), this, SLOT(onShow(bool)));

	ui.pBtnLine->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
	ui.pBtnArea->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
	ui.pBtnAngle->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
	ui.pBtnPoint->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
	ui.pBtnPlane->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
	ui.PBtnOpenCurve->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
	ui.PBtnClose->setStyleSheet("QToolTip{background-color: #ffffff; color:#000000; };");
}

MeasureToolPanel::~MeasureToolPanel()
{
}

void MeasureToolPanel::SetLabelEnabled(QLabel* label, QPushButton* button) {
	//label->setStyleSheet("color: #527fd9;");
	button->setCheckable(true);
	button->setChecked(true);
	button->setEnabled(false);
	QTimer::singleShot(1200, [label,button]() {
		//label->setStyleSheet("color: #ffffff;"); 
		button->setChecked(false);
		button->setCheckable(false);
		button->setEnabled(true);
		});
	
}


void MeasureToolPanel::onMeasureArea()
{
	emit drawAreaTriggered();
	SetLabelEnabled(ui.label_15,ui.pBtnArea);
}

void MeasureToolPanel::OnReset() {
	if(ui.pBtnAngle&&ui.pBtnLine&&ui.pBtnArea){ //直线，角度，面积
	ui.pBtnAngle->setChecked(false);
	ui.pBtnLine->setChecked(false);
	ui.pBtnArea->setChecked(false);
	}
}


void MeasureToolPanel::onMeasurePoint()
{
	emit drawPointTriggered();
	SetLabelEnabled(ui.label_17,ui.pBtnPoint);
}

void MeasureToolPanel::onShow(bool b) {
	emit show_off(!b);
}

void MeasureToolPanel::onMeasureOpenCurve() {
	emit drawOpenCurveTriggered();
	SetLabelEnabled(ui.label_19,ui.PBtnOpenCurve);
}

void MeasureToolPanel::onMeasurePlane() {
	emit drawPlaneTriggered();
	SetLabelEnabled(ui.label_18,ui.pBtnPlane);
}

void MeasureToolPanel::onMeasureROI() {
	/*bool isChecked = ui.pBtnROI->isChecked();
	if (isChecked)
	{
		emit drawROITriggered();
	}*/
}

void MeasureToolPanel::onMeasureAngle()
{
	emit drawAngleTriggered();
	SetLabelEnabled(ui.label_16,ui.pBtnAngle);
}

void MeasureToolPanel::onMeasureLength()
{
	emit drawLineTriggered();
	SetLabelEnabled(ui.label_14,ui.pBtnLine);
}
