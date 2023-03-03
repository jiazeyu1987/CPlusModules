/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

#include <mitkVectorProperty.h>

// Qmitk
#include "patientMangeView.h"
#include "patientnewdialog.h"
#include "patientseri.h"
#include "internal/GLMessageDialog.h"
// Qt
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QDir>
#include <qdebug>
#include <qfiledialog.h>
#include <QDate>
#include <QContextMenuEvent>
#include <qbitarray.h>
#include "GLCustomDialog.h"
#include <qsettings.h>
#include <qtextcodec.h>
#include "GLPDF_operation.h"
#include <QDesktopServices>
#include "PatientAnalyzeItem.h"
#include "internal/DicomConflictEliminate.h"

// mitk image
#include <mitkImage.h>

#include "GLDb.h"
#include "dicom.h"
#include "GLCommonTool.h"
#include "GLTool.h"
#include "SurgeryPlanReportWidget.h"
//ITK
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <string>

//dcmtk
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>

const std::string patientMangeView::VIEW_ID = "org.gl.patientmangeview";

patientMangeView::patientMangeView() {}
patientMangeView::~patientMangeView() {}

void patientMangeView::SetFocus()
{
  // m_Controls.buttonPerformImageProcessing->setFocus();
}

void patientMangeView::CreateQtPartControl(QWidget *parent)
{
    MITK_INFO << "���߹�������ʼ��";
    parentWidget = parent;
    m_Controls.setupUi(parentWidget);
    m_Controls.lineEdit_search->setPlaceholderText(QStringLiteral("��������������"));
    m_Controls.pushButton0->hide(); //���������5����ť������
    m_Controls.pushButton1->hide();
    m_Controls.pushButton2->hide();
    m_Controls.pushButton3->hide();
    m_Controls.pushButton4->hide();

    widgetAll = new QWidget;
    m_Controls.scrollArea_3->setWidget(widgetAll);
    widgetPicture = new QWidget;
    m_Controls.scrollArea_2->setWidget(widgetPicture);
    widgetReport = new QWidget;
    m_Controls.scrollArea->setWidget(widgetReport);

    layoutAll = new QHBoxLayout;
    layoutPicture = new QHBoxLayout;
    layoutReport = new QHBoxLayout;
    widgetPicture->setLayout(layoutPicture);
    widgetReport->setLayout(layoutReport);
    widgetAll->setLayout(layoutAll);

    spacerItemAll = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacerItemPicture = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacerItemReport = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    table = new PatientTable(m_Controls.tableWidget);

    connect(m_Controls.btnLoad, &QPushButton::clicked, this, &patientMangeView::slot_btnLoadClick); //�������ݣ�һ����dicom���У�
    connect(m_Controls.btnSearch, &QPushButton::clicked, this, &patientMangeView::slot_btnSearch); //��ѯ���ص���������
    connect(m_Controls.comboBox, &QComboBox::currentTextChanged, this, [=]() { this->m_Controls.lineEdit_search->clear(); });

	btnVec.append(m_Controls.pushButton0);
	btnVec.append(m_Controls.pushButton1);
	btnVec.append(m_Controls.pushButton2);
	btnVec.append(m_Controls.pushButton3);
	btnVec.append(m_Controls.pushButton4);

	for (int i = 0; i < 5; i++) {
        setButtonDoubleClicked(btnVec.at(i), i);
	}
  
    //���ñ�����Ժ��źźͲ�
    m_Controls.tableWidget->horizontalHeader()->setFixedHeight(64); //�̶��߶�
    m_Controls.tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//���ÿ���Ctrl����shitfѡ�����ڱ��ϲ�
    m_Controls.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //���ò������༭ѡ��

    //m_Controls.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_Controls.tableWidget->horizontalHeader()->setSectionResizeMode(Report_, QHeaderView::ResizeToContents);
    m_Controls.tableWidget->horizontalHeader()->setStretchLastSection(true);

    connect(m_Controls.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int))); //
    connect(m_Controls.tableWidget, &QTableWidget::itemClicked, this, &patientMangeView::onItemClicked);
    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged); //������Ҫ

    //���ϲ��ͷ����Ҽ��˵�
    connect(m_Controls.tableWidget, &QTableWidget::customContextMenuRequested, this, &patientMangeView::slot_tableWidgetCustomContextMenuRequested);
    m_Controls.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //����кϲ��Ҽ��˵�
    pop_menu = new QMenu(m_Controls.tableWidget);
    actionMergeRow = new QAction(QStringLiteral("�ϲ�"), pop_menu);
    pop_menu->addAction(actionMergeRow);
    connect(actionMergeRow, &QAction::triggered, this, &patientMangeView::slot_actionMergeRowTriggered);
    actionSeparate = new QAction(QStringLiteral("����"), pop_menu);
    pop_menu->addAction(actionSeparate);
    connect(actionSeparate, &QAction::triggered, this, &patientMangeView::onActionSeparateRowTriggered);
   
    //����
    connect(m_Controls.tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &patientMangeView::onSort);
    //������ÿһsection���������ɫ����
    for (int i = 0; i < m_Controls.tableWidget->horizontalHeader()->count(); i++)
    {
        m_Controls.tableWidget->horizontalHeaderItem(i)->setData(userRoleSort, -1); //-1��ʾû������
    }  
    m_Controls.tableWidget->horizontalHeader()->setSortIndicatorShown(true);//��ʾ�����־
  
    loadPatientsToTableFromDb(); //�����ݿ���ر��
    loadRecentlyAnalysisFromDb(); //��������½����������5����
}

