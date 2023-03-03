#pragma once

#include <qtablewidget.h>
#include "qGLCommonType.h"

//����洢������������ݣ����ڱ��������ݸı�ͷ��ʣ�����أ����룬�������ϲ�������
//�������ݵķ�ʽ���Ƿ��ʱ���ڲ������ݣ������Ǳ������ʾ�����ݣ�Ϊ�ˣ��趨�����ϸ�ͬ����������Ҫ
//�������ڲ������ݣ�table���ͱ������ʾ������Ӧ������������Ӧ�÷�װ��һ�𣬰�����ӣ��޸ģ�ɾ���ȵȡ�
//���չʾ���ݣ��ڲ�����������������
//����ֻ���Դ�PatientMangerView�г��������Ϊ�˸��õ����ݺ���ʾ�ķ��룬������Ҫ��ע�Ա��ĸı�ı仯�������ڻ�ȡ�͵��������Ȥ
class PatientTable : public QObject
{
    Q_OBJECT
public:
    PatientTable(QTableWidget *tableWidget);

    void addRow(RowData rd); //�����

    int addRowDataToRootVec(RowData rowData); //���һ�����ݵ�ԭʼ���ݼ�

    void addRowToTableWidget(int row, RowData rd); //����е�TableWidget
    void delRowFormTable(QVector<int> rowVec); //ɾ����
    void addIndexToTable(int row, QVector<int> rowVec); //����е��������
    void insertRowToTableWhenMerge(int row, QVector<int> rowVec); //���ϲ���ʱ�������
    void insertRowToTableWidget(int row, RowData rd); //���ϲ���ʱ���������
    void splitRowFromTable(QVector<int> rowVec); //�����������
    QStringList getPatientId(int row); //��ȡĳ������Ӧ��patientId
    QVector<Patient> getPatients(int row); //��ȡĳ�ж�Ӧ��Patient
    QVector<RowData> getRowData(int row);
    void clearTable();
    void updateTableWidget();
    QStringList merge(QVector<int> rowVec); //�ϲ���
    QStringList split(QVector<int> rowVec); //������
    void search(QVector<int> rowVec); //����
    void sort(QVector<int> rowVec); //����
    void setMergeSignIndexHash(QString mergeSign, int row);
    void mergeSignIndexHashToRootIndexList();

    QVector<Patient> patientVec; //���ݿ������
    QHash<QString, QVector<int>> mergeSignIndexHash; //�ϲ���־����
    QVector<RowData> rootVec; //����ԭʼ�ֿ⣬û�кϲ������룬������
    //QVector<int>����ָ��ԭʼ���ݵ����������ĸ��»��ڴˣ���˷ǳ���Ҫ��ʹ��QList�Ľ��в����ʱ�������������ֱ�Ӳ������
    QList<QVector<int>> rootIndexList;  //�±�ͱ���ÿһ�ж�Ӧ

    QTableWidget *tableWidget;
    QString reportImage; //����ͼƬ·��
};
