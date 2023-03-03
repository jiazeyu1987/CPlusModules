#include "qGLPatientseri.h"
#include "ui_qGLPatientseri.h"
#include <QPainter>
#include "QTableWidget"
#include "qGLDb.h"
#include <qmessagebox.h>
#include "qSlicerIOManager.h"
#include "qJMainWindow.h"

PatientSeri::PatientSeri(QVector<Patient> patientVec, PatientsAnalyse pa, GLDb& db, QWidget* parent) :
	QDialog(parent), pa(pa), ui(new Ui::PatientSeri), patientVec(patientVec), db(db)
{
	emit sig_infoLog("患者序列-构造函数");
	icon = QIcon(":/GLExtensionPatient/Icons/allSelected.png");
	iconUnSel = QIcon(":/GLExtensionPatient/Icons/allUnSelected.png");
	ui->setupUi(this);
	//设置半透明背景 by zbs
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setWindowModality(Qt::ApplicationModal);
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	ui->tableWidget->horizontalHeader()->setFixedHeight(64);//设置表头高度
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);

	ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
	for (int row = 1; row < ui->tableWidget->rowCount(); row++)
	{
		ui->tableWidget->horizontalHeader()->setSectionResizeMode(row, QHeaderView::ResizeMode::Stretch);
	}
	for (int column = 0; column <= SliceCount; column++)
	{
		ui->tableWidget->horizontalHeaderItem(column)->setTextAlignment(Qt::AlignVCenter);
	}


	initTable();
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onCancel())); //取消
	connect(ui->btnquit, SIGNAL(clicked()), this, SLOT(onCancel())); //取消
	connect(ui->btnconfirm, SIGNAL(clicked()), this, SLOT(onConfirmSeries())); //确认
	connect(ui->tableWidget, &QTableWidget::itemClicked, this, &PatientSeri::slot_itemColumn0Clicked); //选中某行，只处理第一列
}

PatientSeri::~PatientSeri()
{
	emit sig_infoLog("患者序列-析构函数");
    delete ui;
}

void PatientSeri::onConfirmSeries()
{
	auto im = qSlicerApplication::application()->ioManager();
	qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());

	mw->ShowProgressBar(QStringLiteral("正在新建分析..."));
    emit sig_infoLog("患者序列-确认序列清单");
	//遍历第一列

	QVector<PatientsAnalyseSeries> anlseqListVec;
    QStringList filepathlist; //序列所在文件夹路径
	for (int row = 0; row < ui->tableWidget->rowCount(); row++)
	{
        QTableWidgetItem *item0 = ui->tableWidget->item(row, IsCheck);
		if (item0->data(checkRole).toBool()) //第一列被选中
		{
			//将数据插入到患者分析序列清单表
            PatientsAnalyseSeries list;
            list.patientAnalyseId = pa.patientAnalyseId; //患者分析id
            list.patientsSeriesId = item0->data(patientsSequenceListIdRole).toInt(); //患者序列id
			anlseqListVec.append(list);
            filepathlist.append(item0->data(filepathRole).toString());
		}
	}

    //将新建分析序列存入数据库
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("获取患者数据"), QStringLiteral("数据库连接失败，无法保存新建分析序列！"));
        return;
    }
	if (anlseqListVec.size() < 1)
    {
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("新建分析"), QStringLiteral("至少选择一个序列"));
        return;
    }

	for (int i = 0; i < anlseqListVec.size(); i++)
	{
		int num = db.setPatientsAnalyseSeries(anlseqListVec.at(i));
	}

	emit end_success(filepathlist, pa); //需要将数据传过去，数据应该包括图片路径，患者姓名，患者年龄和医嘱（患者备注）
	close();

}

//背景重绘
void PatientSeri::paintEvent(QPaintEvent * event)
{
	//QPainter painter(this);
	//painter.fillRect(this->rect(), QColor(0, 0, 0, 150));  //QColor最后一个参数代表背景的透明度
}

