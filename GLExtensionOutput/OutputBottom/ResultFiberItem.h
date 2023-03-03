#ifndef Result_FiberItem_h
#define Result_FiberItem_h


#include "qwidget.h"
#include "ui_ResultFiberItem.h"
#include <iostream>
using namespace std;

struct FiberOutPutInfo1
{
	QString fiberName;
	QString fiberType;
	QString X, Y, Z;
	QString arcAngle, ringAngle;

};

class ResultFiberItem : public QWidget {
	Q_OBJECT
public:
	ResultFiberItem(QWidget* parent = nullptr);
	~ResultFiberItem();

	//set basic info of the fiber
	void SetFiberInfo(FiberOutPutInfo1* fiber);
public slots:
	//update info of head rack
	void UpdateHeadRackInfo();
	
private:
	Ui_ResultFiberItem m_Controls;


	QString m_Name;
	QString m_Type;
};

#endif // !Result_FiberItem_h
