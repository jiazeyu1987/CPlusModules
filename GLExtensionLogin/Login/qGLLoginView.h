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
     * Ŀ�꣺��ʱ���usb�����Ƿ���벢��֤ͨ��
     */
    void slot_timerTimeOut();
    /*
     * Ŀ�꣺��������û�Э��
   */
    void slot_btnUserAgreementClicked();
    //��ʷ�û���
    void onHistoryUsername();

private:
    void setQss(QString txt = QString());
    /**
     * Ŀ�꣺��ʾ�ϴ��û�������û�����һλ��
     */
    QStringList getPreviousLoginUsername();

    /*
   * Ŀ�꣺��¼��ǰ�û�������û���
   */
    void setCurrentLoginUsername(QString username);

    /*
   * Ŀ�꣺��ȡ�Ƿ���Ҫ��ʾ������Э��
   */
    bool getSoftwareUserAgreementShow();

    /*
   * Ŀ�꣺��¼��ǰ�û�������û�����һλ��
   */
    void setSoftwareUserAgreementShow(bool isShow);

    /*
     * Ŀ�꣺��֤���ܹ��Ƿ���룬���ҿ���һ��
     * ����ֵ��1��ʾͨ����0��ʾ��ͨ����-1��ʾû�м�⵽���ܹ�
     */
    enum LoginStatus usbkeyVerification();
    //��¼״̬��ǩ��ʾָ�����ֲ��ɼ�������successָ��������û���������
    void setLabelLogStatusText(QString status, QString icon, bool success, bool clearpassword, bool changedPassword = false);
    /*
     * Ŀ�꣺��֤�û���������ĸ�������
     * ����ֵ��enum LogStatus
     */
    enum LoginStatus verify(Account& account);
    /*
     * Ŀ�꣺��Ӧ���̵Ļس��������õ�ͬ�ڵ�¼��ť
     */
     //void keyPressEvent(QKeyEvent *event);
        /*
         * Ŀ�꣺�����Ĺ��λ���û���������У���Ӧ����ǰһ�ε��û���
         */
         //void slot_lineEdit_USER_focusInEvent() override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    /*
     * Ŀ�꣺����ļ�·�������ڣ��򴴽��ļ���·��
     */
    bool createPath(QString filepath);
    //�رճ���
    //void onBtn_CLOSEClicked();
    void onBtnPrivacyPolicyClicked(bool);
    //���ù������˳��˵�
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
    QLabel* labelLoginStatus; //��ʾ��¼״̬
    QMenu* menu;
    QTimer timer; //��ʱȥ�����ܹ�״̬
    QStringList usernameList;
    enum LoginStatus usbkeyStatus = USBKeyNone;
    HistoryNameWidget* historyNameWidget;
    QToolBar* toolBar;

public:
    void hideLoginStatusLabel();

};

#endif // ZZLoginView_h