void PatientSeri::initTable()
{
	emit sig_infoLog("初始化序列，从数据库检索当前用户所有患者序列");
	ui->tableWidget->setRowCount(0); //清空表格
	QTableWidgetItem* item0 = ui->tableWidget->horizontalHeaderItem(0);
	item0->setData(checkRole, true);
	item0->setIcon(icon);
	connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &PatientSeri::onCancelOrSelectAll);
	GLDb db(this);
	if (!db.createDbConnect())
	{
		QMessageBox::warning(nullptr, QStringLiteral("获取患者数据"), QStringLiteral("数据库连接失败，无法获取患者图像序列！"));
		return;
	}

	//获取患者序列清单表-这是患者序列所有的数据
    QVector<Patient> patientVec = db.getPatientIdFromPatientAnalyseIdPatientIdId(pa);
    for (int i = 0; i < patientVec.size(); i++)
    {
        QVector<PatientsSeries> list = db.getPatientsSeries(patientVec.at(i));
		QVector<PatientsSeries> tmp;
		for (int j = 0; j < list.size(); j++) {
			auto item = list.at(j);
			auto path1 = item.seriesDir;
			QFileInfo info1(path1);
			if (!info1.isDir()) {
				continue;
			}
			patientsSeriesVec.append(item);
		}
		
    }

	for (int row = 0; row < patientsSeriesVec.size(); row++)
	{
		
		int rowcount = ui->tableWidget->rowCount();
		
		ui->tableWidget->insertRow(rowcount);

        //第一列
        QTableWidgetItem *item0 = new QTableWidgetItem();
		item0->setData(checkRole, patientsSeriesVec[row].enableSeries); //是否启用序列
        item0->setData(patientsSequenceListIdRole, patientsSeriesVec[row].patientsSeriesId);
        item0->setData(filepathRole, patientsSeriesVec[row].seriesDir);
        item0->setIcon(patientsSeriesVec[row].enableSeries ? icon : iconUnSel);
        ui->tableWidget->setItem(row, 0, item0);

		// Add Seri number
		QTableWidgetItem *itemSequenceNumber = new QTableWidgetItem(QString::number(patientsSeriesVec[row].seriesNumber));
		ui->tableWidget->setItem(row, SequenceNumber, itemSequenceNumber);
		// Add Scan time
		QTableWidgetItem *itemScanTime = new QTableWidgetItem(patientsSeriesVec[row].scanTime.left(10));
		ui->tableWidget->setItem(row, ScanTime, itemScanTime);
		// Add Seri Description
		QTableWidgetItem *itemSequenceDescribe = new QTableWidgetItem(patientsSeriesVec[row].seriesDescribe);
		ui->tableWidget->setItem(row, SequenceDescribe, itemSequenceDescribe);
		// Add Modalities
		QTableWidgetItem *itemModality = new QTableWidgetItem(patientsSeriesVec[row].modality);
		ui->tableWidget->setItem(row, Modalities, itemModality);
		// Add Slices
		QTableWidgetItem *itemsliceCount = new QTableWidgetItem(QString::number(patientsSeriesVec[row].sliceCount));
		ui->tableWidget->setItem(row, SliceCount, itemsliceCount);

        ui->tableWidget->setRowHeight(row, 60); //行高
	}
}

void PatientSeri::onCancelOrSelectAll(int column)
{
	if (column != 0)
		return;

	QTableWidgetItem* item0 = ui->tableWidget->horizontalHeaderItem(0);
	bool value = item0->data(checkRole).toBool();
	item0->setData(checkRole, !value);
	if (item0->data(checkRole).toBool())
	{
		item0->setIcon(icon);
		for (int row = 0; row < patientsSeriesVec.size(); row++)
		{
			//第一列
			ui->tableWidget->item(row, 0);
			QTableWidgetItem* item0_ = ui->tableWidget->item(row, 0);
			item0_->setData(checkRole, true); //是否启用序列
			item0_->setIcon(icon);
		}
	}
	else
	{
		item0->setIcon(iconUnSel);
		for (int row = 0; row < patientsSeriesVec.size(); row++)
		{
			//第一列
			ui->tableWidget->item(row, 0);
			QTableWidgetItem* item0_ = ui->tableWidget->item(row, 0);
			item0_->setData(checkRole, false); //是否启用序列
			item0_->setIcon(iconUnSel);
		}
	}
}


void PatientSeri::slot_itemColumn0Clicked(QTableWidgetItem *item)
{
    emit sig_infoLog("患者序列-选中第一列，设置是否勾选，设置不同图片");
	if (IsCheck != item->column())//判断是否第一列
		return;

	bool value = item->data(checkRole).toBool();
	if (value) //勾选ture 
	{
		item->setIcon(iconUnSel);
	}
	else //false
	{
		item->setIcon(icon);
	}
	item->setData(checkRole, !value);
}

void PatientSeri::onCancel()
{
	pa.enableColumn = false;
    GLDb db;
    if (db.createDbConnect())
        db.setPatientAnalyseEnable(pa);
    else
    {
        emit sig_infoLog("数据库连接失败");
    }
    close();
}

