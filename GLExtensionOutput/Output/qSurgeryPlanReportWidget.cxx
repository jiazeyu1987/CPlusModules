#include "qSurgeryPlanReportWidget.h"
#include "qtabbar.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTextDocument>
#include <qprinter.h>
#include <qdebug.h>
#include <qfiledialog.h>
#include <QTextDocumentFragment>
#include <QPainter>
#include<iostream>
#include <qdatetime.h>
#include <qGLTool.h>
//#include "internal/GLMessageDialog.h"

static int pdfWidth = 0;
static int pdfHeight = 0;
double scaleWidth = 1; //pdf的宽度与窗口的宽度之比
double scaleHeight = 1; //pdf高度与窗口的高度之比
int tabWidghtY = 0; //tabWidget页的y坐标
int tabWidgetX = 0; //tabWidget页的x坐标
static int OpticalBackwayHeight_ = 0; //由行数自行计算
//relateHeight模块相对应窗口的高度，模块所对应的窗口坐标系下的y坐标值

//测试用
void test()
{
    Optical optical;
    optical.ablationCount = 3;
    Backway backway;
    backway.distance = "4mm";
    backway.name = QStringLiteral("后退距离1");
    optical.backwayVec.append(backway);
    GLTool::opticalVec.append(optical);
}

void drawText(QPainter &painter, QLabel* widget, int relateHeight, int relateWidth, Qt::Alignment flag, int fontPixelSize)
{
    int x = widget->x() + relateWidth;
    int y = widget->y() + relateHeight;
    int width = widget->width();
    int height = widget->height();
    int x_ = x * scaleWidth;
    int y_ = y * scaleHeight;
    int width_ = width * scaleWidth;
    int height_ = height * scaleHeight;
    QString txt = widget->text();
    QRect rect(x_, y_, width_, height_);
    QFont font = painter.font();
    font.setPixelSize(fontPixelSize * scaleWidth);
    painter.setFont(font);
    painter.drawText(rect, flag, txt);
}

//通用的绘制文本方法
//exteralIncreaseX, exteralIncreaseY 表示额外附加的坐标增量（一般由于矩形弧度的原因，使得首字符往右下移动）
void drawText1(QPainter &painter, QWidget* widget, int relateHeight, int relateWidth, Qt::Alignment flag,
    QString str, int exteralIncreaseX, int exteralIncreaseY, int fontPixelSize)
{
    int x = widget->x() + relateWidth + exteralIncreaseX;
    int y = widget->y() + relateHeight + exteralIncreaseY;
    int width = widget->width();
    int height = widget->height();
    int x_ = x * scaleWidth;
    int y_ = y * scaleHeight;
    int width_ = width * scaleWidth;
    int height_ = height * scaleHeight;
    QRect rect(x_, y_, width_ - 1, height_ - 1);
    QFont font = painter.font();
    font.setPixelSize(fontPixelSize * scaleWidth);
    painter.setFont(font);
    QTextOption option;
    option.setAlignment(flag);
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    painter.drawText(rect, str, option);
}

void drawRoundRect(QPainter &painter, QWidget* widget, int relateHeight, int radius)
{
    int x = widget->x();
    int y = widget->y() + relateHeight;
    int width = widget->width();
    int height = widget->height();
    int x_ = x * scaleWidth;
    int y_ = y * scaleHeight;
    int width_ = width * scaleWidth;
    int height_ = height * scaleHeight;
    QRect rect(x_, y_, width_, height_);
    painter.drawRoundRect(rect, radius, radius);
}

void drawImage(QPainter &painter, QWidget *widget, int relateHeight, QString image)
{
    int x = widget->x();
    int y = widget->y() + relateHeight;
    int width = widget->width();
    int height = widget->height();
    int x_ = x * scaleWidth;
    int y_ = y * scaleHeight;
    int width_ = width * scaleWidth;
    int height_ = height * scaleHeight;
    painter.drawPixmap(x_, y_, width_, height_, image);
}

//画表格
void drawTable(QPainter& painter, QVector<QVector<QString>> table,
    int topX, int topY, int cellWidth, int cellHeight,
    Qt::Alignment align)
{
    for (int row = 0; row < table.size(); row++)
    {
        for (int col = 0; col < table.at(row).size(); col++)
        {
            //画矩形
            //1)准备矩形坐标，矩形顶点坐标和行列的关系
            int cellX = (topX + col * cellWidth) * scaleWidth;
            int cellY = (topY + row * cellHeight) * scaleHeight;
            QRect rect(cellX, cellY, cellWidth * scaleWidth, cellHeight * scaleHeight);
            painter.drawRect(rect);

            //填充文字
            QString txt = table.at(row).at(col);
            //格式
            QTextOption option;
            option.setAlignment(align);
            option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            painter.drawText(rect, txt, option);
        }
    }
}