void patientMangeView::finished_success(QStringList list, PatientsAnalyse pa)
{
    MITK_INFO << "����½�����������ѡ��֮�󣬽���ͼ��ɸѡ";
	loadPatientsToTableFromDb(); //�����ݿ���ر��
	loadRecentlyAnalysisFromDb(); //��������½����������5����
    QList<QAction *> actList = GetToolBarActions();
    for each(auto act in actList)
    {
        act->setVisible(true);
    }

    auto windowbench = dynamic_cast<berry::Workbench*>(GetSite()->GetWorkbenchWindow()->GetWorkbench());
    auto windows = windowbench->GetWorkbenchWindows();
    berry::IWorkbenchWindow::Pointer iWind = windows.at(0);
    berry::WorkbenchWindow * wind = dynamic_cast<berry::WorkbenchWindow *>(iWind.GetPointer());
    wind->pSelfDefToolBar->setVisible(false);

    QList<QWidget *> widgets = wind->pSelfDefToolBar->findChildren<QWidget *>();
    for each (auto widget in widgets)
    {
        if (widget->objectName() == "toolLblPatientInfo")
        {
            QLabel * label = qobject_cast<QLabel *>(widget); //�����ȡ����ָ�룬����setTextʵ�����õ���toolLblPatientIfno(QLable)
            //label->setText(gdata1 + " (" + gdata4 + ")");
        }
    }

    //��Ҫ������Ϣ������ɸѡ����
    //���飬ֱ�ӽ�ͼƬ���ݴ���DataNode��Ȼ�����DataStorage

	SetGlobalValue("analyseId", pa.patientAnalyseId);
    //PDF_operation::instance()->output(); //���ڲ���
	SetGlobalValue("NewAnalysisForFilter", true);
	SetGlobalValue("NewAnalysisForModel", true);
	SetGlobalValue("newAnalysisFilePath", list.join(';').toStdString().c_str());

    //mitk::VectorProperty<std::string>::Pointer newAnalysisPathVec = mitk::VectorProperty<std::string>::New();
    //newAnalysisPathVec->SetValue(list.toVector().toStdVector())

	AddValueSignal("JumpPage", PICTURE_CHOOSE_PERSPECTIVE);

    PDF_operation::instance()->patientAnalyseId = pa.patientAnalyseId;
    PDF_operation::instance()->patientAnalyName = pa.analyseName;
    PDF_operation::instance()->patientName = pa.patientName;
    PDF_operation::instance()->patientBirthday = pa.birthday;
    PDF_operation::instance()->patientAge = pa.age;
    PDF_operation::instance()->patientSex = pa.sex;
    //PDF_operation::instance()->height = pa.height;
    //PDF_operation::instance()->weight = pa.weight;

    //SurgeryPlanReportWidget sprw;
    //return;

    berry::SmartPointer<berry::IWorkbenchPage> pPage = berry::PlatformUI::GetWorkbench()->ShowPerspective(PICTURE_CHOOSE_PERSPECTIVE,
        berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow()); //ͼ��ɸѡ����
}

//˫�����߹�������У����½�����ҳ
void patientMangeView::onCellDoubleClicked(int row)
{
	MITK_CUSTOM << "�����½��������棬�������ڵ��У�" << row;
    QVector<Patient> patientVec = table->getPatients(row); //һ������Ǻϲ��Ļ������ܰ����������ߣ���˰���������ߵ�����
    bool acceptOrCancel = true; //�ֲ�������Ҫ��ֵ��������ܳ������⣬����debug��release�汾��һ��
    PatientsAnalyse pa = getNewAnalysis(patientVec, &acceptOrCancel);
    if (acceptOrCancel)
    {
	    PatientNewDialog *pnd = new PatientNewDialog(patientVec, pa); //�½�����
	    connect(pnd, &PatientNewDialog::sig_PatientSeriConfirmClicked, this, &patientMangeView::finished_success);
        pnd->exec();
    }

}

