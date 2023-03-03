#include "qGLPrivacyPolicy.h"
#include <QDesktopWidget>
#include <qGLLog.h>

PrivacyPolicy::PrivacyPolicy(QWidget* parent)
    : QTextEdit(parent)
{
    //Log* instance = Log::instance();
    //connect(this, &PrivacyPolicy::sig_customLog, instance, &Log::appendInfoLog);
    //connect(this, &PrivacyPolicy::sig_errorLog, instance, &Log::appendInfoLog);
    //connect(this, &PrivacyPolicy::sig_infoLog, instance, &Log::appendInfoLog);

    QDesktopWidget desktop;
    int screenWidth = desktop.availableGeometry().width();
    int screenHeight = desktop.availableGeometry().height();

    setFixedSize(screenWidth / 2, screenHeight / 2);
    setWindowTitle(QStringLiteral("��˽����"));

    setStyleSheet("color:black;background-color:rgb(230,230,230)");
    setAttribute(Qt::WA_DeleteOnClose); //�Զ�ɾ�������ǲ��Ὣԭ��ָ���ָ����nullptr
    //setWindowFlags(Qt::FramelessWindowHint); //����ز���
    setWindowModality(Qt::ApplicationModal);
    setReadOnly(true);
    setHtml(content);
    show();
    emit sig_infoLog("������˽����");
}

PrivacyPolicy::~PrivacyPolicy()
{
}
