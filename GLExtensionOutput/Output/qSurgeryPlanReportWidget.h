#pragma once

#include <qdesktopwidget.h>
#include <qdialog.h>
#include <QTextTableCell>
#include <qpdfwriter.h>
#include "ui_SurgeryPlanReport.h"
#include "ui_ReportParticularsPatientInfo.h"
#include "ui_FocalZone.h"
#include "ui_FocalZoneTitle.h"
#include "ui_OpticalTitle.h"
#include "ui_Optical.h"
#include "ui_ImageTitle.h"
#include "ui_Image.h"
#include "ui_OpticalBackWay.h"
#include "qGLTool.h"
const int Width = 765;
const int ReportParticularsPatientInfoHeight = 511;
const int FocalZoneTitleHeight = 40;
const int FocalZoneHeight = 262;
const int OpticalTitleHeight = 40;
const int OpticalHeight = 261;
const int OpticalBackwayCellHeight = 30;
const int ImageTitleHeight = 40;
const int ImageHeight = 267;

class Module;
class SurgeryPlanReportWidget : public QDialog
{
    Q_OBJECT
public:
    SurgeryPlanReportWidget(QWidget *parent = nullptr);

public slots:
    bool onExprotPdf();
private:
    void addModule(int height, Module *module); //添加一个模块
    void setPage();

    void addOnePage(int pageNum);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="painter"></param>
    void paintTitle(QPainter &painter);
    void patintRule(QPainter &painter);
    void paintPageNum(QPainter &painter, int pageNum);

    QTextTableFormat tableFormat();
private slots:
    void next();
    void previous();
    void wordLimit(int numOfWord, int limit);

private:
    QWidget *parentWidget;
    Ui::SurgeryPlanReport *surgeryPlanReport;

    //拼装数据
    //PDF_operation *data;
    int pageCurrentNum = 1; // 当前页数
    int pageTotalNum = 1; //总页数
    int currentTab = 1; 
    
    int xPos; //当前位置x1
    int yPos; //当前位置y1
    int lastPos; //最下面的位置

    QTextDocument *document;
    QTextCursor *cursor;
    QPdfWriter* pdfWriter;

    QVector<QVector<Module*>> moduleVec; //保存pdf所有的模块，下标为属于同一页的索引
};

class Module : public QWidget
{
    Q_OBJECT
public:
    Module(QWidget* parent = nullptr)
        :QWidget(parent)
    {
        
    }
    void virtual paintPdf(QPainter& painter) = 0;  

signals:
    void sig_wordLimit(int, int);
};

//报告详情和患者信息
class ReportParticularsPatientInfoPrivate : public Module
{
    Q_OBJECT
public:
    ReportParticularsPatientInfoPrivate(QTextCursor *cursor, int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;
private:
    bool eventFilter(QObject* obj, QEvent *event) override;
    
private:
    Ui::ReportParticularsPatientInfo *ui;
};

//病灶区标题
class FocalZoneTitlePrivate : public Module
{
    Q_OBJECT
public:
    FocalZoneTitlePrivate(QTextCursor *cursor, int num, int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;

private:
    Ui::FocalZoneTitle *ui;
};
//病灶区
class FocalZonePrivate : public Module
{
    Q_OBJECT
public:
    FocalZonePrivate(QTextCursor *cursor, QString focalName, QString diagnose, QString ablationVolume, QString opticalNumber,
        int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;
private:
    bool eventFilter(QObject* obj, QEvent *event) override;
private:
    Ui::FocalZone *ui;
};

//光纤标题
class OpticalTitlePrivate : public Module
{
    Q_OBJECT
public:
    OpticalTitlePrivate(QTextCursor *cursor, int num, int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;

private:
    Ui::OpticalTitle *ui;
};
//光纤
class OpticalPrivate : public Module
{
    Q_OBJECT
public:
    OpticalPrivate(QTextCursor *cursor, QString fiberName, QString model, QString num, QString distance, QString kind,
        QString X, QString Y, QString Z, QString ARC, QString RAN, 
        int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;

private:
    bool eventFilter(QObject* obj, QEvent *event) override;

private:
    Ui::Optical *ui;
};

//光纤后退距离
class OpticalBackwayPrivate : public Module
{
    Q_OBJECT
public:
    OpticalBackwayPrivate(QTextCursor* cursor, const QVector<Backway>& backwayVec, QWidget* parent = nullptr);

    void paintPdf(QPainter& painter) override;
private:
    //bool eventFilter(QObject* obj, QEvent *event) override;

private:
    Ui::Form* ui;

    QVector<Backway> backwayVec;  //后退距离
};

//参考图片标题
class ImageTitlePrivate : public Module
{
    Q_OBJECT
public:
    ImageTitlePrivate(QTextCursor *cursor, int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;

private:
    Ui::ImageTitle *ui;
    QTextCursor *cursor;
};
//参考图片
class ImagePrivate : public Module
{
    Q_OBJECT
public:
    ImagePrivate(QTextCursor *cursor, QStringList image, int x, int y, QWidget *parent = nullptr);
    void paintPdf(QPainter& painter) override;

private:
    Ui::Image *ui;
    QStringList images;
};
