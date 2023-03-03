#include "qGLPatientMangeView.h"
#include "qGLPatientnewdialog.h"
#include "qGLPatientseri.h"
#include "qGLDb.h"
#include "qGLDicom.h"
#include "qGLTool.h"
#include <qGLlog.h>
#include "qSlicerPatientManagerModuleWidget.h"
#include "qPatientAnalyzeItem.h"
#include "DicomConflictEliminate.h"
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
#include <QMenu>
#include <qsettings.h>
#include <qtextcodec.h>
#include <QDesktopServices>
#include "qGLMessageDialog.h"
#include "qGLCustomProgressBar.h"
#include <qdesktopwidget.h>

#include <Windows.h>
#include <winioctl.h>

//ITK
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <string>

//dcmtk
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmf.h>

#include "qSlicerIOManager.h"
#include "qJMainWindow.h"
patientMangeView::patientMangeView(Ui_patientMange& m_Controls, qSlicerPatientManagerModuleWidget* patientManagerMoudleWidget)
    : QObject(nullptr), m_Controls(m_Controls), patientManagerMoudleWidget(patientManagerMoudleWidget), parentWidget(patientManagerMoudleWidget)
{
    //Log* instance = Log::instance();
    //QObject::connect(this, &patientMangeView::sig_infoLog, instance, &Log::appendInfoLog);
    CreateQtPartControl(patientManagerMoudleWidget);
    
}
patientMangeView::~patientMangeView() {}


void patientMangeView::CreateQtPartControl(QWidget *parent)
{
    emit sig_infoLog("���߹�������ʼ��");
    parentWidget = parent;
    parent->installEventFilter(this); //QObject�����������
    m_Controls.tableWidget->setRowCount(0);
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
    connect(m_Controls.btnLoadMITK, &QPushButton::clicked, this, &patientMangeView::OnLoadMitkClick);
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
    m_Controls.tableWidget->horizontalHeader()->setSectionResizeMode(Report_, QHeaderView::ResizeToContents);
    m_Controls.tableWidget->horizontalHeader()->setStretchLastSection(true);

    connect(m_Controls.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int))); //
    connect(m_Controls.tableWidget, &QTableWidget::itemClicked, this, &patientMangeView::onItemClicked);
    connect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

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
  
    //loadPatientsToTableFromDb(); //�����ݿ���ر��
    //loadRecentlyAnalysisFromDb(); //��������½�����������5����
}

