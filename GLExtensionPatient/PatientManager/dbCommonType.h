#pragma once

#include <qvector.h>
#define ADMINISTRATOR_USERNAME "administrator"
#define ADMINISTRATOR_PASSWORD "1234567890"

//���߷�����
typedef struct
{
	int patientAnalyseId = -1; //���߷���id
	int accountId = -1; //�����������˻�����ͬ���˻������߲��ܹ���
	QString analyseName; //��������
	QString illnessKind; //��������
	QString doctorRemark; //���߱�ע
	QString patientName; //��������
	QString birthday; //��������
	QString age; //ע�⣺���������ݿ���У����Ͻ���Ϊ�˷����ʾ
	QString sex; //�����Ա�
	QString height; //���
	QString weight; //����
	QString reportPath; //����·��
	bool enableColumn = true; //���Ƿ���Ч
	QString createTime; //���ݴ���ʱ��
	QString changedTime; //�������¸ı�ʱ��
	QString remark = ""; //��ע
	QString md5 = QString::null; //MD5������Ϣ
} PatientsAnalyse, * p_struct_patientsAnalyse;