void patientMangeView::slot_btnSearch() //ʹ��QList<QTableWidgetItem *> QTableWidget::findItems(const QString &text, Qt::MatchFlags flags) const�Ƚϼ�
{
    MITK_CUSTOM << "��������";
    QString searchStr = m_Controls.lineEdit_search->text().trimmed(); //��ȡ�����ַ���
    
    if (searchStr.isEmpty()) //�����������Ϊ�գ������������
    {
        loadPatientsToTableFromDb();
        return;
    }

    QVector<int> rowVec;
    QString searchType = m_Controls.comboBox->currentText(); //��ȡ��������
    //������������������Ϲ涨�Ļ�������
    for (size_t row = 0; row < m_Controls.tableWidget->rowCount(); row++)
    {
        QStringList search;
        if (searchType == QStringLiteral("��������"))
            search.append(m_Controls.tableWidget->item(row, PatientName)->text());
        else if (searchType.compare(QStringLiteral("����ID")) == 0)
            search.append(m_Controls.tableWidget->item(row, DicomPatientId)->text());
        else if (searchType == QStringLiteral("ģ������"))
        {
            search.append(m_Controls.tableWidget->item(row, PatientName)->text());
            search.append(m_Controls.tableWidget->item(row, DicomPatientId)->text());
            search.append(m_Controls.tableWidget->item(row, PatientBirthday)->text());
            search.append(m_Controls.tableWidget->item(row, PatientAge)->text());
            search.append(m_Controls.tableWidget->item(row, PatientSex)->text());
            search.append(m_Controls.tableWidget->item(row, RecordModificationTime)->text());
            search.append(m_Controls.tableWidget->item(row, AccessionNumber)->text());
            search.append(m_Controls.tableWidget->item(row, Modalities)->text());
            search.append(m_Controls.tableWidget->item(row, SurgerSummary)->text());
        }

        //������Ҫ����м��뵽������
        if (search.filter(searchStr).size() > 0)
        {
            rowVec.append(row);
        }
    }

    //��ʾ�µĽ��
    table->search(rowVec);
}

void patientMangeView::highlight(int row)
{
    MITK_INFO << "��ѡ�л��߱���ĳ��ʱ����ӱ���ͼƬ��������" << row;
    QLabel *L1 = new QLabel();

    L1->setPixmap(QPixmap(GLTool::imagesDir + "/linebg.png"));
    m_Controls.tableWidget->setCellWidget(row, 0, L1);
}

void patientMangeView::labelclear()
{
    for (int i = 0; i < labelAllVec.size(); i++)
    {
        delete labelAllVec[i];
    }

    for (int i = 0; i < labelPictureVec.size(); i++)
    {
        delete labelPictureVec[i];
    }

    for (int i = 0; i < labelAnalyseReportVec.size(); i++)
    {
        delete labelAnalyseReportVec[i];
    }

    labelAllVec.clear();
    labelPictureVec.clear();
    labelAnalyseReportVec.clear();

    layoutAll->removeItem(spacerItemAll);
    layoutPicture->removeItem(spacerItemPicture);
    layoutReport->removeItem(spacerItemReport);

    //��ն�Ӧ�ļ�ʱ������������������ʾ�����������
    clickTimerVec.resize(5);
    //��ն�Ӧ�ļ�ʱ��������������ʾ�����������
    for(int i = 5; i < timerVec.size(); i++)
    {      
        timerVec.at(i)->deleteLater();
    }
    timerVec.resize(5);
}

