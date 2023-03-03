#include "qGLPatientTable.h"
#include <qlabel.h>
#include "qGLTool.h"

PatientTable::PatientTable(QTableWidget * tableWidget)
    :tableWidget(tableWidget)
{
    reportImage = QString(":/Icons/report_small.png"); //��ʾ���ڱ���
}

int PatientTable::addRowDataToRootVec(RowData rowData)
{
    rootVec.append(rowData);
    rootVec.append(rowData);
    return rootVec.size() - 1; //�������һ�������
}

//��ԭʼ���ݶ�Ӧ���������뵽tableIndex������
void PatientTable::addIndexToTable(int row, QVector<int> rowVec)
{
    if (rootIndexList.size() < row + 1)
        rootIndexList.insert(row, rowVec);
    else
        rootIndexList[row] = rowVec;

    //����rootVec�е����ݣ�Ȼ��ϲ�
    QVector<QString> rowData;

    QStringList reportList;
    QStringList patientNameList;
    QStringList dicomPatientIdList;
    QStringList birthdayList;
    QStringList ageList;
    QStringList sexList;
    QStringList modifyList;
    QStringList accessionNumberList;
    QStringList modalitiesList;
    QStringList surgerySummaryList;
    for (int i = 0; i < rowVec.size(); i++)
    {
        int rowIndex = rowVec.at(i);
       RowData rd = rootVec.at(rowIndex); //��ȡʵ�ʵ�����
        reportList.append(rd.report);
        patientNameList.append(rd.patientName);
        dicomPatientIdList.append(rd.dicomPatientId);
        birthdayList.append(rd.birthday);
        ageList.append(rd.age);
        sexList.append(rd.sex);
        modifyList.append(rd.modifyDateTime);
        accessionNumberList.append(rd.accessionNumber);
        modalitiesList.append(rd.modalities);
        surgerySummaryList.append(rd.surgrySummary.trimmed());
    }

    //�ϲ���һ��
    RowData rds;

    reportList.removeDuplicates();
    rds.report = reportList;

    patientNameList.removeDuplicates();
    rds.patientName = patientNameList.join(";");

    dicomPatientIdList.removeDuplicates();
    rds.dicomPatientId = dicomPatientIdList.join(";");

    birthdayList.removeDuplicates();
    rds.birthday = birthdayList.join(";");

    ageList.removeDuplicates();
    rds.age = ageList.join(";");

    sexList.removeDuplicates();
    rds.sex = sexList.join(";");

    modifyList.removeDuplicates();
    rds.modifyDateTime = modifyList.join(";");

    accessionNumberList.removeDuplicates();
    rds.accessionNumber = accessionNumberList.join(";");

    modalitiesList.removeDuplicates();
    rds.modalities = modalitiesList.join(";");

    surgerySummaryList.removeDuplicates(); //�����������У�ɾ��һ��
    rds.surgrySummary = surgerySummaryList.join(";");

    addRowToTableWidget(row, rds);
}

