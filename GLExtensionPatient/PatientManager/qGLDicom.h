#pragma once

#include <qobject.h>
#include "qGLCommonType.h"
#include <qfileinfo.h>
#include <itkMetaDataDictionary.h>

class Dicom : public QObject
{
  Q_OBJECT
public:
	struct Miss
	{
		QString dir;
		QString seriesUID;
		QVector<int> missVec;
	};
public:
    explicit Dicom(QObject *parent = nullptr);
    ~Dicom();

    static void vtkReadDicom(std::string inputFilename);
	static  int itkReadDicom(const std::string& path, Patient& patient,
        PatientsSeries& patientsSequenceList);

	static int dcmtkReadDicom();

    // ��������Ŀ¼�����ܰ�����Ŀ¼���ݹ鴦��
	static void itkReadDicomSeriesAnyDir(QString dir);

    // ��������Ŀ¼�����������Զ�����ã���д��.nrrd�ļ�
	static  int itkReadDicomSeries1(std::string dirName);

    // ��������Ŀ¼����������Ŀ¼�������������Զ�����ã���ȡ���б��
	//isJustGenPng is just generate cover.png
	static int itkReadDicomSeries(std::string dirName, QStringList& badFileList, QVector<struct Miss>& missVec);

	//isJustGenPng is just generate cover.png
	static  int itkReadDicom(std::vector<std::string>& fileNames, Patient& patient,
        PatientsSeries& series, QString moveDir = QString(), QVector<int>& missVec_ = QVector<int>());

	//�ж������Ƿ���ȱʧ
	//�жϵ�������ͨ��(0020,0013)����֪����ǰͼ���������еı��
	//ͨ�������DICOM�ļ��������������б��˳��һ�£������ų�����
	//����ͷ��βȱ�ٵ�����޷��жϣ�������ǰ֪�����е�����
	//˳����ȷ����������	01.DCM��1��02.DCM��2��03.DCM��3��04.DCM��4��05.DCM��5��
	//˳�������������	01.DCM��3��02.DCM��2��03.DCM��1��04.DCM��5��05.DCM��4��
	//˳����ȷ������ȱʧ	01.DCM��1��02.DCM��2��03.DCM��4��04.DCM��6��05.DCM��7��
	static int dicomSequenceErrorAndSeriesMiss(std::string& fileName);

	static int gdcmReadDicom(std::string inputFilename);

	static void dcmtkDicomSaveToPng(std::string source, std::string target);

	//get the largest value and smallest value of a dicom
	static void GetMaxAndMinOfDicom(const  itk::MetaDataDictionary& dictionary, int& max, int& min);

	static QString itkDicomSaveToPng(std::string source, std::string target);

	static void ShowMissWarning(QVector<struct Miss> missVec, QStringList badFileList);

	static QFileInfoList getFileInfoFromDir(QString dir);


	static QFileInfoList fileInfoList;
};