void patientMangeView::showScreetshotAndReport(int row)
{
    MITK_INFO << "��ʾ������ص�����ͼ�ͱ���ȣ�������: " << row;
    labelclear(); //���֮ǰ��ͼƬ

    m_Controls.tabWidget->setCurrentIndex(0); //Ĭ����ʾȫ��tabҳ

    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("��ȡ��������"), QStringLiteral("���ݿ�����ʧ�ܣ�"));
        return;
    }

    //���ǵ��ϲ��У�����һ��������ж��patientId������ѭ��
    QStringList patientIdList = table->getPatientId(row);
    QStringList imagesPathList;
    QStringList modalityList;
    QStringList sliceCountList;
    QStringList pixelList; //��λ
    QStringList describeList;
    for (int i = 0; i < patientIdList.size(); i++) //��ȡͼƬ����ʾ������
    {
        Patient patient;
        patient.patientId = patientIdList.at(i).toInt();
        QVector<PatientsSeries> patientsSeriesVec = db.getPatientsSeries(patient);
        
        for (size_t i = 0; i < patientsSeriesVec.length(); i++)
        {
            imagesPathList.append(patientsSeriesVec.at(i).screenshot);
            modalityList.append(patientsSeriesVec.at(i).modality);
            sliceCountList.append(QString::number(patientsSeriesVec.at(i).sliceCount));
            QString pixelSpacing = patientsSeriesVec.at(i).pixelSpacing;
            QString thickness = patientsSeriesVec.at(i).thickness;
            pixelList.append(pixelSpacing + "*" + thickness);
            describeList.append(patientsSeriesVec.at(i).scanTime.left(10));
        }
    }

    QVector<RowData> rdVec = table->getRowData(row);

    //��ʾͼ������
    for (int i = 0; i < imagesPathList.size(); i++)
    {
        QLabel* widget = new QLabel();
        widget->setFixedSize(336, 175);
        layoutPicture->addWidget(widget);
        labelPictureVec.append(widget);

        QLabel* image = new QLabel(widget); 
        QPixmap Pix = QPixmap(imagesPathList.at(i));
        QSize PixSize = image->size();
        Pix.scaled(PixSize, Qt::KeepAspectRatioByExpanding);
        image->setPixmap(Pix);
        image->setScaledContents(true);
        int width = widget->width();
        int height = widget->height();
        image->setGeometry(0, 0, width / 2, height);

        QString txt = QString(
            "<font color=\"#FFFFFF\" size=\"18px\">%1(%2)</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%3</font><br/>"
            "<font color=\"#7886B0\" size=\"18px\">%4</font>")
            .arg(modalityList.at(i)).arg(sliceCountList.at(i)).arg(pixelList.at(i)).arg(describeList.at(i));
        QLabel* illustrate = new QLabel(widget);
        illustrate->setTextFormat(Qt::RichText);
        illustrate->setText(txt);
        illustrate->setGeometry(width / 2, 0, width / 2, height);
    }
    layoutPicture->addItem(spacerItemPicture);

    //��ʾ����/����
    int n2 = clickTimerVec.size();
	QVector<int> patientAnalyseIdVec;
    for (int i = 0; i < rdVec.size(); i++)
    {
        QVector<PatientsAnalyse> paVec = rdVec.at(i).patientAnalyseVec; //��ȡ���У������Ǻϲ��еģ����л��߷���
		
        for (int j = 0; j < paVec.size(); j++)
        {
			if (!patientAnalyseIdVec.contains(paVec.at(j).patientAnalyseId)) //ȥ���ظ��Ĳ���
			{
				PatientAnalyseItem* item = new PatientAnalyseItem();
				connect(item, &PatientAnalyseItem::OpenAnalyByIndex, this, &patientMangeView::onOpenAnalySe);
				item->setFixedSize(185, 194);
				item->SetInfo(paVec.at(j));
				layoutReport->addWidget(item);
				labelAnalyseReportVec.append(item);

				patientAnalyseIdVec.append(paVec.at(j).patientAnalyseId);
			}
        }
    }
    layoutReport->addItem(spacerItemReport);

    //��ʾȫ���ļ�
    //����ʾͼ������
    for (int i = 0; i < imagesPathList.size(); i++)
    {
        QLabel* widget = new QLabel();
        widget->setFixedSize(336, 175);
        labelAllVec.append(widget);
        layoutAll->addWidget(widget);

        QLabel* image = new QLabel(widget);
        QPixmap Pix = QPixmap(imagesPathList.at(i));
        QSize PixSize = image->size();
        Pix.scaled(PixSize, Qt::KeepAspectRatioByExpanding);
        image->setPixmap(Pix);
        image->setScaledContents(true);
        int width = widget->width();
        int height = widget->height();
        image->setGeometry(0, 0, width / 2, height);

        QString txt = QString(
            "<font color=\"#FFFFFF\" size=\"18px\">%1(%2)</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%3</font><br/>"
            "<font color=\"#7886B0\" size=\"18px\">%4</font>")
            .arg(modalityList.at(i)).arg(sliceCountList.at(i)).arg(pixelList.at(i)).arg(describeList.at(i));
        QLabel* illustrate = new QLabel(widget);
        illustrate->setTextFormat(Qt::RichText);
        illustrate->setText(txt);
        illustrate->setGeometry(width / 2, 0, width / 2, height);
    }

    //����ʾ����/���棬ע��Qt��QObject֮��û�п����͸��ƣ���˿ؼ������Լ�����
    int n1 = clickTimerVec.size();
	QVector<int> patientAnalyseIdVec1;
    for (size_t i = 0; i < rdVec.size(); i++)
    {
        QVector<PatientsAnalyse> paVec = rdVec.at(i).patientAnalyseVec;		
        for (int j = 0; j < paVec.size(); j++)
        {
			if (!patientAnalyseIdVec1.contains(paVec.at(j).patientAnalyseId)) //ȥ���ظ��Ĳ���
			{
				PatientAnalyseItem* item = new PatientAnalyseItem();
				connect(item, &PatientAnalyseItem::OpenAnalyByIndex, this, &patientMangeView::onOpenAnalySe);
				item->setFixedSize(185, 194);
				item->SetInfo(paVec.at(j));
				layoutAll->addWidget(item);
				labelAllVec.append(item);

				patientAnalyseIdVec1.append(paVec.at(j).patientAnalyseId);
			}
        }
    }

    layoutAll->addItem(spacerItemAll);
}

