#ifndef q_GLChooseItem_h
#define q_GLChooseItem_h
 
// Qt includes
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qwidget.h"

// FooBar Widgets includes
#include "qSlicerFilterRightModuleExport.h"
#include "vtkSlicerFilterTopLogic.h"
class qGLChooseItemPrivate;

/// \ingroup Slicer_QtModules_FilterRight
class Q_SLICER_QTMODULES_FILTERRIGHT_EXPORT qGLChooseItem
	: public QWidget
{
	Q_OBJECT
public:
	typedef QWidget Superclass;
	qGLChooseItem(QWidget* parent = 0);
	virtual ~qGLChooseItem();
	RightItemData* GetItemData();
	QLineEdit* GetEditRegistration();
	QLineEdit* GetEditTransmit();

	QPushButton* GetBtnDelete();
	QPushButton* GetBtnRegistIcon();
	QPushButton* GetBtnTransmitIcon();

	void HideDelete();	//hide delete button
	void SetEditInfo();//set edit info 

	void SetRegistAndTransmitName(QString registName, QString transmitName);
	void SetRegistrationIcon(QString path);
	void SetTransmitIcon(QString path);
	QString GetNameTransmit() { return QString::fromStdString(m_RightItemData->m_NameTransmit); }
	QString GetNameRegistration() { return QString::fromStdString(m_RightItemData->m_NameRegistration); }
public:
	RightItemData* m_RightItemData;
protected slots:

protected:
	QScopedPointer<qGLChooseItemPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qGLChooseItem);
	Q_DISABLE_COPY(qGLChooseItem);
};


#endif // !q_GLChooseItem_h