void patientMangeView::finished_success(QStringList list, PatientsAnalyse pa)
{
    emit sig_infoLog("����½�����������ѡ��֮�󣬽���ͼ��ɸѡ ");
    //loadPatientsToTableFromDb(); //�����ݿ���ر��
    //loadRecentlyAnalysisFromDb();
    //��Ҫ������Ϣ������ɸѡ����
    //���飬ֱ�ӽ�ͼƬ���ݴ���DataNode��Ȼ�����DataStorage
    QString newAnalysisImagePath = list.join(';');
    patientManagerMoudleWidget->AddSignalNode("newAnalysisFilePath", newAnalysisImagePath.toStdString().c_str());   
    patientManagerMoudleWidget->AddSignalNode("patientAnalyseId", QString::number(pa.patientAnalyseId).toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("newAnalysisFilePath", newAnalysisImagePath.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientAnalyseId", QString::number(pa.patientAnalyseId).toStdString().c_str());
    if (pa.is_package) {
        patientManagerMoudleWidget->SetGlobalSaveValue("init_analyse", "2");
    }
    else {
        patientManagerMoudleWidget->SetGlobalSaveValue("init_analyse", "1");
    }
    
    patientManagerMoudleWidget->SetGlobalSaveValue("patientName", pa.patientName.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientBirthday", pa.birthday.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("analyseName", pa.analyseName.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientAge", pa.age.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("patientSex", pa.sex.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("doctorRemark", pa.doctorRemark.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("illnessKind", pa.illnessKind.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("remark", pa.remark.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("weight", pa.weight.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("height", pa.height.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("createTime", pa.createTime.toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("changedTime", pa.changedTime.toStdString().c_str());

    GLTool::patientAnalyseId = pa.patientAnalyseId;
    GLTool::patientAnalyName = pa.analyseName;
    GLTool::patientName = pa.patientName;
    GLTool::patientBirthday = pa.birthday;

    GLTool::patientAge = pa.age;
    GLTool::patientSex = pa.sex;
    
    emit sig_nextPage();
    
}

void patientMangeView::load_package_file(int pid)
{
    patientManagerMoudleWidget->SetGlobalSaveValue("patientAnalyseId", QString::number(pid).toStdString().c_str());
    patientManagerMoudleWidget->SetGlobalSaveValue("init_analyse", "2");
    emit sig_nextPage();
}

//˫�����߹�������У����½�����ҳ
void patientMangeView::onCellDoubleClicked(int row)
{
	emit sig_customLog("�����½��������棬�������ڵ��У�" + QString::number(row));
    QVector<Patient> patientVec = table->getPatients(row); //һ������Ǻϲ��Ļ������ܰ����������ߣ���˰���������ߵ�����
    bool acceptOrCancel = true; //�ֲ�������Ҫ��ֵ��������ܳ������⣬����debug��release�汾��һ��
    PatientsAnalyse pa = getNewAnalysis(patientVec, &acceptOrCancel);
    if (acceptOrCancel)
    {
        PatientNewDialog* pnd = new PatientNewDialog(patientVec, pa); //�½�����
        pnd->setWindowModality(Qt::ApplicationModal);
        connect(pnd, &PatientNewDialog::sig_PatientSeriConfirmClicked, this, &patientMangeView::finished_success);
        pnd->exec();
    }
}
    
void patientMangeView::slot_btnSearch() //ʹ��QList<QTableWidgetItem *> QTableWidget::findItems(const QString &text, Qt::MatchFlags flags) const�Ƚϼ�
{
    emit sig_customLog("��������");
    QString searchStr = m_Controls.lineEdit_search->text().trimmed(); //��ȡ�����ַ���

    if (searchStr.isEmpty()) //�����������Ϊ�գ������������
    {
        loadPatientsToTableFromDb();
        return;
    }

    QVector<int> rowVec;
    QString searchType = m_Controls.comboBox->currentText().trimmed(); //��ȡ��������
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
    emit sig_infoLog("��ѡ�л��߱���ĳ��ʱ����ӱ���ͼƬ��������" + QString::number(row));
    QLabel *L1 = new QLabel();

    L1->setPixmap(QPixmap(":/Icons/linebg.png"));
    m_Controls.tableWidget->setCellWidget(row, 0, L1);
}

void patientMangeView::labelclear()
{
    emit sig_infoLog("���ͼƬ ");
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
    for (int i = 5; i < timerVec.size(); i++)
    {
        timerVec.at(i)->deleteLater();
    }
    timerVec.resize(5);
}

void patientMangeView::showScreetshotAndReport(int row)
{
    emit sig_infoLog("��ʾ������ص�����ͼ�ͱ���ȣ�������: " + QString::number(row));
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
    QStringList datetimeList;
    for (int i = 0; i < patientIdList.size(); i++) //��ȡͼƬ����ʾ������
    {
        Patient patient;
        patient.patientId = patientIdList.at(i).toInt();
        QVector<PatientsSeries> patientsSeriesVec = db.getPatientsSeries(patient);

        for (size_t i = 0; i < patientsSeriesVec.length(); i++)
        {
            imagesPathList.append(patientsSeriesVec.at(i).screenshot);

            QString seriesDescribe = patientsSeriesVec.at(i).seriesDescribe;
            describeList.append(seriesDescribe); //��������

            //ģ̬����Ƭ����
            modalityList.append(patientsSeriesVec.at(i).modality);
            sliceCountList.append(QString::number(patientsSeriesVec.at(i).sliceCount));

            //���
            QString pixelSpacing = patientsSeriesVec.at(i).pixelSpacing;
            QString thickness = patientsSeriesVec.at(i).thickness;
            int index = thickness.lastIndexOf(QChar('.'));
            if (index != -1)
                thickness = thickness.left(index + 3);
            QStringList list = pixelSpacing.split("*");
            if (list.size() == 2) //����С����2λ
            {
                QString space0 = list.at(0);
                QString space1 = list.at(1);

                int index0 = space0.lastIndexOf(QChar('.'));
                if (index0 != -1)
                    space0 = space0.left(index0 + 3);
                int index1 = space1.lastIndexOf(QChar('.'));
                if (index1 != -1)
                    space1 = space1.left(index1 + 3);

                pixelList.append(space0 + "*" + space1 + "*" + thickness);
            }
            else
                pixelList.append(pixelSpacing + "*" + thickness);

            //ɨ��ʱ��
            datetimeList.append(patientsSeriesVec.at(i).scanTime.left(10));
        }
    }

    QVector<RowData> rdVec = table->getRowData(row);

    //��ʾͼ������
    for (int i = 0; i < imagesPathList.size(); i++)
    {
        auto path1 = imagesPathList.at(i);
        QFileInfo qfinfo(path1);
        if (qfinfo.isFile() || qfinfo.isDir()) {
        
        }
        else {
            continue;
        }
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
            "<font color=\"#FFFFFF\" size=\"18px\">%1</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%2(%3)</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%4</font><br/>"
            "<font color=\"#7886B0\" size=\"18px\">%5</font>")
            .arg(describeList.at(i)).arg(modalityList.at(i)).arg(sliceCountList.at(i)).arg(pixelList.at(i)).arg(datetimeList.at(i));
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
        auto path1 = imagesPathList.at(i);
        QFileInfo qfinfo(path1);
        if (qfinfo.isFile() || qfinfo.isDir()) {

        }
        else {
            continue;
        }
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
            "<font color=\"#FFFFFF\" size=\"18px\">%1</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%2(%3)</font><br/>"
            "<font color=\"#FFFFFF\" size=\"18px\">%4</font><br/>"
            "<font color=\"#7886B0\" size=\"18px\">%5</font>")
            .arg(describeList.at(i)).arg(modalityList.at(i)).arg(sliceCountList.at(i)).arg(pixelList.at(i)).arg(datetimeList.at(i));
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
    emit sig_infoLog("��Patients���м�������");
    //�Ͽ�������Ҫ��Ԫ��༭���źźͲ۵����ӣ���Ϊ����������ݣ������еĹ����У�Ҳ�ᴥ���༭�Ĳ���
    //��ʱ�Ͽ�itemChanged�ź�����
    disconnect(m_Controls.tableWidget, &QTableWidget::itemChanged, this, &patientMangeView::onOperationSummaryItemChanged);

    //�����ݿ��ȡ����
    GLDb db(this);
    if (!db.createDbConnect())
    {
        GLMessageBox msg;
        msg.queRenError(QStringLiteral("���߹���"), QStringLiteral("���ݿ�����ʧ�ܣ��޷������ݿ�������ݵ����߹�����档"));
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
            if (pa.patientAnalyseId != -1) {
                QString curDirPath = QCoreApplication::applicationDirPath();
                QString backupPath = QStringLiteral("%1/%2").arg(curDirPath).arg("bak");
                auto fileName = QStringLiteral("%1/%2.mrb").arg(backupPath).arg(pa.patientAnalyseId);
                QFileInfo qfile(fileName);
                if (!qfile.isFile()) {
                    continue;
                }
                patientAnalyseVec.append(pa);
            }
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
    emit sig_infoLog("��ʼ���������");
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

        //�˷�����mitk�汾������ȷ�ҵ�pushButton������slicer�汾������
        //QPushButton* parent = parentWidget->findChild<QPushButton*>(QString("pushButton%1").arg(i));
        //����ķ�������
        //QPushButton* parent = m_Controls.groupBox->findChild<QPushButton*>(QString("pushButton%1").arg(i));
        //��Ϊ�˸��ɿ���ֱ��ʹ��m_Controls.pushButtonX
        if (btnVec.at(i))
        {
            //���������������
            createRecentlyAnalysisWidget(btnVec.at(i), patientsAnalyseVec.at(i).illnessKind, patientsAnalyseVec.at(i).analyseName,
                patientsAnalyseVec.at(i).patientName, patientsAnalyseVec.at(i).sex, patientsAnalyseVec.at(i).birthday, patientsAnalyseVec.at(i).doctorRemark,
                patientsAnalyseVec.at(i).changedTime);
            if (recentlyAnalysisHash.contains(btnVec.at(i)))
                recentlyAnalysisHash.remove(btnVec.at(i));
            recentlyAnalysisHash.insert(btnVec.at(i), patientsAnalyseVec.at(i));
        }
    }
}

void patientMangeView::createRecentlyAnalysisWidget(QWidget * parent, QString diseaseName, 
    QString analysisName, QString patientName, QString sex, QString birthday, QString remark, QString createTime)
{
    //��������
    QLabel* labelDiseaseName = parent->findChild<QLabel*>("labelDiseaseName");
    if (!labelDiseaseName)
    {
        labelDiseaseName = new QLabel();
        labelDiseaseName->setObjectName("labelDiseaseName");
    }
    labelDiseaseName->setText(diseaseName);
    labelDiseaseName->setStyleSheet("background-color: #3377FF; border-radius: 8px");

    //��������
    QLabel* labelAnalysisName = parent->findChild<QLabel*>("labelAnalysisName");
    if (!labelAnalysisName)
    {
        labelAnalysisName = new QLabel();
        labelAnalysisName->setObjectName("labelAnalysisName");
    }
    labelAnalysisName->setText(analysisName);

    //��������
    QLabel* labelPatientName = parent->findChild<QLabel*>("labelPatientName");
    if (!labelPatientName)
    {
        labelPatientName = new QLabel();
        labelPatientName->setObjectName("labelPatientName");
    }
    labelPatientName->setText(QStringLiteral("����: ") + patientName.split(":")[0].trimmed());

    //�Ա�
    QLabel* labelSex = parent->findChild<QLabel*>("labelSex");
    if (!labelSex)
    {
        labelSex = new QLabel();
        labelSex->setObjectName("labelSex");
    }
    labelSex->setText(QStringLiteral("  �Ա�: ") + sex);

    //����
    QLabel* labelBirthday = parent->findChild<QLabel*>("labelBirthday");
    if (!labelBirthday)
    {
        labelBirthday = new QLabel();
        labelBirthday->setObjectName("labelBirthday");
    }
    labelBirthday->setText(QStringLiteral("����: ") + birthday);

    //��ע
    QLabel* labelRemark = parent->findChild<QLabel*>("labelRemark");
    if (!labelRemark)
    {
        labelRemark = new QLabel();
        labelRemark->setObjectName("labelRemark");
    }
    labelRemark->setText(QStringLiteral("��ע: ") + remark);
    labelRemark->hide(); //��ע̫���ˣ���ʱ���ؿ�Ч��

    //����ʱ��
    QLabel* labelCreateTime = parent->findChild<QLabel*>("labelCreateTime");
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

bool patientMangeView::eventFilter(QObject* obj, QEvent* event)
{
	//if (event->type() == QEvent::ContextMenu)
	//{
	//	QContextMenuEvent* event1 = static_cast<QContextMenuEvent*>(event); //�����Ĳ˵��¼�
	//	if (event1)
	//	{
	//		emit sig_infoLog("�Ҽ��˵����ϲ���");
	//		Qt::ContextMenuPolicy policy = m_Controls.tableWidget->contextMenuPolicy();
	//		QPoint point = QCursor::pos();
	//		pop_menu->popup(point);
	//		//pop_menu->exec(point);
	//	   // event1->accept();
	//		return true;
	//	}
	//}
    if (event->type() == QEvent::Show)
    {
        if (obj == parentWidget)
        {
            loadPatientsToTableFromDb(); //�����ݿ���ر��
            loadRecentlyAnalysisFromDb(); //��������½����������5����
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

void patientMangeView::OnLoadMitkClick()
{
    //QString fileName = "";
    //QString curPath = QDir::currentPath();
    //fileName = QFileDialog::getOpenFileName(nullptr, QStringLiteral("�򿪷����ļ�"), curPath, "mitk File(*.mitk)");
    //if (fileName == "") {
    //    GLMessageDialog::ShowInformation(QStringLiteral("�������"), QStringLiteral("����ʧ��, ��ѡ��һ����Ч�ķ����ļ���"));
    //    return;
    //}
    ////-99 means to open a mitk file without analyse
    //SetGlobalValue("analyseId", -99);
    //SetGlobalValue("NewAnalysisForFilter", true);
    //SetGlobalValue("NewAnalysisForModel", true);
    //SetGlobalValue("NewAnalysisForTools", true);
    //SetGlobalValue("newAnalysisFilePath", fileName.toStdString());

    //berry::SmartPointer<berry::IWorkbenchPage> pPage = berry::PlatformUI::GetWorkbench()->ShowPerspective(PICTURE_CHOOSE_PERSPECTIVE,
    //    berry::PlatformUI::GetWorkbench()->GetActiveWorkbenchWindow()); //ͼ��ɸѡ����
}


void patientMangeView::resetColumnWidth()
{
    int total = m_Controls.tableWidget->width() - m_Controls.tableWidget->columnWidth(Report_);
    int columnWidth = total / SurgerSummary;
    for (int column = PatientName; column < m_Controls.tableWidget->columnCount() - 1; column++)
    {
        m_Controls.tableWidget->setColumnWidth(column, columnWidth);
    }
}

void patientMangeView::setButtonDoubleClicked(QPushButton* btn, int i)
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
    QTimer* timer = new QTimer();
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
        nameList.append(patientVec.at(i).patientName); //GLCommonTool::GetDisplayPatientName(patientVec.at(i).patientName)
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


void patientMangeView::onOpenReport()
{
    QObject* obj = sender();
    if (obj)
    {
        QString report = obj->property(reportProperty).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(report));
    }
}

void patientMangeView::onPushButtonXDoubleClicked(QPushButton* btn)
{
    /*GLTool::progressBar = new qSlicerProgressBar;
    GLTool::progressBar->InitProgressBar(QStringLiteral("���ڴ򿪷���..."),0,100);
    GLTool::progressBar->show();*/
    qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
    auto im = qSlicerApplication::application()->ioManager();
    mw->ShowProgressBar(QStringLiteral("���ڴ򿪷���"));

    PatientsAnalyse pa = recentlyAnalysisHash.value(btn);

    //GLTool::progressBar->setProgressValue(4);
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
    QString curPath = qApp->applicationDirPath();
    QSettings set(curPath + "/configure/cfg.ini", QSettings::IniFormat);
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
    auto im = qSlicerApplication::application()->ioManager();
    qJMainWindow* mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());

    mw->ShowProgressBar(QStringLiteral("���ڴ򿪷���"));
    //��ȡ���������嵥��-���ǻ����������е�����
    QVector<PatientsAnalyseSeries> patientsSeriesVec = db.getPatientAnalyseSeries(analy);
    QStringList list;
    QVector<PatientsSeries> vec = db.getPatientsSeries(patientsSeriesVec);
    for (int i = 0; i < vec.size(); i++)
    {
        list.append(vec.at(i).seriesDir);
    }
    analy.is_package = true;
    finished_success(list, analy);
}

void patientMangeView::onItemClicked(QTableWidgetItem *item)
{
  if (item->column() == SurgerSummary)
    m_Controls.tableWidget->editItem(item);
  else
      showScreetshotAndReport(item->row());
}

void lubksb(double b[], double vv[], int indx[], itk::Matrix<double, 3, 3> matrix) {
    int n = 3, ii = 0;
    // y
    for (int i = 0; i < n; i++) {
        int ip = indx[i];
        double sum = b[ip];
        b[ip] = b[i];
        if (ii != 0)
            for (int j = ii - 1; j < i; j++) sum -= matrix[i][j] * b[j];
        else
            ii = i + 1;
        b[i] = sum;
    }
    // x
    for (int i = n - 1; i >= 0; i--) {
        double sum = b[i];
        for (int j = i + 1; j < n; j++) sum -= matrix[i][j] * b[j];
        b[i] = sum / matrix[i][i];
    }
}
void lucmp(itk::Matrix<double, 3, 3>& matrix, double vv[], int indx[]) {
    int n = 3, imax = 0;
    for (int i = 0; i < n; i++) {
        double big = 0.0;
        for (int j = 0; j < n; j++) {
            double temp = abs(matrix[i][j]);
            if (temp > big) big = temp;
        }
        vv[i] = 1.0 / big;
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < j; i++) {
            double sum = matrix[i][j];
            for (int k = 0; k < i; k++) sum -= matrix[i][k] * matrix[k][j];
            matrix[i][j] = sum;
        }
        double big = 0.0;
        for (int i = j; i < n; i++) {
            double sum = matrix[i][j];
            for (int k = 0; k < j; k++) sum -= matrix[i][k] * matrix[k][j];
            matrix[i][j] = sum;
            double dum = vv[i] * abs(sum);
            if (dum >= big) {
                big = dum;
                imax = i;
            }
        }
        if (j != imax) {
            for (int i = 0; i < n; i++) {
                double mid = matrix[imax][i];
                matrix[imax][i] = matrix[j][i];
                matrix[j][i] = mid;
            }
            double mid = vv[j];
            vv[j] = vv[imax];
            vv[imax] = mid;
        }
        indx[j] = imax;
        if (j != n - 1) {
            double dum = 1.0 / matrix[j][j];
            for (int i = j + 1; i < n; i++) matrix[i][j] *= dum;
        }
    }
}
//Gets the inverse matrix of the matrix use lut
void GetInvertMatrix(itk::Matrix<double, 3, 3> matrix, itk::Matrix<double, 3, 3>& newMatrix)
{
    double vv[3] = { 0 };
    int indx[3] = { 0 };
    lucmp(matrix, vv, indx);
    double b[3] = { 0 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) b[j] = 0;
        b[i] = 1.0;
        lubksb(b, vv, indx, matrix);
        for (int j = 0; j < 3; j++) newMatrix[j][i] = b[j];
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="path"></param>
/// <returns></returns>

bool IsFixDisk(QString path)
{
    itk::Matrix<double, 3, 3> matrix; //3��3�о���
    itk::Matrix<double, 3, 3> invmatrix;
    matrix[0][0] = 1;
    matrix[0][1] = -4;
    matrix[0][2] = -3;
    matrix[1][0] = 1;
    matrix[1][1] = -5;
    matrix[1][2] = -3;
    matrix[2][0] = -1;
    matrix[2][1] = 6;
    matrix[2][2] = 4;

    invmatrix[0][0] = 1;
    invmatrix[0][1] = 0;
    invmatrix[0][2] = 0;
    invmatrix[1][0] = 0;
    invmatrix[1][1] = 1;
    invmatrix[1][2] = 0;
    invmatrix[2][0] = 0;
    invmatrix[2][1] = 0;
    invmatrix[2][2] = 1;
    GetInvertMatrix(matrix, invmatrix);


    QStringList list = path.split("/");
    UINT res = ::GetDriveType(list[0].toStdString().c_str()); //�жϴ�������
    return res == DRIVE_FIXED;
}

bool IsUSB(QString path)
{
    QStringList list = path.split("/");
    QString test = QString("\\\\.\\%1").arg(list[0]);
    DWORD dwOutBytes;
    BOOL bResult;
    HANDLE hDevice = CreateFile(
        test.toStdString().c_str(), // Ҫ�򿪵��������豸����ʽ����Ϊ��\\.\C:���������ʧ�ܣ����ش�����2��ϵͳ�Ҳ���ָ�����ļ���
        GENERIC_READ | GENERIC_WRITE,     // ����Ȩ��
        FILE_SHARE_READ | FILE_SHARE_WRITE, //����ģʽ
        NULL,             // ʹ��Ĭ�ϵİ�ȫ����
        OPEN_EXISTING,    // �򿪴��ڵ��豸
        NULL,
        NULL
    );
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    STORAGE_DESCRIPTOR_HEADER* pDevDescHeader;
    pDevDescHeader = (STORAGE_DESCRIPTOR_HEADER*)malloc(sizeof(STORAGE_DESCRIPTOR_HEADER));
    if (NULL == pDevDescHeader)
    {
        return FALSE;
    }
    STORAGE_PROPERTY_QUERY Query;
    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;
    bResult = DeviceIoControl(
        hDevice,     // �豸���
        IOCTL_STORAGE_QUERY_PROPERTY,     // ����������
        &Query, sizeof Query,   //�����������������С
        pDevDescHeader,     //�������
        sizeof(STORAGE_DESCRIPTOR_HEADER),  //��������Ļ�������С
        &dwOutBytes,     //ʵ������Ĵ�С
        NULL);
    if (!bResult)
    {
        free(pDevDescHeader);
        (void)CloseHandle(hDevice);
        return FALSE;
    }
    PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
    pDevDesc = (STORAGE_DEVICE_DESCRIPTOR*)malloc(pDevDescHeader->Size);
    bResult = DeviceIoControl(hDevice,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &Query, sizeof Query,
        pDevDesc, pDevDescHeader->Size,
        &dwOutBytes,
        NULL);
    free(pDevDescHeader);
    (void)CloseHandle(hDevice);
    if (pDevDesc->BusType == BusTypeUsb)//��������ΪUSB������TRUE
    {
        free(pDevDesc);
        return TRUE;
    }
    free(pDevDesc);
    return FALSE;
}

QString GetExResPath(QString path, QString typeDes)
{
    QStringList list = path.split("/");
    int size = list.size();
    QString filePath = "";
    //"C:/Users/Administrator/AppData/Local"
    QString storageDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    //����·�� ��׼λ�������jlsp�ļ�+�ļ�����+֮ǰ�Ĳ���·��
    if (size == 2)
        filePath = QStringLiteral("%1/jlsq/%2/%3").arg(storageDir).arg(typeDes).arg(list[size - 1]);
    else if (size > 2)
        filePath = QStringLiteral("%1/jlsq/%2/%3/%4").arg(storageDir).arg(typeDes).arg(list[size - 2]).arg(list[size - 1]);
    else if (typeDes == "")
        filePath = QStringLiteral("%1/jlsq/%2").arg(storageDir).arg(typeDes);
    else
        filePath = QStringLiteral("%1/jlsq/%2").arg(storageDir).arg(typeDes);

    return filePath;
}

bool patientMangeView::CopyDir(const QString& source, const QString& destination, bool override, GLCustomProgressBar *bar)
{
    QDir directory(source);
    if (!directory.exists())
    {
        return false;
    }

    QString srcPath = QDir::toNativeSeparators(source);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QString dstPath = QDir::toNativeSeparators(destination);
    if (!dstPath.endsWith(QDir::separator()))
        dstPath += QDir::separator();
    QDir tmp(dstPath);
    tmp.mkpath(dstPath);

    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (override)
            {
                QFile::setPermissions(dstFilePath, QFile::WriteOwner);
            }
            QFile::copy(srcFilePath, dstFilePath);
        }
        else if (fileInfo.isDir())
        {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if (!CopyDir(srcFilePath, dstFilePath, override, bar))
            {
                error = true;
            }
            //mitk::ProgressBar::GetInstance()->Progress(1);
            bar->setValue(i);
        }
    }
    return !error;
}

void patientMangeView::slot_btnLoadClick()
{
    emit sig_customLog("���뻼����Ϣ��DICOM�ļ���");
    //QString path = QFileDialog::getExistingDirectory(parentWidget, QStringLiteral("ѡ��һ��DICOM����Ŀ¼"), getRecentDir()); //����Դ�������л�ȡDICOM�ļ�����
   QString path;
   QSettings set(qJMainWindow::GetProjectIniPath(), QSettings::IniFormat);
    bool JTest = set.value("General/qfiledialog_fixed").toString() == "2";
    if (JTest) {
        path = QStringLiteral("E:/CYH");
    }
    else 
    {
        path = QFileDialog::getExistingDirectory(parentWidget, QStringLiteral("ѡ��һ��DICOM����Ŀ¼"), getRecentDir()); //����Դ�������л�ȡDICOM�ļ�����
    }

    if (path.isEmpty())
        return;

    bool isFixDisk = IsFixDisk(path); //�ж��Ƿ��ǹ̶�����
    bool isUSB = IsUSB(path); //�ж��Ƿ�ΪU��
    if (!isFixDisk || isUSB) { //���ǹ̶�����
        QString exPath = GetExResPath(path, "dcmData"); //�ҵ�����·��
        QDir dir(exPath);
        if (dir.exists()) //·���Ѿ����ڣ���ʹ��ԭ��������
        {
            qGLMessageDialog::ShowInformation(QStringLiteral("��������"), QStringLiteral("����ʹ�ÿ��ƶ�����, ��ʹ�ñ���ͬ������"));
        }
        else {
            QStringList tmpList = { QStringLiteral("����"), QStringLiteral("������") };
            MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("��������"), QStringLiteral("����ʹ�ÿ��ƶ�����, ���������ݵ�����"), MessageButtonType::BUTTON_OK_AND_CANCEL, tmpList);
            if (res == MessageResult::OK) {
                //��������
                QDir oldDir(path);
                auto list = oldDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
                dir.mkdir(exPath);
                //AddValueSignal("show_progress_bar", QStringLiteral("���ݿ�����...").toStdString().c_str());
                GLCustomProgressBar progressBar;
                QDesktopWidget* desk = QApplication::desktop();
                progressBar.setGeometry(desk->availableGeometry()); //����������������ʾ

                progressBar.setText(QStringLiteral("���ݿ�����..."));
                
                progressBar.setWindowFlags(progressBar.windowFlags() | Qt::WindowStaysOnTopHint);
                progressBar.showNormal();
                progressBar.showFullScreen();
                /*QTimer::singleShot(200, this, [=]() */
                {
                    //mitk::ProgressBar::GetInstance()->AddStepsToDo(list.size());
                    progressBar.setMaximum(list.size());
                    CopyDir(path, exPath, true, &progressBar);
                    //mitk::ProgressBar::GetInstance()->Progress(list.size());
                    progressBar.setValue(list.size());
                    //AddSignal("hide_progress_bar");
                    progressBar.close();
                    Dicom dicom;
                    dicom.itkReadDicomSeriesAnyDir(exPath);

                    loadPatientsToTableFromDb(); //Ŀǰ���õ���һ��dicom���ʹ����ݿ���ȫ�����أ�����Ȼ���Ա�֤һ���ԣ������ܻ�Ӱ������

                    saveRecentDir(exPath);
                };
                return;
            }
        }
    }

    Dicom dicom;
    dicom.itkReadDicomSeriesAnyDir(path);

    loadPatientsToTableFromDb(); //Ŀǰ���õ���һ��dicom���ʹ����ݿ���ȫ�����أ�����Ȼ���Ա�֤һ���ԣ����ƺ��д�����

    saveRecentDir(path);
}