SurgeryPlanReportWidget::SurgeryPlanReportWidget(QWidget *parent)
    :QDialog(parent), surgeryPlanReport(new Ui::SurgeryPlanReport)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    setFixedSize(1920, 1080);
    parentWidget = this;
    surgeryPlanReport->setupUi(this);

    QString qss("QPushButton:pressed { background-color: rgb(0,128,255);} QPushButton{ background-color: #465686; color: white }");
    surgeryPlanReport->next->setStyleSheet(qss);
    surgeryPlanReport->previous->setStyleSheet(qss);
    surgeryPlanReport->exportPDF->setStyleSheet(qss);

    //data = PDF_operation::instance();

    xPos = surgeryPlanReport->tabWidget1->widget(0)->x();
    yPos = surgeryPlanReport->tabWidget1->widget(0)->y() + 1;
    lastPos = surgeryPlanReport->tabWidget1->rect().bottomLeft().y() - 1;

    surgeryPlanReport->tabWidget1->tabBar()->setMaximumHeight(0);
    surgeryPlanReport->tabWidget1->tabBar()->hide();
    surgeryPlanReport->tabWidget2->tabBar()->setMaximumHeight(0);
    surgeryPlanReport->tabWidget2->tabBar()->hide();

    document = new QTextDocument;
    document->setDefaultFont(QFont("Times", 12)); //默认字体
    cursor = new QTextCursor(document); //光标

    moduleVec.resize(1);
    //test(); //测试
    setPage(); 

    if (currentTab * 2 - 1 <= 1) //初次应该关闭上一页
    {
        surgeryPlanReport->previous->setEnabled(false);
        surgeryPlanReport->next->setEnabled(true);
    }
    if (pageTotalNum <= 2) //如果总页数不能超过两页,显然是不能翻页的
    {
        surgeryPlanReport->next->setEnabled(false);
        surgeryPlanReport->previous->setEnabled(false);
    }

    //设置当前页
    surgeryPlanReport->tabWidget1->setCurrentIndex(0);
    surgeryPlanReport->tabWidget2->setCurrentIndex(0);
    surgeryPlanReport->page1->setText(QString("%1/%2").arg(currentTab * 2 - 1).arg(pageTotalNum));
    if (pageTotalNum >= 2) //如果大于第2页，则设置
        surgeryPlanReport->page2->setText(QString("%1/%2").arg(currentTab * 2).arg(pageTotalNum));

    connect(surgeryPlanReport->previous, &QPushButton::clicked, this, &SurgeryPlanReportWidget::previous);
    connect(surgeryPlanReport->next, &QPushButton::clicked, this, &SurgeryPlanReportWidget::next);
    connect(surgeryPlanReport->exit, &QPushButton::clicked, this, &SurgeryPlanReportWidget::close);
    connect(surgeryPlanReport->exportPDF, &QPushButton::clicked, this, &SurgeryPlanReportWidget::onExprotPdf);

    parentWidget->showNormal();
    exec();
}

bool SurgeryPlanReportWidget::onExprotPdf()
{
    surgeryPlanReport->exportPDF->setEnabled(false);
    QString filename = QStringLiteral("%1手术计划%3.pdf")
        .arg(GLTool::patientName)
        .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    filename = QFileDialog::getSaveFileName(nullptr, QStringLiteral("选择输出文件路径"), filename, tr("(*.pdf)"));
    if (filename.isEmpty())
    {
        surgeryPlanReport->exportPDF->setEnabled(true);
        return false;
    }

    pdfWriter = new QPdfWriter(filename);
    pdfWriter->setPageSize(QPagedPaintDevice::A4);
    pdfWriter->setResolution(300);
    int margin = 0;
    pdfWriter->setPageMargins(QMarginsF(margin, margin, margin, margin));
    pdfWidth = pdfWriter->width();
    pdfHeight = pdfWriter->height();
    int width = surgeryPlanReport->widget->width();
    int height = surgeryPlanReport->widget->height();
    scaleWidth = pdfWidth / width;
    scaleHeight = pdfHeight / height;
    QPainter painter(pdfWriter);

    int pix = painter.font().pixelSize();

    tabWidghtY = surgeryPlanReport->tabWidget1->y();
    tabWidgetX = surgeryPlanReport->tabWidget1->x();
    for (int i = 0; i < moduleVec.size(); i++)
    {
        //添加顶部标题
        paintTitle(painter);
        //添加直线
        patintRule(painter);
        QVector<Module *> _moduleVec = moduleVec.at(i);
        for (int j = 0; j < _moduleVec.size(); j++)
        {
            _moduleVec.at(j)->paintPdf(painter);
        }
        //添加页码
        paintPageNum(painter, i + 1);
        //新的一页
        if (i < moduleVec.size() - 1)
            pdfWriter->newPage();
    }

    surgeryPlanReport->exportPDF->setEnabled(true);
    return true;
}

//添加一个模块
void SurgeryPlanReportWidget::addModule(int height, Module *module)
{
    if (yPos + height > lastPos)
    {
        addOnePage(pageTotalNum);
    }
    if (pageTotalNum % 2 == 0)
    {
        module->setParent(surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
    }
    else
    {
        module->setParent(surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2));
    }

    yPos += module->height();
}

