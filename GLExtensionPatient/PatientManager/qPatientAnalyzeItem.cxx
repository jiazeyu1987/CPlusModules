#include "qPatientAnalyzeItem.h"

PatientAnalyseItem::PatientAnalyseItem(QWidget *parent)
{
	ui.setupUi(this);
	connect(ui.btnOpen, &QPushButton::clicked, this, &PatientAnalyseItem::OnOpenClick);
	timer = new QTimer();
	connect(timer, &QTimer::timeout, this, [=]() {
		clickTimes = 0;
		timer->stop();
		ui.btnOpen->setEnabled(true);
	});
}

PatientAnalyseItem::~PatientAnalyseItem()
{
	timer->stop();
	delete timer;
	timer = 0;
}

void PatientAnalyseItem::SetInfo(PatientsAnalyse pa)
{
	ui.lblName->setText(QString("%1:%2").arg(QStringLiteral("��������")).arg(pa.analyseName));
	ui.lblType->setText(QString("%1:%2").arg(QStringLiteral("��������")).arg(pa.illnessKind));
	ui.lblDate->setText(pa.changedTime);
	this->pa = pa;
}

void PatientAnalyseItem::OnOpenClick()
{
	clickTimes++; //���һ�μ�ʱ
	if (clickTimes >= 2)
	{
		ui.btnOpen->setEnabled(false);
		OpenAnalyByIndex(pa);
	}
	if (timer->isActive() == false)
		timer->start(350);
}
