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
    setWindowTitle(QStringLiteral("隐私政策"));

    setStyleSheet("color:black;background-color:rgb(230,230,230)");
    setAttribute(Qt::WA_DeleteOnClose); //自动删除，但是不会将原来指向的指针置nullptr
    //setWindowFlags(Qt::FramelessWindowHint); //否则关不掉
    setWindowModality(Qt::ApplicationModal);
    setReadOnly(true);
    setHtml(content);
    show();
    emit sig_infoLog("弹出隐私政策");
}

PrivacyPolicy::~PrivacyPolicy()
{
}
