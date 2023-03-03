#include "ResultFiberItem.h"
#define DECIMAL_NUMBER 1
ResultFiberItem::ResultFiberItem(QWidget * parent)
{
	m_Controls.setupUi(this);
}

ResultFiberItem::~ResultFiberItem()
{
}

void ResultFiberItem::SetFiberInfo(FiberOutPutInfo1* fiber)
{
	m_Controls.lblFiberName->setText(fiber->fiberName);
	m_Controls.lblType->setText(QStringLiteral("ÐÍºÅ: %1").arg(fiber->fiberType));
	m_Controls.lblPosX->setText(QString("X: %1").arg(fiber->X));
	m_Controls.lblPosY->setText(QString("Y: %1").arg(fiber->Y));
	m_Controls.lblPosZ->setText(QString("Z: %1").arg(fiber->Z));
	m_Controls.lblARC->setText(QString("ARC: %1").arg(fiber->arcAngle));
	m_Controls.lblRAN->setText(QString("RAN: %1").arg(fiber->ringAngle));
	
}

void ResultFiberItem::UpdateHeadRackInfo()
{
}