//��Patients�м�����������
void patientMangeView::loadPatientsToTableFromDb()
{
    MITK_INFO << "��Patients���м�������";
    //�Ͽ�������Ҫ��Ԫ��༭���źźͲ۵����ӣ���Ϊ����������ݣ������еĹ����У�Ҳ�ᴥ���༭�Ĳ���
    //��ʱ�Ͽ�itemChanged�ź�����
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    //�����ݿ��ȡ����
    GLDb db(this);
    if (!db.createDbConnect())
    {
        
        GLMessageDialog::ShowWarnning(QStringLiteral("���߹���"), QStringLiteral("���ݿ�����ʧ�ܣ��޷������ݿ�������ݵ����߹�����棡"));
        return;
    }

    table->clearTable(); //�������
    table->patientVec = db.getPatients(GLDb::glAccount); //��ȡ��ǰ�û���ӵ�е����л���

    for (int i = 0; i < table->patientVec.length(); i++)
    {
        RowData rd; //����һ������

        rd.patientId = QString::number(table->patientVec[i].patientId);
        Patient patient;
        patient.patientId = rd.patientId.toInt();
        QVector<int> patientAnalyseIdVec = db.getPatientAnalyseIdFromPatientAnalyseIdPatientIdId(patient);
        QVector<PatientsAnalyse> patientAnalyseVec;
        for (int i = 0; i < patientAnalyseIdVec.size(); i++)
        {
            PatientsAnalyse pa = db.getPatientsAnalyse1(patientAnalyseIdVec.at(i));
            if (pa.patientAnalyseId != -1)
                patientAnalyseVec.append(pa);
        }
        rd.patientAnalyseVec = patientAnalyseVec; //�Ƿ�һ���Ի�ȡ�����ӳٻ�ȡ
        for (size_t i = 0; i < patientAnalyseVec.size(); i++)
        {
            if (!patientAnalyseVec.at(i).reportPath.isEmpty())
                rd.report.append(patientAnalyseVec.at(i).reportPath); //��ȡ����
        }

        rd.patientName = table->patientVec[i].patientName;
        rd.dicomPatientId = table->patientVec[i].dicomPatientId;
        rd.birthday = table->patientVec[i].birthday;
        rd.age = QString::number(GLTool::calAge(rd.birthday));
        rd.sex = table->patientVec[i].sex;
        rd.height = table->patientVec[i].height;
        rd.weight = table->patientVec[i].weight;
        rd.modifyDateTime = table->patientVec[i].recordModificationTime;
        
        //��ȡ���е�modality��accessionNumber
        QVector<PatientsSeries> patientsSeriesVec = db.getPatientsSeries(table->patientVec[i]); 

        QStringList accessionNumberList;
        QStringList modalityList;
        for (int i = 0; i < patientsSeriesVec.size(); i++)
        {
            accessionNumberList.append(patientsSeriesVec[i].accessionNumber);
            modalityList.append(patientsSeriesVec[i].modality);
        }

        accessionNumberList.removeDuplicates();
        rd.accessionNumber = accessionNumberList.join(",");

        modalityList.removeDuplicates();
        rd.modalities = modalityList.join(",");

        rd.surgrySummary = table->patientVec[i].surgerSummary;
        rd.mergeSign = table->patientVec.at(i).mergeSign;

        //�������ݼ��뵽table��ԭʼ�ֿ⣬ԭʼ�ֿ�����ݿ��ÿ��һһ��Ӧ
        int index = table->addRowDataToRootVec(rd);
        //���պϲ���־Ϊ�գ����Ȳ��룬��Ϊ�գ������hash���ɣ���ȻҲ����ͳһ���뵽hash��Ȼ��ͳһ����
        table->setMergeSignIndexHash(rd.mergeSign, index);
    }

    //����hash��rootIndexList
    table->mergeSignIndexHashToRootIndexList();

    //���ñ��
    table->updateTableWidget();

    resetColumnWidth();

    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);
}

//ȡ�����5���½�����
void patientMangeView::loadRecentlyAnalysisFromDb()
{
  MITK_INFO << "��ʼ���������";
  GLDb db(this);
  if (!db.createDbConnect())
  {
    QMessageBox::warning(nullptr, QStringLiteral("��ȡ�������"), QStringLiteral("���ݿ�����ʧ�ܣ��޷������������"));
    return;
  }

  QVector<PatientsAnalyse> patientsAnalyseVec = db.getPatientsAnalyse(5, 2);
  for (int i = 0; i < patientsAnalyseVec.size(); i++)
  {
    QVector<Patient> patientVec1 = db.getPatientIdFromPatientAnalyseIdPatientIdId(patientsAnalyseVec.at(i));
    QVector<Patient> patientVec;
    for (int i = 0; i < patientVec1.size(); i++)
    {
        Patient patient = db.getPatients(patientVec1.at(i).patientId);
        patientVec.append(patient);
    }

    QPushButton *parent = parentWidget->findChild<QPushButton *>(QString("pushButton%1").arg(i));
    if (parent)
    {
      //���������������
      createRecentlyAnalysisWidget(parent, patientsAnalyseVec.at(i).illnessKind, patientsAnalyseVec.at(i).analyseName,
          patientsAnalyseVec.at(i).patientName, patientsAnalyseVec.at(i).sex, patientsAnalyseVec.at(i).birthday, patientsAnalyseVec.at(i).doctorRemark,
        patientsAnalyseVec.at(i).changedTime);
      if (recentlyAnalysisHash.contains(parent))
          recentlyAnalysisHash.remove(parent);
      recentlyAnalysisHash.insert(parent, patientsAnalyseVec.at(i));
    }
  }
}

