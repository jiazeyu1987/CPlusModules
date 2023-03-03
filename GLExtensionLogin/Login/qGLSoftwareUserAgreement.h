#pragma once
#include <qwidget.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <QHBoxLayout>
#include <QVBoxLayout>

class  SoftwareUserAgreement : public QWidget
{
	Q_OBJECT
public:
	 SoftwareUserAgreement(QWidget *parent = nullptr);
	~ SoftwareUserAgreement();

  static QString userAgreementContent;

signals:
	void sig_quitEventLoop();
	void sig_infolog(QString txt);

private:
	QTextEdit *edit;
	QPushButton *btnAccept, *btnRefuse;
	QHBoxLayout *layoutBtn;
	QVBoxLayout *layoutV; //垂直布局
	QHBoxLayout *layoutH; //水平布局
};
