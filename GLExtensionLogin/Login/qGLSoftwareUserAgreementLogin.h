#pragma once
#include <QTextEdit>

class SoftwareUserAgreementLogin : public QTextEdit
{
	Q_OBJECT
public:
	SoftwareUserAgreementLogin(QWidget *parent = nullptr);
	~SoftwareUserAgreementLogin();

public slots:

signals:
	void sig_infoLog(QString txt);
	void sig_customLog(QString txt);
	void sig_errorLog(QString txt);
private slots:

private:

};