void patientMangeView::createRecentlyAnalysisWidget(QWidget * parent, QString diseaseName, 
    QString analysisName, QString patientName, QString sex, QString birthday, QString remark, QString createTime)
{
    //��������
    QLabel *labelDiseaseName = parent->findChild<QLabel *>("labelDiseaseName");
    if (!labelDiseaseName)
    {
        labelDiseaseName = new QLabel();
        labelDiseaseName->setObjectName("labelDiseaseName");
    }
    labelDiseaseName->setText(" " + diseaseName+ " ");
    labelDiseaseName->setStyleSheet("background-color: #3377FF; border-radius: 8px");

    //��������
    QLabel *labelAnalysisName = parent->findChild<QLabel *>("labelAnalysisName");
    if (!labelAnalysisName)
    {
        labelAnalysisName = new QLabel();
        labelAnalysisName->setObjectName("labelAnalysisName");
    }
    labelAnalysisName->setText(analysisName);

    //��������
    QLabel *labelPatientName = parent->findChild<QLabel *>("labelPatientName");
    if (!labelPatientName)
    {
        labelPatientName = new QLabel();
        labelPatientName->setObjectName("labelPatientName");
    }
    labelPatientName->setText(QStringLiteral("����: ") + GLCommonTool::GetDisplayPatientName(patientName));

    //�Ա�
    QLabel *labelSex = parent->findChild<QLabel *>("labelSex");
    if (!labelSex)
    {
        labelSex = new QLabel();
        labelSex->setObjectName("labelSex");
    }
    labelSex->setText(QStringLiteral("�Ա�: ") + sex);

    //����
    QLabel *labelBirthday = parent->findChild<QLabel *>("labelBirthday");
    if (!labelBirthday)
    {
        labelBirthday = new QLabel();
        labelBirthday->setObjectName("labelBirthday");
    }
    labelBirthday->setText(QStringLiteral("����: ") + birthday);

    //��ע
    QLabel *labelRemark = parent->findChild<QLabel *>("labelRemark");
    if (!labelRemark)
    {
        labelRemark = new QLabel();
        labelRemark->setObjectName("labelRemark");
    }
    labelRemark->setText(QStringLiteral("��ע: ") + remark);
    labelRemark->hide(); //��ע̫���ˣ���ʱ���ؿ�Ч��

    //����ʱ��
    QLabel *labelCreateTime = parent->findChild<QLabel *>("labelCreateTime");
    if (!labelCreateTime)
    {
        labelCreateTime = new QLabel();
        labelCreateTime->setObjectName("labelCreateTime");
    }
    labelCreateTime->setText(createTime);
    labelCreateTime->setStyleSheet("color: #7886B0");

    QHBoxLayout *layoutTop = new QHBoxLayout;
    layoutTop->addWidget(labelDiseaseName);
    layoutTop->addWidget(labelAnalysisName, 10);
    layoutTop->addStretch();

    QGridLayout *layoutMid = new QGridLayout;
    layoutMid->addWidget(labelPatientName, 0, 0);
    layoutMid->addWidget(labelSex, 0, 1);
    layoutMid->addWidget(labelBirthday, 1, 0);
    layoutMid->addWidget(labelRemark, 1, 1);

    QHBoxLayout *layoutBottom = new QHBoxLayout;
    layoutBottom->addWidget(labelCreateTime);
    layoutBottom->addStretch();

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addLayout(layoutTop);
    layoutMain->addLayout(layoutMid);
    layoutMain->addLayout(layoutBottom);

    parent->setLayout(layoutMain);
    parent->show();
    parent->setStyleSheet("background-color: #22305B; border: none;");
}

bool patientMangeView::eventFilter(QObject * obj, QEvent * event)
{
  if (event->type() == QEvent::ContextMenu)
  {
    QContextMenuEvent *event1 = static_cast<QContextMenuEvent *>(event);
    if (event1)
    {
      MITK_INFO << "�Ҽ��˵����ϲ���";
      Qt::ContextMenuPolicy policy = m_Controls.tableWidget->contextMenuPolicy();
      QPoint point = QCursor::pos();
      pop_menu->popup(point);
      //pop_menu->exec(point);
     // event1->accept();
      return true;
    }
  }
  return QObject::eventFilter(obj, event);
}

//�ϲ�
void patientMangeView::slot_actionMergeRowTriggered()
{  
    QVector<int> rowVec = getSelectedRow(); //�洢���е�����
    if (rowVec.size() < 2)
        return;

    //��ʱ�Ͽ�itemChanged�ź�����
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    //�ϲ�����������У�������ɾ��������
    qSort(rowVec); //�����Ա��ҵ����������
    QStringList patientIdList = table->merge(rowVec);
    table->updateTableWidget();

    //�������ݿ�
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("���߹���"), QStringLiteral("���ݿ�����ʧ�ܣ������ܽ��ϲ���Ϣ�������ݿ⣡"));
    }
    else
    {
        db.setPatientMergeSplitSign(patientIdList, patientIdList.join(","));
    }
    
    //�ָ��źŵ�����
    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);
}

