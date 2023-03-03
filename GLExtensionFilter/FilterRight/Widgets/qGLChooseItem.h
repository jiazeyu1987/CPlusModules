#ifndef q_GLChooseItem_h
#define q_GLChooseItem_h

// Qt includes
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qwidget.h"

// FooBar Widgets includes
#include "qSlicerFilterRightModuleWidgetsExport.h"
#include "vtkmrm"
class qGLChooseItemPrivate;

/// \ingroup Slicer_QtModules_FilterRight
class Q_SLICER_MODULE_FILTERRIGHT_WIDGETS_EXPORT qGLChooseItem
	: public QWidget
{
	Q_OBJECT
public:
	typedef QWidget Superclass;
	qGLChooseItem(QWidget* parent = 0);
	virtual ~qGLChooseItem();
	int id = -1;
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

public:
	QString m_NameRegistration;
	QString m_NameTransmit;

protected slots:

protected:
	QScopedPointer<qGLChooseItemPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qGLChooseItem);
	Q_DISABLE_COPY(qGLChooseItem);
};


#endif // !q_GLChooseItem_h