//���ϲ���ʱ�򣬲���ȫ��ɾ����Ȼ��ϲ��ķ�ʽ
void PatientTable::insertRowToTableWhenMerge(int row, QVector<int> rowVec)
{
    rootIndexList.insert(row, rowVec); //ֱ�Ӳ��뵽ԭ�ȵ���

    //����rootVec�е����ݣ�Ȼ��ϲ�
    QStringList reportList;
    QStringList patientNameList;
    QStringList dicomPatientIdList;
    QStringList birthdayList;
    QStringList ageList;
    QStringList sexList;
    QStringList modifyList;
    QStringList accessionNumberList;
    QStringList modalitiesList;
    QStringList surgerySummaryList;
    for (int i = 0; i < rowVec.size(); i++)
    {
        int rowIndex = rowVec.at(i);
        RowData rd = rootVec.at(rowIndex); //��ȡʵ�ʵ�����
        reportList.append(rd.report);
        patientNameList.append(rd.patientName);
        dicomPatientIdList.append(rd.dicomPatientId);
        birthdayList.append(rd.birthday);
        ageList.append(rd.age);
        sexList.append(rd.sex);
        modifyList.append(rd.modifyDateTime);
        accessionNumberList.append(rd.accessionNumber);
        modalitiesList.append(rd.modalities);
        surgerySummaryList.append(rd.surgrySummary.trimmed());
    }

    //�ϲ���һ��
    RowData rds;

    reportList.removeDuplicates();
    rds.report = reportList;

    patientNameList.removeDuplicates();
    rds.patientName = patientNameList.join(";");

    dicomPatientIdList.removeDuplicates();
    rds.dicomPatientId = dicomPatientIdList.join(";");

    birthdayList.removeDuplicates();
    rds.birthday = birthdayList.join(";");

    ageList.removeDuplicates();
    rds.age = ageList.join(";");

    sexList.removeDuplicates();
    rds.sex = sexList.join(";");

    modifyList.removeDuplicates();
    rds.modifyDateTime = modifyList.join(";");

    accessionNumberList.removeDuplicates();
    rds.accessionNumber = accessionNumberList.join(";");

    modalitiesList.removeDuplicates();
    rds.modalities = modalitiesList.join(";");

    surgerySummaryList.removeDuplicates(); //�����������У�ɾ��һ��
    rds.surgrySummary = surgerySummaryList.join(";");

    insertRowToTableWidget(row, rds);
}