//������
void patientMangeView::onActionSeparateRowTriggered()
{
    //��ʱ�Ͽ�itemChanged�ź�����
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    QVector<int> rowVec = getSelectedRow(); //�洢���е�����
    
    QStringList patientIdList = table->split(rowVec); //��ʼ����

    resetColumnWidth();

    //�������ݿ�
    GLDb db(this);
    if (!db.createDbConnect())
    {
        QMessageBox::warning(nullptr, QStringLiteral("���߹���"), QStringLiteral("���ݿ�����ʧ�ܣ������ܽ�������Ϣ�������ݿ⣡"));
    }
    else
    {
        db.setPatientMergeSplitSign(patientIdList, QString());
    }

    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);
}

void patientMangeView::slot_tableWidgetCustomContextMenuRequested(const QPoint & pos)
{
  pop_menu->popup(QCursor::pos());
  //pop_menu->exec(pos);
}

void patientMangeView::onSort(int column)
{
    QMultiMap<QString, int> map;
    for (int row = 0; row < m_Controls.tableWidget->rowCount(); row++)
    {
        QString txt = m_Controls.tableWidget->item(row, column)->text();
        map.insert(txt, row);
    }

    QMapIterator<QString, int> it(map);
    QVector<int> rowVec;
    while (it.hasNext())
    {
        it.next();
        rowVec.append(it.value());
    }

    int value = m_Controls.tableWidget->horizontalHeaderItem(column)->data(userRoleSort).toInt();
    if (value == -1 || value == Qt::DescendingOrder)
    {
        m_Controls.tableWidget->horizontalHeader()->setSortIndicator(column, Qt::AscendingOrder);
        //m_Controls.tableWidget->sortByColumn(column);
        //�Ӵ�С���У���ת
        int a = 0, b = 0;
        int size = rowVec.size();
        for (int i = 0; i < size; i++)
        {
            if (size - i - 1 <= i)
                break;
            int a = rowVec.at(i);
            rowVec[i] = rowVec[size - i - 1];
            rowVec[size - i - 1] = a;
        }
        table->sort(rowVec);
        m_Controls.tableWidget->horizontalHeaderItem(column)->setData(userRoleSort, Qt::AscendingOrder);
    }

    if (value == Qt::AscendingOrder)
    {
        m_Controls.tableWidget->horizontalHeader()->setSortIndicator(column, Qt::DescendingOrder);
        //m_Controls.tableWidget->sortByColumn(column);
        table->sort(rowVec);
        m_Controls.tableWidget->horizontalHeaderItem(column)->setData(userRoleSort, Qt::DescendingOrder);
    }
}

void patientMangeView::onOpenReport()
{
    QObject * obj = sender();
    if (obj)
    {
        QString report = obj->property(reportProperty).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(report));
    }  
}

void patientMangeView::resetColumnWidth()
{
    int total = m_Controls.tableWidget->width() - m_Controls.tableWidget->columnWidth(Report_);
    int columnWidth = total / SurgerSummary;
    for (int  column = PatientName; column < m_Controls.tableWidget->colorCount() - 1; column++)
    {
        m_Controls.tableWidget->setColumnWidth(column, columnWidth);
    }
}

void patientMangeView::setButtonDoubleClicked(QPushButton * btn, int i)
{
    clickTimerVec.append(0);
    connect(btn, &QPushButton::clicked, this, [=]()
    {
        clickTimerVec[i]++; //���һ�μ�ʱ
        if (clickTimerVec[i] >= 2)
        {
            onPushButtonXDoubleClicked(btn);
            btn->setEnabled(false);
        }
        if (timerVec[i]->isActive() == false)
            timerVec[i]->start(350);
    });
    QTimer *timer = new QTimer();
    timerVec.append(timer);
    connect(timer, &QTimer::timeout, this, [=]() {
        clickTimerVec[i] = 0;
        timer->stop();
        btn->setEnabled(true);
    });
}

