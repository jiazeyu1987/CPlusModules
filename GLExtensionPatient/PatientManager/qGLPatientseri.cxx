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
	emit sig_infoLog("��������-���캯��");
	icon = QIcon(":/GLExtensionPatient/Icons/allSelected.png");
	iconUnSel = QIcon(":/GLExtensionPatient/Icons/allUnSelected.png");
	ui->setupUi(this);
	//���ð�͸������ by zbs
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setWindowModality(Qt::ApplicationModal);
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	ui->tableWidget->horizontalHeader()->setFixedHeight(64);//���ñ�ͷ�߶�
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);

	ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//��ͷ�������
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
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onCancel())); //ȡ��
	connect(ui->btnquit, SIGNAL(clicked()), this, SLOT(onCancel())); //ȡ��
	connect(ui->btnconfirm, SIGNAL(clicked()), this, SLOT(onConfirmSeries())); //ȷ��
	connect(ui->tableWidget, &QTableWidget::itemClicked, this, &PatientSeri::slot_itemColumn0Clicked); //ѡ��ĳ�У�ֻ�����һ��
}

PatientSeri::~PatientSeri()
{
	emit sig_infoLog("��������-��������");
    delete ui;
}

void PatientSeri::onConfirmSeries()
{
	auto im = qSlicerApplication::application()->ioManager();
	qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());

	mw->ShowProgressBar(QStringLiteral("�����½�����..."));
    emit sig_infoLog("��������-ȷ�������嵥");
	//������һ��

	QVector<PatientsAnalyseSeries> anlseqListVec;
    QStringList filepathlist; //���������ļ���·��
	for (int row = 0; row < ui->tableWidget->rowCount(); row++)
	{
        QTableWidgetItem *item0 = ui->tableWidget->item(row, IsCheck);
		if (item0->data(checkRole).toBool()) //��һ�б�ѡ��
		{
			//�����ݲ��뵽���߷��������嵥��
            PatientsAnalyseSeries list;
            list.patientAnalyseId = pa.patientAnalyseId; //���߷���id
            list.patientsSeriesId = item0->data(patientsSequenceListIdRole).toInt(); //��������id
			anlseqListVec.append(list);
            filepathlist.append(item0->data(filepathRole).toString());
		}
	}

    //���½��������д������ݿ�
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("��ȡ��������"), QStringLiteral("���ݿ�����ʧ�ܣ��޷������½��������У�"));
        return;
    }
	if (anlseqListVec.size() < 1)
    {
        GLMessageBox msg;
        msg.queRenWarn(QStringLiteral("�½�����"), QStringLiteral("����ѡ��һ������"));
        return;
    }

	for (int i = 0; i < anlseqListVec.size(); i++)
	{
		int num = db.setPatientsAnalyseSeries(anlseqListVec.at(i));
	}

	emit end_success(filepathlist, pa); //��Ҫ�����ݴ���ȥ������Ӧ�ð���ͼƬ·�����������������������ҽ�������߱�ע��
	close();

}

//�����ػ�
void PatientSeri::paintEvent(QPaintEvent * event)
{
	//QPainter painter(this);
	//painter.fillRect(this->rect(), QColor(0, 0, 0, 150));  //QColor���һ��������������͸����
}

void PatientSeri::initTable()
{
	emit sig_infoLog("��ʼ�����У������ݿ������ǰ�û����л�������");
	ui->tableWidget->setRowCount(0); //��ձ��
	QTableWidgetItem* item0 = ui->tableWidget->horizontalHeaderItem(0);
	item0->setData(checkRole, true);
	item0->setIcon(icon);
	connect(ui->tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &PatientSeri::onCancelOrSelectAll);
	GLDb db(this);
	if (!db.createDbConnect())
	{
		QMessageBox::warning(nullptr, QStringLiteral("��ȡ��������"), QStringLiteral("���ݿ�����ʧ�ܣ��޷���ȡ����ͼ�����У�"));
		return;
	}

	//��ȡ���������嵥��-���ǻ����������е�����
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

        //��һ��
        QTableWidgetItem *item0 = new QTableWidgetItem();
		item0->setData(checkRole, patientsSeriesVec[row].enableSeries); //�Ƿ���������
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

        ui->tableWidget->setRowHeight(row, 60); //�и�
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
			//��һ��
			ui->tableWidget->item(row, 0);
			QTableWidgetItem* item0_ = ui->tableWidget->item(row, 0);
			item0_->setData(checkRole, true); //�Ƿ���������
			item0_->setIcon(icon);
		}
	}
	else
	{
		item0->setIcon(iconUnSel);
		for (int row = 0; row < patientsSeriesVec.size(); row++)
		{
			//��һ��
			ui->tableWidget->item(row, 0);
			QTableWidgetItem* item0_ = ui->tableWidget->item(row, 0);
			item0_->setData(checkRole, false); //�Ƿ���������
			item0_->setIcon(iconUnSel);
		}
	}
}


void PatientSeri::slot_itemColumn0Clicked(QTableWidgetItem *item)
{
    emit sig_infoLog("��������-ѡ�е�һ�У������Ƿ�ѡ�����ò�ͬͼƬ");
	if (IsCheck != item->column())//�ж��Ƿ��һ��
		return;

	bool value = item->data(checkRole).toBool();
	if (value) //��ѡture 
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
        emit sig_infoLog("���ݿ�����ʧ��");
    }
    close();
}