//添加模块之前判断是否可能超页
//如果超页，添加新页，更新xPos和yPos，然后添加模块
//如果不超页，则添加模块
//更新xPos和yPos
void SurgeryPlanReportWidget::setPage()
{    
    //-----------报告详情和患者信息-----------
    if (yPos + ReportParticularsPatientInfoHeight > lastPos)
    {
        addOnePage(pageTotalNum);
    }
    ReportParticularsPatientInfoPrivate *rppip;
    if (pageTotalNum % 2 == 0)
    {
        rppip = new ReportParticularsPatientInfoPrivate(cursor, xPos, yPos, 
            surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
    }
    else
    {
        rppip = new ReportParticularsPatientInfoPrivate(cursor, xPos, yPos,
            surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
    }
    yPos += rppip->height();
    moduleVec.last().append(rppip); //添加进来，然后按照顺序遍历
    connect(moduleVec.last().last(), &Module::sig_wordLimit, this, &SurgeryPlanReportWidget::wordLimit);
    //--------------病灶区标题----------
    if (yPos + FocalZoneTitleHeight > lastPos)
    {
        addOnePage(pageTotalNum);
    }
    FocalZoneTitlePrivate *focalZoneTitle;
    if (pageTotalNum % 2 == 0)
    {
        focalZoneTitle = new FocalZoneTitlePrivate(cursor, GLTool::focalZoneVec.size(), xPos, yPos,
            surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
    }
    else
    {
        focalZoneTitle = new FocalZoneTitlePrivate(cursor, GLTool::focalZoneVec.size(), xPos, yPos,
            surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
    }
    yPos += focalZoneTitle->height();
    moduleVec.last().append(focalZoneTitle);
    //---------------病灶区--------------
    for (int i = 0; i < GLTool::focalZoneVec.size(); i++)
    {
        if (yPos + FocalZoneHeight > lastPos) //如果大于最大位置，插入一行
        {
            addOnePage(pageTotalNum);
        }
        FocalZonePrivate *focalZone;
        if (pageTotalNum % 2 == 0)
        {
            focalZone = new FocalZonePrivate(cursor,
                GLTool::focalZoneVec.at(i).focusName, GLTool::focalZoneVec.at(i).diagnose,
                GLTool::focalZoneVec.at(i).ablationVolume, GLTool::focalZoneVec.at(i).opticalNumber,
                xPos, yPos, surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
        }
        else
        {
            focalZone = new FocalZonePrivate(cursor,
                GLTool::focalZoneVec.at(i).focusName, GLTool::focalZoneVec.at(i).diagnose,
                GLTool::focalZoneVec.at(i).ablationVolume, GLTool::focalZoneVec.at(i).opticalNumber,
                xPos, yPos, surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
        }
        yPos += focalZone->height();
        moduleVec.last().append(focalZone);
        connect(focalZone, &FocalZonePrivate::sig_wordLimit, this, &SurgeryPlanReportWidget::wordLimit);
    }
    //----------光纤标题--------------
    if (yPos + OpticalTitleHeight > lastPos)
    {
        addOnePage(pageTotalNum);
    }
    OpticalTitlePrivate *opticalTitle;
    if (pageTotalNum % 2 == 0)
    {
        opticalTitle = new OpticalTitlePrivate(cursor, GLTool::opticalVec.size(), xPos, yPos,
            surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
    }
    else
    {
        opticalTitle = new OpticalTitlePrivate(cursor, GLTool::opticalVec.size(), xPos, yPos,
            surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
    }
    yPos += opticalTitle->height();
    moduleVec.last().append(opticalTitle);
    
    //-----------光纤------------
    for (int i = 0; i < GLTool::opticalVec.size(); i++)
    {
        if (yPos + OpticalHeight > lastPos)
        {
            addOnePage(pageTotalNum);
        }
        OpticalPrivate *optical;
        if (pageTotalNum % 2 == 0)
        {
            optical = new OpticalPrivate(cursor, GLTool::opticalVec.at(i).name,
                GLTool::opticalVec.at(i).model, GLTool::opticalVec.at(i).ablationCount,
                GLTool::opticalVec.at(i).targetOffset, GLTool::opticalVec.at(i).headstockType,
                GLTool::opticalVec.at(i).X, GLTool::opticalVec.at(i).Y, GLTool::opticalVec.at(i).Z,
                GLTool::opticalVec.at(i).ARC, GLTool::opticalVec.at(i).RAN,
                xPos, yPos, surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
        }
        else
        {
            optical = new OpticalPrivate(cursor, GLTool::opticalVec.at(i).name,
                GLTool::opticalVec.at(i).model, GLTool::opticalVec.at(i).ablationCount,
                GLTool::opticalVec.at(i).targetOffset, GLTool::opticalVec.at(i).headstockType,
                GLTool::opticalVec.at(i).X, GLTool::opticalVec.at(i).Y, GLTool::opticalVec.at(i).Z,
                GLTool::opticalVec.at(i).ARC, GLTool::opticalVec.at(i).RAN,
                xPos, yPos, surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
        }
        yPos += optical->height();
        moduleVec.last().append(optical);
        connect(optical, &OpticalPrivate::sig_wordLimit, this, &SurgeryPlanReportWidget::wordLimit);

        //添加后退距离
        if (GLTool::opticalVec.at(i).backwayVec.size() > 0)
        {
            OpticalBackwayPrivate* backway = new OpticalBackwayPrivate(cursor, GLTool::opticalVec[i].backwayVec);
            if (yPos + OpticalBackwayHeight_ > lastPos)
            {
                addOnePage(pageTotalNum);
            }
            //设置父类
            if (pageTotalNum % 2 == 0)
            {
                backway->setParent(surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
            }
            else
            {
                backway->setParent(surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
            }
            //设置位置
            backway->setGeometry(xPos, yPos, Width, OpticalBackwayHeight_);
            //更新位置
            yPos += backway->height();
            moduleVec.last().append(backway);
        }
    }

    //----------参考图片-----------
	if (GLTool::capture.size() != 0)
    {
		if (yPos + ImageTitleHeight > lastPos)
		{
			addOnePage(pageTotalNum);
		}
		ImageTitlePrivate *imageTitle;
		if (pageTotalNum % 2 == 0)
		{
			imageTitle = new ImageTitlePrivate(cursor, xPos, yPos,
				surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
		}
		else
		{
			imageTitle = new ImageTitlePrivate(cursor, xPos, yPos,
				surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
		}
		yPos += imageTitle->height();
		moduleVec.last().append(imageTitle);

		for (int i = 0; i < GLTool::capture.size(); i += 2)
		{
			if (yPos + ImageHeight > lastPos)
			{
				addOnePage(pageTotalNum);
			}
			QStringList imageList{ GLTool::capture.at(i) };
			if (i + 1 < GLTool::capture.size())
				imageList << GLTool::capture.at(i + 1);
			ImagePrivate *image;
			if (pageTotalNum % 2 == 0)
			{
				image = new ImagePrivate(cursor, imageList, xPos, yPos,
					surgeryPlanReport->tabWidget2->widget(pageTotalNum / 2 - 1));
			}
			else
			{
				image = new ImagePrivate(cursor, imageList, xPos, yPos,
					surgeryPlanReport->tabWidget1->widget(pageTotalNum / 2));
			}
			yPos += image->height();
			moduleVec.last().append(image);
            connect(image, &ImagePrivate::sig_wordLimit, this, &SurgeryPlanReportWidget::wordLimit);
		}
	}
}

void SurgeryPlanReportWidget::addOnePage(int pageNum)
{
    QWidget *tabWidget = new QWidget;
    if (pageNum % 2 == 0)
    {
        surgeryPlanReport->tabWidget1->addTab(tabWidget, QString());
        xPos = surgeryPlanReport->tabWidget1->widget(0)->x();
        yPos = surgeryPlanReport->tabWidget1->widget(0)->y() + 1;
        surgeryPlanReport->tabWidget1->tabBar()->hide();
    }
    else
    {
        surgeryPlanReport->tabWidget2->addTab(tabWidget, QString());
        xPos = surgeryPlanReport->tabWidget2->widget(0)->x();
        yPos = surgeryPlanReport->tabWidget2->widget(0)->y() + 1;
        surgeryPlanReport->tabWidget2->tabBar()->hide();
    }
    
    pageTotalNum++;

    //PDF分页
    QTextBlockFormat blockFormat;
    blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor->mergeBlockFormat(blockFormat);

    //新增模块数组
    QVector<Module*> module;
    moduleVec.append(module);
}

void SurgeryPlanReportWidget::paintTitle(QPainter &painter)
{
    //佳量医疗
    int pix = surgeryPlanReport->logo1->font().pixelSize();
    drawText(painter, surgeryPlanReport->logo1, 0, 0, Qt::AlignHCenter, 14);

    //LITTPlan
    drawText(painter, surgeryPlanReport->littPlan1, 0, 0, Qt::AlignHCenter, 14);
}

//添加水平线
void SurgeryPlanReportWidget::patintRule(QPainter &painter)
{
    int x0 = surgeryPlanReport->line->x();
    int y0 = surgeryPlanReport->line->y();
    int x1 = x0 + surgeryPlanReport->line->width();
    int y1 = y0;
    int x0_ = x0 * scaleWidth;
    int y0_ = y0 * scaleHeight;
    int x1_ = x1 * scaleWidth;
    int y1_ = y1 * scaleHeight;

    QPen pen(Qt::black, 3);
    painter.setPen(pen);
    painter.drawLine(x0_, y0_, x1_, y1_);
    QPen pen1(Qt::black, 2); //恢复2个大小
    painter.setPen(pen1);
}

void SurgeryPlanReportWidget::paintPageNum(QPainter & painter, int pageNum)
{
    drawText(painter, surgeryPlanReport->page1, 0, 0, Qt::AlignHCenter, 12);
}

QTextTableFormat SurgeryPlanReportWidget::tableFormat()
{
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setCellPadding(10);
    tableFormat.setTopMargin(10);
    tableFormat.setBottomMargin(10);
    QVector<QTextLength> widths;
    widths << QTextLength(QTextLength::PercentageLength, 50) << QTextLength(QTextLength::PercentageLength, 50); //50表示总宽度的百分比
    tableFormat.setColumnWidthConstraints(widths);
    return tableFormat;
}

void SurgeryPlanReportWidget::next()
{
    currentTab++;
    surgeryPlanReport->tabWidget1->setCurrentIndex(currentTab - 1);
    surgeryPlanReport->tabWidget2->setCurrentIndex(currentTab - 1);
    surgeryPlanReport->page1->setText(QString("%1/%2").arg(currentTab * 2 - 1).arg(pageTotalNum));
    surgeryPlanReport->page2->setText(QString("%1/%2").arg(currentTab * 2).arg(pageTotalNum));
    if (currentTab * 2 - 1 == pageTotalNum)
    {
        surgeryPlanReport->next->setEnabled(false);
        surgeryPlanReport->previous->setEnabled(true);
		surgeryPlanReport->page2->clear(); //如果左边页已经到达最大值，则清除右边的页标志
    }
    else if (currentTab * 2 >= pageTotalNum)
    {
        surgeryPlanReport->next->setEnabled(false);
        surgeryPlanReport->previous->setEnabled(true);
    }
}

void SurgeryPlanReportWidget::previous()
{
    currentTab--; //两页同时减，就像翻书一样
    surgeryPlanReport->tabWidget1->setCurrentIndex(currentTab-1);
    surgeryPlanReport->tabWidget2->setCurrentIndex(currentTab-1);
    surgeryPlanReport->page1->setText(QString("%1/%2").arg(currentTab * 2 - 1).arg(pageTotalNum));
    surgeryPlanReport->page2->setText(QString("%1/%2").arg(currentTab * 2).arg(pageTotalNum));
    if (currentTab * 2 - 1 <= 1)
    {
        surgeryPlanReport->previous->setEnabled(false);
        surgeryPlanReport->next->setEnabled(true);
    }
}

void SurgeryPlanReportWidget::wordLimit(int numOfWord, int limit)
{
    if (numOfWord > limit)
    {
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("患者管理"), QStringLiteral("字数不能超过%1").arg(limit));
    }
        //GLMessageDialog::ShowInformation(QStringLiteral("患者管理"), QStringLiteral("字数不能超过%1").arg(limit));
}

//报告详情-患者信息
ReportParticularsPatientInfoPrivate::ReportParticularsPatientInfoPrivate(QTextCursor *cursor, int x, int y, QWidget *parent)
    :Module(parent), ui(new Ui::ReportParticularsPatientInfo)
{
    ui->setupUi(this);
    ui->remark->installEventFilter(this);
    ui->medicalHostory->installEventFilter(this);
    ui->remark->viewport()->installEventFilter(this);
    ui->medicalHostory->viewport()->installEventFilter(this);

    //PDF_operation *data = PDF_operation::instance();
    ui->provideTime->setText(GLTool::datetime);
    ui->provider->setText(GLTool::worker);
	QDateTime dt = QDateTime::currentDateTime();
    ui->receiverTime->setDateTime(dt);
	//ui->receiverTime->setCalendarPopup(true);
    ui->verifier->setText(GLTool::verifier);
    ui->doctor->setText(GLTool::doctor);
    ui->hospital->setText(GLTool::address);
	ui->verifyTime->setDateTime(dt);
	//ui->verifyTime->setCalendarPopup(true);
    ui->patientName->setText(GLTool::patientName);
    ui->sex->setText(GLTool::patientSex);
    ui->birthday->setText(GLTool::patientBirthday);
    ui->height->setText(GLTool::height);
    ui->weight->setText(GLTool::weight);
    setGeometry(x, y, Width, ReportParticularsPatientInfoHeight);
    connect(ui->medicalHostory, &QTextEdit::textChanged, this, [=]() 
    { 
        emit sig_wordLimit(ui->medicalHostory->toPlainText().size(), 286); 
    });
    connect(ui->remark, &QTextEdit::textChanged, this, [=]()
    {
        emit sig_wordLimit(ui->remark->toPlainText().size(), 286);
    });
	//审核人
	connect(ui->verifier, &QLineEdit::textChanged, this, [=]()
	{
		emit sig_wordLimit(ui->verifier->text().size(), 10);
	});
	//主治医生
	connect(ui->doctor, &QLineEdit::textChanged, this, [=]()
	{
		emit sig_wordLimit(ui->doctor->text().size(), 10);
	});
	//所在医院
	connect(ui->hospital, &QLineEdit::textChanged, this, [=]()
	{
		emit sig_wordLimit(ui->hospital->text().size(), 19);
	});
	//所在地
	connect(ui->address, &QLineEdit::textChanged, this, [=]()
	{
		emit sig_wordLimit(ui->address->text().size(), 19);
	});
	//身高
	connect(ui->height, &QLineEdit::textChanged, this, [=]()
	{
		//身高大小在0-1000cm,小数点后最多一位
		QString txt = ui->height->text();
		if (txt.isEmpty())
			return;
		int dotNum = 0;
		double num = txt.toDouble();
		if (txt.contains(QChar('.')))
			dotNum = txt.mid(txt.lastIndexOf(QChar('.'))).size() - 1;
		if (num <= 0 || num >= 1000 || dotNum > 1)
		{
			QMessageBox msg;
			QPushButton *btn = new QPushButton(QStringLiteral("确认"));
			msg.addButton(btn, QMessageBox::AcceptRole);
			msg.setWindowTitle(QStringLiteral("PDF报告输出"));
			msg.setText(QStringLiteral("身高应大于0,小于1000cm,且只能包含数字和最多一位小数点,小数点后最多一位"));
			msg.exec();
		}
	});
	//体重
	connect(ui->weight, &QLineEdit::textChanged, this, [=]()
	{
		//体重大小在0-1000kg,小数点后最多一位
		QString txt = ui->weight->text();
		if (txt.isEmpty())
			return;
		int dotNum = 0;
		double num = txt.toDouble();
		if (txt.contains(QChar('.')))
			dotNum = txt.mid(txt.lastIndexOf(QChar('.'))).size() - 1;
		if (num <= 0 || num >= 1000 || dotNum > 1)
		{
			QMessageBox msg;
			QPushButton *btn = new QPushButton(QStringLiteral("确认"));
			msg.addButton(btn, QMessageBox::AcceptRole);
			msg.setWindowTitle(QStringLiteral("PDF报告输出"));
			msg.setText(QStringLiteral("身高应大于0,小于1000kg,且只能包含数字和最多一位小数点,小数点后最多一位"));
			msg.exec();
		}
	});
}

FocalZoneTitlePrivate::FocalZoneTitlePrivate(QTextCursor *cursor, int num, int x, int y, QWidget *parent)
    :Module(parent), ui(new Ui::FocalZoneTitle)
{
    ui->setupUi(this);
    ui->label_2->setText(QStringLiteral("病灶区（%1）").arg(num));
    setGeometry(x, y, Width, FocalZoneTitleHeight);
}

FocalZonePrivate::FocalZonePrivate(QTextCursor *cursor, QString focalName, QString diagnose, 
    QString ablationVolume, QString opticalNumber,
    int x, int y, QWidget *parent)
    :Module(parent), ui(new Ui::FocalZone)
{
    ui->setupUi(this);
    ui->remark->installEventFilter(this);
    ui->remark->viewport()->installEventFilter(this);
    ui->focalName->setText(focalName);
    //ui->diagnose->setText(diagnose);
    ui->meltVolume->setText(ablationVolume);
    //ui->opticalNum->setText(opticalNumber);

    setGeometry(x, y, Width, FocalZoneHeight);

    connect(ui->remark, &QTextEdit::textChanged, this, [=]()
    {
        emit sig_wordLimit(ui->remark->toPlainText().size(), 345);
    });
}

OpticalTitlePrivate::OpticalTitlePrivate(QTextCursor *cursor, int num, int x, int y, QWidget *parent)
    :Module(parent), ui(new Ui::OpticalTitle)
{
    ui->setupUi(this);
    ui->optical->setText(QStringLiteral("光纤（%1）").arg(num));
    setGeometry(x, y, Width, OpticalTitleHeight);
}

OpticalPrivate::OpticalPrivate(QTextCursor *cursor, QString fiberName, QString model, QString num, QString distance, QString kind,
    QString X, QString Y, QString Z, QString ARC, QString RAN, int x, int y, QWidget *parent)
    : Module(parent), ui(new Ui::Optical)
{
    ui->setupUi(this);
    ui->textEdit->installEventFilter(this);
    ui->textEdit->viewport()->installEventFilter(this);
	ui->lblName->setText(fiberName);
    ui->model->setText(model);
    ui->num->setText(num);
    //ui->distance->setText(distance);
    ui->kind->setText(kind);
	ui->X->setText(X);
	ui->Y->setText(Y);
	ui->Z->setText(Z);
	ui->ARC->setText(ARC);
	ui->RAN->setText(RAN);
	ui->kind->setText(kind);
    setGeometry(x, y, Width, OpticalHeight);
    connect(ui->textEdit, &QTextEdit::textChanged, this, [=]()
    {
        emit sig_wordLimit(ui->textEdit->toPlainText().size(), 240);
    });
}

OpticalBackwayPrivate::OpticalBackwayPrivate(QTextCursor* cursor, const QVector<Backway>& backwayVec, QWidget* parent)
    :backwayVec(backwayVec), ui(new Ui::Form)
{
    ui->setupUi(this);
    //初始化表格
    ui->tableWidget->setColumnCount(2);
    //设置标题
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << QStringLiteral("步骤") << QStringLiteral("距离"));
    ui->tableWidget->horizontalHeader()->setFixedHeight(35);
    QPalette pa;
    pa.setColor(QPalette::ColorRole::Background, Qt::white);
    pa.setColor(QPalette::ColorRole::Text, Qt::black);
    ui->tableWidget->horizontalHeader()->setPalette(pa);
    //设置充满表格，平均分布
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setLineWidth(1);
    ui->tableWidget->setStyleSheet("QTableView::Item{background-color:white; color: black; border:1px solid rgb(0,0,0) }");
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:white; color: black; border:1px solid rgb(0,0,0)}");
    ui->tableWidget->setShowGrid(false);
    //设置位置
    int rows = backwayVec.size();
    OpticalBackwayHeight_ = rows * OpticalBackwayCellHeight + 35; //35表示标题的高度
    //setGeometry(x, y, Width, rows * OpticalBackwayHeight);

    setFixedHeight(OpticalBackwayHeight_);
    ui->tableWidget->setFixedHeight(OpticalBackwayHeight_);
    setStyleSheet("background-color: white; color: black");

    for (int row = 0; row < rows; row++)
    {
        QString col0 = backwayVec.at(row).name;
        QString col1 = backwayVec.at(row).distance;

        ui->tableWidget->insertRow(row);
        QTableWidgetItem* item0 = new QTableWidgetItem(col0);
        item0->setForeground(QBrush(Qt::black));
        item0->setTextAlignment(Qt::AlignHCenter);
        QTableWidgetItem* item1 = new QTableWidgetItem(col1);
        item1->setForeground(QBrush(Qt::black));
        item1->setTextAlignment(Qt::AlignHCenter);

        ui->tableWidget->setItem(row, 0, item0);
        ui->tableWidget->setItem(row, 1, item1);

        //设置行高
        ui->tableWidget->setRowHeight(row, OpticalBackwayCellHeight);
    }
}


ImageTitlePrivate::ImageTitlePrivate(QTextCursor *cursor, int x, int y, QWidget *parent)
    :Module(parent), ui(new Ui::ImageTitle), cursor(cursor)
{
    ui->setupUi(this);
    setGeometry(x, y, Width, ImageTitleHeight);
}

ImagePrivate::ImagePrivate(QTextCursor *cursor, QStringList image, int x, int y, QWidget *parent)
    :Module(parent), ui(new Ui::Image), images(image)
{
    ui->setupUi(this);
    QPixmap pix = QPixmap(image.at(0));
    QSize pixSize = ui->image1->size();
    pix.scaled(pixSize, Qt::KeepAspectRatioByExpanding);
    ui->image1->setPixmap(pix);
    ui->image1->setScaledContents(true);
    if (image.size() >= 2)
    {
        QPixmap pix = QPixmap(image.at(1));
        QSize pixSize = ui->image2->size();
        pix.scaled(pixSize, Qt::KeepAspectRatioByExpanding);
        ui->image2->setPixmap(pix);
        ui->image2->setScaledContents(true);
    }
    else
    {
        ui->label->hide();
        ui->image2->hide();
        ui->remark2->hide();
    }
    setGeometry(x, y, Width, ImageHeight);
    connect(ui->remark1, &QLineEdit::textChanged, this, [=]()
    {
        emit sig_wordLimit(ui->remark1->text().size(), 19);
    });
    connect(ui->remark2, &QLineEdit::textChanged, this, [=]()
    {
        emit sig_wordLimit(ui->remark2->text().size(), 19);
    });
}

bool ReportParticularsPatientInfoPrivate::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == ui->remark && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *eventKey = static_cast<QKeyEvent *>(event);
        if (eventKey->key() == Qt::Key_Backspace)
        {
            //获取光标的位置

            if (ui->remark->toPlainText() == QStringLiteral("备注："))
                return true;
        }
    } 
    else if (obj == ui->medicalHostory && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *eventKey = static_cast<QKeyEvent *>(event);
        if (eventKey->key() == Qt::Key_Backspace)
        {
            if (ui->medicalHostory->toPlainText() == QStringLiteral("既往病史："))
                return true;
        }
    } 
    else if (obj == ui->remark->viewport() && event->type() == QEvent::MouseButtonPress)
    {       
        QMouseEvent *mouseKey = static_cast<QMouseEvent *>(event);
        if (mouseKey->button() == Qt::LeftButton)
        {
            //找到光标位置
            QPoint cursor = mouseKey->pos();
            //找到字的范围
            QFontMetrics met = ui->remark->fontMetrics();
            QRect rect = met.boundingRect(ui->remark->viewport()->rect(), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("备注："));
            //内容边距
            QMargins m = ui->remark->contentsMargins();
            //[signal] void QTextEdit::copyAvailable(bool yes)
            //QFont QTextEdit::currentFont() const
            //cursor() const : QCursor
            if (rect.contains(cursor))
            {
                return true;
            }
        }
    }
    else if (obj == ui->medicalHostory->viewport() && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseKey = static_cast<QMouseEvent *>(event);
        if (mouseKey->button() == Qt::LeftButton)
        {
            //找到光标位置
            QPoint cursor = mouseKey->pos();
            //找到字的范围
            QFontMetrics met = ui->medicalHostory->fontMetrics();
            QRect rect = met.boundingRect(ui->medicalHostory->viewport()->rect(), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("既往病史："));
            if (rect.contains(cursor))
            {
                return true;
            }
        }
    }

    return Module::eventFilter(obj, event);
}

void ReportParticularsPatientInfoPrivate::paintPdf(QPainter & painter)
{    
    int relateHeight = y() + tabWidghtY;
    //报告详情
    drawText(painter, ui->label, relateHeight, 0, Qt::AlignLeft, 16);
    //出具时间
    drawText(painter, ui->label_3, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->provideTime, relateHeight, 0, Qt::AlignLeft, 12);
    //主治医生
    drawText(painter, ui->label_15, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->doctor, relateHeight, 0, Qt::AlignLeft, ui->doctor->text(), 0, 0, 12);
    //出具人
    drawText(painter, ui->label_5, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->provider, relateHeight, 0, Qt::AlignLeft, 12);
    //所在医院
    drawText(painter, ui->label_13, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->hospital, relateHeight, 0, Qt::AlignLeft, ui->hospital->text(), 0, 0, 12);
    //接收时间
    drawText(painter, ui->label_7, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->receiverTime, relateHeight, 0, Qt::AlignLeft, ui->receiverTime->text(), 0, 0, 12);
    //所在地
    drawText(painter, ui->label_16, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->address, relateHeight, 0, Qt::AlignLeft, ui->address->text(), 0, 0, 12);
    //审核人
    drawText(painter, ui->label_9, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->verifier, relateHeight, 0, Qt::AlignLeft, ui->verifier->text(), 0, 0, 12);
    //审核时间
    drawText(painter, ui->label_17, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->verifyTime, relateHeight, 0, Qt::AlignLeft, ui->verifyTime->text(), 0, 0, 12);

    //患者信息
    drawText(painter, ui->label_20, relateHeight, 0, Qt::AlignLeft, 16);
    //患者姓名
    drawText(painter, ui->label_21, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->patientName, relateHeight, 0, Qt::AlignLeft, 12);
    //身高
    drawText(painter, ui->label_23, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->height, relateHeight, 0, Qt::AlignLeft, ui->height->text(), 0, 0, 12);
    drawText(painter, ui->label_4, relateHeight, 0, Qt::AlignLeft, 12);
    //性别
    drawText(painter, ui->label_25, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->sex, relateHeight, 0, Qt::AlignLeft, 12);
    //体重
    drawText(painter, ui->label_27, relateHeight, 0, Qt::AlignLeft, 12);
    drawText1(painter, ui->weight, relateHeight, 0, Qt::AlignLeft, ui->weight->text(), 0, 0, 12);
    drawText(painter, ui->label_6, relateHeight, 0, Qt::AlignLeft, 12);
    //生日
    drawText(painter, ui->label_29, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->birthday, relateHeight, 0, Qt::AlignLeft, 12);

    //绘制圆角矩形
    drawRoundRect(painter, ui->medicalHostory, relateHeight, 8);
    drawRoundRect(painter, ui->remark, relateHeight, 8);
    //绘制内容
    drawText1(painter, ui->medicalHostory, relateHeight, 0, Qt::AlignLeft, ui->medicalHostory->toPlainText(), 2, 1, 12);
    drawText1(painter, ui->remark, relateHeight, 0, Qt::AlignLeft, ui->remark->toPlainText(), 1, 1, 12);
    //QString x = ui->medicalHostory->toPlainText();
    //int len = x.size();
    //QMessageBox::information(nullptr, QStringLiteral("病史备注"), QString::number(len));
    //QString x1 = ui->remark->toPlainText();
    //int len1 = x1.size();
    //QMessageBox::information(nullptr, QStringLiteral("患者信息备注"), QString::number(len1));
}


void FocalZonePrivate::paintPdf(QPainter & painter)
{
    int relateHeight = y() + tabWidghtY;
    //名称
    drawText(painter, ui->labelFocalZone, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->focalName, relateHeight, 0,  Qt::AlignLeft, 12);
    //诊断
    //drawText(painter, ui->label_2, relateHeight, 0, Qt::AlignLeft, 12);
    //drawText(painter, ui->diagnose, relateHeight, 0, Qt::AlignLeft, 12);
    //消融体积
    drawText(painter, ui->label, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->meltVolume, relateHeight, 0, Qt::AlignLeft, 12);
    //光纤使用数
    //drawText(painter, ui->label_7, relateHeight, 0, Qt::AlignLeft, 12);
    //drawText(painter, ui->opticalNum, relateHeight, 0, Qt::AlignLeft, 12);
    //绘制圆角矩形
    drawRoundRect(painter, ui->remark, relateHeight, 8);
    //绘制矩形中的内容
    drawText1(painter, ui->remark, relateHeight, 0, Qt::AlignLeft, ui->remark->toPlainText(), 1, 1, 12);
}

bool FocalZonePrivate::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == ui->remark && event->type() == QEvent::KeyPress)
    {        QKeyEvent *eventKey = static_cast<QKeyEvent *>(event);
        if (eventKey->key() == Qt::Key_Backspace)
        {
            if (ui->remark->toPlainText() == QStringLiteral("备注："))
                return true;
            //else if (ui->remark->textCursor().block().contains()) {

            //}
        }
    }
    else if (obj == ui->remark->viewport() && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseKey = static_cast<QMouseEvent *>(event);
        if (mouseKey->button() == Qt::LeftButton)
        {
            //找到光标位置
            QPoint cursor = mouseKey->pos();
            //找到字的范围
            QFontMetrics met = ui->remark->fontMetrics();
            QRect rect = met.boundingRect(ui->remark->viewport()->rect(), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("备注："));
            if (rect.contains(cursor))
            {
                return true;
            }
        }
    }

    return Module::eventFilter(obj, event);
}

bool OpticalPrivate::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == ui->textEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *eventKey = static_cast<QKeyEvent *>(event);
        if (eventKey->key() == Qt::Key_Backspace)
        {
            if (ui->textEdit->toPlainText() == QStringLiteral("备注："))
                return true;
        }
    }
    else if (obj == ui->textEdit->viewport() && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseKey = static_cast<QMouseEvent *>(event);
        if (mouseKey->button() == Qt::LeftButton)
        {
            //找到光标位置
            QPoint cursor = mouseKey->pos();
            //找到字的范围
            QFontMetrics met = ui->textEdit->fontMetrics();
            QRect rect = met.boundingRect(ui->textEdit->viewport()->rect(), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("备注："));
            if (rect.contains(cursor))
            {
                return true;
            }
        }
    }

    return Module::eventFilter(obj, event);
}


void OpticalPrivate::paintPdf(QPainter & painter)
{
    int relateHeight = y() + tabWidghtY;
    //光纤名称
    drawText(painter, ui->lblName, relateHeight, 0, Qt::AlignLeft, 12);
    //光纤型号
    drawText(painter, ui->label_3, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->model, relateHeight, 0, Qt::AlignLeft, 12);
    //出光次数
    drawText(painter, ui->label_5, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->num, relateHeight, 0, Qt::AlignLeft, 12);
    //预计移动距离
    //drawText(painter, ui->label_7, relateHeight, 0, Qt::AlignLeft, 12);
    //drawText(painter, ui->distance, relateHeight, 0, Qt::AlignLeft, 12);
    //头架类型
    drawText(painter, ui->label_2, relateHeight, 0, Qt::AlignLeft, 12);
    drawText(painter, ui->kind, relateHeight, 0, Qt::AlignLeft, 12);
    //绘制圆角矩形
    drawRoundRect(painter, ui->groupBox, relateHeight, 8);
    //绘制圆角矩形中的内容
    relateHeight = y() + tabWidghtY + ui->groupBox->y(); //由于绘制对象的父类改变，因此需要重新计算绝对位置
    int absoluteX = x() + tabWidgetX + ui->groupBox->x();
    //X
    drawText(painter, ui->label_4, relateHeight, absoluteX, Qt::AlignLeft, 12);
    drawText1(painter, ui->X, relateHeight, absoluteX, Qt::AlignLeft, ui->X->text(), 0, 0, 12);
    //Y
    drawText(painter, ui->label_6, relateHeight, absoluteX, Qt::AlignLeft, 12);
    drawText1(painter, ui->Y, relateHeight, absoluteX, Qt::AlignLeft, ui->Y->text(), 0, 0, 12);
    //Z
    drawText(painter, ui->label_8, relateHeight, absoluteX, Qt::AlignLeft, 12);
    drawText1(painter, ui->Z, relateHeight, absoluteX, Qt::AlignLeft, ui->Z->text(), 0, 0, 12);
    //ARC
    drawText(painter, ui->label_9, relateHeight, absoluteX, Qt::AlignLeft, 12);
    drawText1(painter, ui->ARC, relateHeight, absoluteX, Qt::AlignLeft, ui->ARC->text(), 0, 0, 12);
    //RAN
    drawText(painter, ui->label_10, relateHeight, absoluteX, Qt::AlignLeft, 12);
    drawText1(painter, ui->RAN, relateHeight, absoluteX, Qt::AlignLeft, ui->RAN->text(), 0, 0, 12);
    //备注
    drawText1(painter, ui->textEdit, relateHeight, absoluteX, Qt::AlignLeft, ui->textEdit->toPlainText(), 1, 1, 12);
}


void ImageTitlePrivate::paintPdf(QPainter & painter)
{
    int relateHeight = y() + tabWidghtY;
    drawText(painter, ui->label_2, relateHeight, 0, Qt::AlignLeft, 16);
}

void ImagePrivate::paintPdf(QPainter & painter)
{
    int relateHeight = y() + tabWidghtY;
    if (images.size() > 0)
    {
        drawImage(painter, ui->image1, relateHeight, images.at(0));
        drawText(painter, ui->label_2, relateHeight, 0, Qt::AlignLeft, 12);
        drawText1(painter, ui->remark1, relateHeight, 0, Qt::AlignLeft, ui->remark1->text(), 0, 0, 12);
    }

    if (images.size() > 1)
    {
        drawImage(painter, ui->image2, relateHeight, images.at(1));
        drawText(painter, ui->label, relateHeight, 0, Qt::AlignLeft, 12);
        drawText1(painter, ui->remark2, relateHeight, 0, Qt::AlignLeft, ui->remark2->text(), 0, 0, 12);
    }
}


void OpticalTitlePrivate::paintPdf(QPainter & painter)
{
    int relateHeight = y() + tabWidghtY;
    //光纤
    drawText(painter, ui->optical, relateHeight, 0, Qt::AlignLeft, 16);
}

void FocalZoneTitlePrivate::paintPdf(QPainter & painter)
{
    int relateHeight = y() + tabWidghtY;
    drawText(painter, ui->label_2, relateHeight, 0, Qt::AlignLeft, 16);
}

void OpticalBackwayPrivate::paintPdf(QPainter& painter)
{
    //准备文字
    QVector<QVector<QString>> table;
    int size = backwayVec.size();
    table.resize(size + 1);
    table[0].resize(2);
    table[0][0] = QStringLiteral("步骤");
    table[0][1] = QStringLiteral("距离");
    for (int row = 0; row < backwayVec.size(); row++)
    {
        table[row + 1].resize(2);
        table[row + 1][0] = backwayVec.at(row).name;
        table[row + 1][1] = backwayVec.at(row).distance;
    }

    //绘制（这里的尺寸是实际动态的）
    int relateHeight = y() + tabWidghtY + ui->tableWidget->y();
    int relateWidth = x() + tabWidgetX + ui->tableWidget->x();
    drawTable(painter, table, relateWidth, relateHeight, ui->tableWidget->width() / 2, OpticalBackwayCellHeight, Qt::AlignHCenter);
}