PatientsAnalyse patientMangeView::getNewAnalysis(QVector<Patient> patientVec, bool* acceptOrCancel)
{
    QStringList nameList;
    QStringList birthList;
    QStringList sexList;
    QStringList ageList;
    QStringList heightList;
    QStringList weightList;
    for (int i = 0; i < patientVec.size(); i++)
    {
        nameList.append(GLCommonTool::GetDisplayPatientName(patientVec.at(i).patientName));
        birthList.append(patientVec.at(i).birthday);
        sexList.append(patientVec.at(i).sex);
        ageList.append(patientVec.at(i).age);
        heightList.append(patientVec.at(i).height);
        weightList.append(patientVec.at(i).weight);
    }
    nameList.removeDuplicates();
    birthList.removeDuplicates();
    sexList.removeDuplicates();
    ageList.removeDuplicates();
    heightList.removeDuplicates();
    weightList.removeDuplicates();

    PatientsAnalyse pa;
    if (nameList.size() > 1 || birthList.size() > 1 || sexList.size() > 1/* || heightList.size() > 1 || weightList.size() > 1*/)
    {
        Patient patient;        
        DicomConfilctEliminate dce(patientVec, patient, QStringLiteral("�ϲ�֮�����ֶ�ѡ��"), acceptOrCancel);
        pa.patientName = patient.patientName;
        pa.birthday = patient.birthday;
        pa.sex = patient.sex;
        pa.age = QString::number(GLTool::calAge(pa.birthday));
        pa.height = patient.height;
        pa.weight = patient.weight;
    }
    else
    {
        pa.patientName = nameList.join(",");
        pa.birthday = birthList.join(",");
        pa.sex = sexList.join(",");
        pa.age = QString::number(GLTool::calAge(pa.birthday));
        pa.height = heightList.join(",");
        pa.weight = weightList.join(",");
    }

    return pa;
}

void patientMangeView::onPushButtonXDoubleClicked(QPushButton *btn)
{
  PatientsAnalyse pa = recentlyAnalysisHash.value(btn);

  onOpenAnalySe(pa);
}

QVector<int> patientMangeView::getSelectedRow()
{
    QList<QTableWidgetSelectionRange> ranges = m_Controls.tableWidget->selectedRanges(); //��ȡ����ѡ����

    QVector<int> rowVec; //�洢���е�����
    for (int i = 0; i < ranges.size(); i++)
    {
        QTableWidgetSelectionRange range = ranges.at(i);//��ȡÿһ��range
        for (int row = range.topRow(); row <= range.bottomRow(); row++)
        {
            rowVec.append(row);
        }
    }

    return rowVec;
}

QString patientMangeView::getRecentDir()
{
    QSettings set(GLTool::curDir + "/configure/cfg.ini", QSettings::IniFormat);
    set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
    QString dir = set.value("PatientManager/importRecentDir").toString();
    if (dir.isEmpty())
        return QDir::currentPath();
    return dir;
}

void patientMangeView::saveRecentDir(QString dir)
{
    QString curPath = qApp->applicationDirPath();
    QSettings set(curPath + "/configure/cfg.ini", QSettings::IniFormat);
    set.setIniCodec(QTextCodec::codecForName("GBK")); //��ֹ��������
    set.setValue("PatientManager/importRecentDir", dir);
}

void patientMangeView::onOperationSummaryItemChanged(QTableWidgetItem * item)
{
  if (item->column() != SurgerSummary)
    return;

  QWidget *label = m_Controls.tableWidget->cellWidget(item->row(), Report_);
  if (!label)
      return;

  Patient patient;
  patient.patientId = table->rootVec.at(table->rootIndexList.at(item->row()).at(0)).patientId.toInt();
  QString operationSummary = item->text();
  patient.surgerSummary = operationSummary;

  //�������ݿ�
  GLDb db(this);
  if (!db.createDbConnect())
  {
    QMessageBox::warning(nullptr, QStringLiteral("���ݿ�"), QStringLiteral("���ݿ�����ʧ�ܣ�"));
    return;
  }
  db.setSurgerySummary(patient);
}

void patientMangeView::onOpenAnalySe(PatientsAnalyse analy)
{
	GLDb db(this);
	if (!db.createDbConnect())
	{
		QMessageBox::warning(nullptr, QStringLiteral("��ȡ��������"), QStringLiteral("���ݿ�����ʧ�ܣ��޷���ȡ����ͼ�����У�"));
		return;
	}
	//��ȡ���������嵥��-���ǻ����������е�����
	QVector<PatientsAnalyseSeries> patientsSeriesVec = db.getPatientAnalyseSeries(analy);

	QStringList list;
	QVector<PatientsSeries> vec = db.getPatientsSeries(patientsSeriesVec);
	for (int i = 0; i < vec.size(); i++)
	{
		list.append(vec.at(i).seriesDir);
	}

	finished_success(list, analy);
}

void patientMangeView::onItemClicked(QTableWidgetItem *item)
{
  if (item->column() == SurgerSummary)
    m_Controls.tableWidget->editItem(item);
  else
      showScreetshotAndReport(item->row());
}

void patientMangeView::slot_btnLoadClick()
{
    MITK_CUSTOM << "���뻼����Ϣ��DICOM�ļ���";
    QString path = QFileDialog::getExistingDirectory(parentWidget, QStringLiteral("ѡ��һ��DICOM����Ŀ¼"), getRecentDir()); //����Դ�������л�ȡDICOM�ļ�����
    if (path.isEmpty())
        return;
    Dicom dicom;
    dicom.itkReadDicomSeriesAnyDir(path);

    loadPatientsToTableFromDb(); //Ŀǰ���õ���һ��dicom���ʹ����ݿ���ȫ�����أ�����Ȼ���Ա�֤һ���ԣ������ܻ�Ӱ������

    saveRecentDir(path);
}
