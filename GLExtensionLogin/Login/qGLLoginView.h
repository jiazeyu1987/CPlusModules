#ifndef ZZLoginView_h
#define ZZLoginView_h

#include <qlistwidget.h>
#include <qcheckbox.h>
#include <qGLCommonType.h>
#include <qtimer.h>
#include "ui_qSlicerLoginModuleWidget.h"
#include <qmenu.h>
#include <qSlicerGLModuleWidget.h>
#include <QVBoxLayout>
#include <qtoolbar.h>

class qSlicerLoginModuleWidget;
class HistoryNameWidget : public QDialog
{
  Q_OBJECT
public:
  HistoryNameWidget(QWidget *parent = nullptr);

  void addRow(QString doctorName, QString username);

signals:
  void sigUsername(QString username);
  void sigDelUsername(QString username);

private:
    QVBoxLayout* layoutMain;
};

class ZZLoginView : public QObject 
{
    // this is needed for all Qt objects that should have a Qt meta-object
    // (everything that derives from QObject and wants to have signal/slots)
    Q_OBJECT

public:
    static const std::string VIEW_ID;
    ZZLoginView(Ui_qSlicerLoginModuleWidget& m_Controls, qSlicerLoginModuleWidget* loginModuleWidget);
    ~ZZLoginView();

Q_SIGNALS:
    void errorLog(QString mes);
    void sig_nextPage();

protected  Q_SLOTS:
    void confirm();
    void Exit(QString exitLogin);

protected:
    virtual void CreateQtPartControl(QWidget* parent);

    Ui_qSlicerLoginModuleWidget& m_Controls;
    void OnTimerOutFunction(QString);
    QMainWindow* mainwindow;

private slots:
    /*
     * 目标：定时检查usb密码是否插入并验证通过
     */
    void slot_timerTimeOut();
    /*
     * 目标：弹出软件用户协议
   */
    void slot_btnUserAgreementClicked();
    //历史用户名
    void onHistoryUsername();

private:
    void setQss(QString txt = QString());
    /**
     * 目标：显示上次用户输入的用户名（一位）
     */
    QStringList getPreviousLoginUsername();

    /*
   * 目标：记录当前用户输入的用户名
   */
    void setCurrentLoginUsername(QString username);

    /*
   * 目标：获取是否需要显示软件许可协议
   */
    bool getSoftwareUserAgreementShow();

    /*
   * 目标：记录当前用户输入的用户名（一位）
   */
    void setSoftwareUserAgreementShow(bool isShow);

    /*
     * 目标：验证加密狗是否插入，并且口令一致
     * 返回值：1表示通过，0表示不通过，-1表示没有检测到加密狗
     */
    enum LoginStatus usbkeyVerification();
    //登录状态标签显示指定文字并可见，根据success指定是清空用户名和密码
    void setLabelLogStatusText(QString status, QString icon, bool success, bool clearpassword, bool changedPassword = false);
    /*
     * 目标：验证用户名和密码的各种问题
     * 返回值：enum LogStatus
     */
    enum LoginStatus verify(Account& account);
    /*
     * 目标：响应键盘的回车键，则用等同于登录按钮
     */
     //void keyPressEvent(QKeyEvent *event);
        /*
         * 目标：当鼠标的光标位于用户名输入框中，响应现有前一次的用户名
         */
         //void slot_lineEdit_USER_focusInEvent() override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    /*
     * 目标：如果文件路径不存在，则创建文件和路径
     */
    bool createPath(QString filepath);
    //关闭程序
    //void onBtn_CLOSEClicked();
    void onBtnPrivacyPolicyClicked(bool);
    //设置工具条退出菜单
    void setSelfDefToolBar();

    void ResetLastOperate();

    void GetVarConfig();

    void GetTestCaseConfig(int);

    void AutoCheckAgreement(QString usrname);
signals:
    void sig_infoLog(QString txt);
    void sig_customLog(QString txt);
    void sig_errorLog(QString txt);

private:   
    qSlicerLoginModuleWidget* loginModuleWidget;
    QWidget* parentWidget;
    QLabel* labelLoginStatus; //提示登录状态
    QMenu* menu;
    QTimer timer; //定时去检测加密狗状态
    QStringList usernameList;
    enum LoginStatus usbkeyStatus = USBKeyNone;
    HistoryNameWidget* historyNameWidget;
    QToolBar* toolBar;

public:
    void hideLoginStatusLabel();

};

#endif // ZZLoginView_h