void PatientTable::insertRowToTableWidget(int row, RowData rd)
{
    tableWidget->insertRow(row);

    QLabel *L1 = new QLabel();
    if (!rd.report.isEmpty())
    {
        L1->setPixmap(QPixmap(reportImage));
    }
    tableWidget->setCellWidget(row, Report_, L1);

    QTableWidgetItem *item1 = new QTableWidgetItem(rd.patientName.split(":")[0].trimmed());
    tableWidget->setItem(row, PatientName, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem(rd.dicomPatientId);
    tableWidget->setItem(row, DicomPatientId, item2);

    QTableWidgetItem *item3 = new QTableWidgetItem(rd.birthday);
    tableWidget->setItem(row, PatientBirthday, item3);

    QTableWidgetItem *item4 = new QTableWidgetItem(rd.age);
    tableWidget->setItem(row, PatientAge, item4);

    QTableWidgetItem *item5 = new QTableWidgetItem(rd.sex);
    tableWidget->setItem(row, PatientSex, item5);

    QTableWidgetItem *item6 = new QTableWidgetItem(rd.modifyDateTime);
    tableWidget->setItem(row, RecordModificationTime, item6);

    QTableWidgetItem *item7 = new QTableWidgetItem(rd.accessionNumber);
    tableWidget->setItem(row, AccessionNumber, item7);

    QTableWidgetItem *item8 = new QTableWidgetItem(rd.modalities);
    tableWidget->setItem(row, Modalities, item8);

    QTableWidgetItem *item9 = new QTableWidgetItem(rd.surgrySummary);
    tableWidget->setItem(row, SurgerSummary, item9);
}

void PatientTable::splitRowFromTable(QVector<int> rowVec) //rowVec����������ѡ�е���
{
    for each (int row in rowVec)
    {
        QVector<int>  rows = rootIndexList.at(row);
        if (rows.size() <= 1) //С�ڻ����1�Ͳ��÷ֿ�
            return;

        delRowFormTable(QVector<int>(1, row)); //��ɾ�������

        for (int i = 0; i < rows.size(); i++)
        {
            int rowIndex = rows.at(i);
            RowData rd = rootVec.at(rowIndex);
            QVector<RowData> rdVec(1, rd);
            addIndexToTable(rows.at(i), QVector<int>(1, rows.at(i)));
        }
    }
}

QStringList PatientTable::getPatientId(int row)
{
    QVector<int> rows = rootIndexList.at(row); //��ȡԭʼ����
    QStringList patientIdList;
    for (int i = 0; i < rows.size(); i++)
    {
        QString patientId = rootVec.at(rows.at(i)).patientId;
        patientIdList.append(patientId);
    }

    return patientIdList;
}

QVector<Patient> PatientTable::getPatients(int row)
{
    QVector<int> rootRowVec = rootIndexList.at(row);
    QVector<Patient> patientVec;
    for each (int rr in rootRowVec)
    {
        Patient patient;
        patient.patientId = rootVec.at(rr).patientId.toInt();
        patient.reportList = rootVec.at(rr).report;
        patient.patientName = rootVec.at(rr).patientName;
        patient.dicomPatientId = rootVec.at(rr).dicomPatientId;
        patient.birthday = rootVec.at(rr).birthday;
        patient.age = rootVec.at(rr).age;
        patient.sex = rootVec.at(rr).sex;
        patient.recordModificationTime = rootVec.at(rr).modifyDateTime;
        patient.surgerSummary = rootVec.at(rr).surgrySummary;
        patientVec.append(patient);
    }
    return patientVec;
}

QVector<RowData> PatientTable::getRowData(int row)
{
    QVector<int> rootRowVec = rootIndexList.at(row); //�����row������Ӧ��ԭʼ��ά�������
    QVector<RowData> rowDataVec;
    for each (int rr in rootRowVec) //��ԭʼ����rootVec��ȡ����
    {
        RowData rd = rootVec.at(rr);
        rowDataVec.append(rd);
    }
    return rowDataVec;
}

void PatientTable::clearTable()
{
    tableWidget->setRowCount(0); //��ձ��
    rootVec.clear(); //�������
    rootIndexList.clear(); //�������Ӧ������
    mergeSignIndexHash.clear();
    patientVec.clear(); //��ջ���
}

void PatientTable::updateTableWidget()
{
    tableWidget->setRowCount(0);
    for (int row = 0; row < rootIndexList.size(); row++)
    {
        QVector<int> rootIndexVec = rootIndexList.at(row);

        QStringList reportList;
        QStringList patientNameList;
        QStringList dicomPatientIdList;
        QStringList birthdayList;
        QStringList ageList;
        QStringList sexList;
        QStringList modifyList;
        QStringList accessionNumberList;
        QStringList modalitiesList;
        QStringList surgerySummaryList;
        for (int i = 0; i < rootIndexVec.size(); i++)
        {
            int rootIndex = rootIndexVec.at(i);
            RowData rd = rootVec.at(rootIndex);
            reportList.append(rd.report);
            patientNameList.append(rd.patientName);
            dicomPatientIdList.append(rd.dicomPatientId);
            birthdayList.append(rd.birthday);
            ageList.append(rd.age);
            sexList.append(rd.sex);
            modifyList.append(rd.modifyDateTime);
            accessionNumberList.append(rd.accessionNumber);
            modalitiesList.append(rd.modalities);
            surgerySummaryList.append(rd.surgrySummary.trimmed());
        }

        //�ϲ���һ��
        RowData rds;

        reportList.removeDuplicates();
        rds.report = reportList;

        patientNameList.removeDuplicates();
        rds.patientName = patientNameList.join(";");

        dicomPatientIdList.removeDuplicates();
        rds.dicomPatientId = dicomPatientIdList.join(";");

        birthdayList.removeDuplicates();
        rds.birthday = birthdayList.join(";");

        ageList.removeDuplicates();
        rds.age = ageList.join(";");

        sexList.removeDuplicates();
        rds.sex = sexList.join(";");

        modifyList.removeDuplicates();
        rds.modifyDateTime = modifyList.join(";");

        accessionNumberList.removeDuplicates();
        rds.accessionNumber = accessionNumberList.join(";");

        modalitiesList.removeDuplicates();
        rds.modalities = modalitiesList.join(";");

        surgerySummaryList.removeDuplicates(); //�����������У�ɾ��һ��
        rds.surgrySummary = surgerySummaryList.join(";");

        insertRowToTableWidget(row, rds);
    }
}

QStringList PatientTable::merge(QVector<int> rowVec)
{
    QVector<int> rootIndexMergeVec;
    int size = rowVec.size();
    for (int i = 0; i < size; i++)
    {
        int row = rowVec.at(i);
        QVector<int> rootIndexVec = rootIndexList.at(row);
        rootIndexMergeVec.append(rootIndexVec);
    }

    int row = rowVec.at(0); //�����һ��

    for (int i = size - 1; i >= 0; i--)
    {
        int row = rowVec.at(i);
        rootIndexList.removeAt(row);
    }

    rootIndexList.insert(row, rootIndexMergeVec);

    QStringList patientIdList;
    for (int i = 0; i < rootIndexMergeVec.size(); i++)
    {
        //��ȡԭʼ�е�����
        int index = rootIndexMergeVec.at(i);
        RowData rd = rootVec.at(index);
        patientIdList.append(rd.patientId);
    }

    return patientIdList; //���غϲ����
}

QStringList PatientTable::split(QVector<int> rowVec)
{
    std::sort(rowVec.begin(), rowVec.end()); //ȷ����С����
    QStringList patientIdList;
    for (int i = rowVec.size() - 1; i >= 0; i--)
    {
        int row = rowVec.at(i);
        QVector<int> rootIndexVec = rootIndexList.at(row);
        if (rootIndexVec.size() < 2)
            continue;

        rootIndexList.removeAt(row);
        for (int j = 0; j < rootIndexVec.size(); j++)
        {
            QVector<int> _rowIndexVec(1, rootIndexVec.at(j));
            rootIndexList.insert(row, _rowIndexVec);
            row++;
            patientIdList.append(rootVec.at(rootIndexVec.at(j)).patientId);
        }
    }

    updateTableWidget();

    return patientIdList;
}

void PatientTable::search(QVector<int> rowVec)
{
    QList<QVector<int>> _rootIndexList;
    //ȡ����Ӧ����
    for (int i = 0; i < rowVec.size(); i++)
    {
        int row = rowVec.at(i);
        QVector<int> rowIndexVec = rootIndexList.at(row);
        _rootIndexList.append(rowIndexVec);
    }

    rootIndexList = _rootIndexList;
    updateTableWidget();
}

void PatientTable::sort(QVector<int> rowVec)
{
    QList<QVector<int>> _rootIndexList;
    //ȡ����Ӧ����
    for (int i = 0; i < rowVec.size(); i++)
    {
        int row = rowVec.at(i);
        QVector<int> rowIndexVec = rootIndexList.at(row);
        _rootIndexList.append(rowIndexVec);
    }

    rootIndexList = _rootIndexList;
    updateTableWidget();
}

void PatientTable::setMergeSignIndexHash(QString mergeSign, int index)
{
    if (mergeSign.isEmpty()) //���Ϊ�գ���ֱ�Ӳ���ԭʼ��
        rootIndexList.append(QVector<int>(1, index));
    else //�������hash
    {
        QHash<QString, QVector<int>>::iterator it = mergeSignIndexHash.find(mergeSign);
        if (it == mergeSignIndexHash.end())
            mergeSignIndexHash.insert(mergeSign, QVector<int>(1, index));
        else
            it.value().append(index);
    }
}

void PatientTable::mergeSignIndexHashToRootIndexList()
{
    QHashIterator<QString, QVector<int> > it(mergeSignIndexHash);
    while (it.hasNext())
    {
        it.next();
        rootIndexList.append(it.value());
    }
}

void PatientTable::delRowFormTable(QVector<int> rowVec)
{
    qSort(rowVec); //ȷ���Ӵ�Сɾ��
    for (int i = rowVec.size() - 1; i >= 0; i--)
    {
        int row = rowVec.at(i);
        rootIndexList.removeAt(row);
        tableWidget->removeRow(row);
    }
}

void PatientTable::addRow(RowData rd)
{
    int currentRow = tableWidget->rowCount();

    addRowDataToRootVec(rd);
    addIndexToTable(currentRow, QVector<int>(1, currentRow));
}

void PatientTable::addRowToTableWidget(int row, RowData rd)
{
    if (tableWidget->rowCount() > row + 1) //����������ڣ���ɾ����Ȼ���޸�
    {
        tableWidget->removeRow(row);
    }

    insertRowToTableWidget(row, rd);
}

