#pragma once

#include <qvector.h>
#define ADMINISTRATOR_USERNAME "administrator"
#define ADMINISTRATOR_PASSWORD "1234567890"

//患者分析表
typedef struct
{
	int patientAnalyseId = -1; //患者分析id
	int accountId = -1; //病人所属的账户，不同的账户，患者不能共用
	QString analyseName; //分析名称
	QString illnessKind; //疾病种类
	QString doctorRemark; //患者备注
	QString patientName; //病人姓名
	QString birthday; //病人生日
	QString age; //注意：不属于数据库的列，加上仅是为了方便表示
	QString sex; //病人性别
	QString height; //身高
	QString weight; //体重
	QString reportPath; //报告路径
	bool enableColumn = true; //列是否有效
	QString createTime; //数据创建时间
	QString changedTime; //数据最新改变时间
	QString remark = ""; //备注
	QString md5 = QString::null; //MD5编码信息
} PatientsAnalyse, * p_struct_patientsAnalyse;