#include "qglsoftwareUserAgreementLogin.h"
#include <QDesktopWidget>
#include "qglsoftwareUserAgreement.h"
#include <qGLlog.h>

SoftwareUserAgreementLogin::SoftwareUserAgreementLogin(QWidget* parent)
    : QTextEdit(parent)
{
    //Log* instance = Log::instance();
    //connect(this, &SoftwareUserAgreementLogin::sig_customLog, instance, &Log::appendInfoLog);
    //connect(this, &SoftwareUserAgreementLogin::sig_errorLog, instance, &Log::appendInfoLog);
    //connect(this, &SoftwareUserAgreementLogin::sig_infoLog, instance, &Log::appendInfoLog);

    QDesktopWidget desktop;
    int screenWidth = desktop.availableGeometry().width();
    int screenHeight = desktop.availableGeometry().height();

    setFixedSize(screenWidth / 2, screenHeight / 2);
    setWindowTitle(QStringLiteral("����Э��"));

    //setStyleSheet("color:black;background-color:rgb(21,31,67)");
    setStyleSheet("color:black;background-color:rgb(255,255,255)");


    setAttribute(Qt::WA_DeleteOnClose);
    //setWindowFlags(Qt::FramelessWindowHint); //����ز���
    setWindowModality(Qt::ApplicationModal);
    setReadOnly(true);
    setHtml(SoftwareUserAgreement::userAgreementContent);
    show();
    emit sig_infoLog("������¼�����û�Э��");
}

SoftwareUserAgreementLogin::~SoftwareUserAgreementLogin()
{
}
