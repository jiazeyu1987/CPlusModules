#ifndef q_GLChooseDialog_h
#define q_GLChooseDialog_h

// Qt includes
#include "qdialog.h"

// FooBar Widgets includes
#include "qSlicerFilterRightModuleWidgetsExport.h"

class qGLChooseDialogPrivate;

/// \ingroup Slicer_QtModules_FilterRight
class Q_SLICER_MODULE_FILTERRIGHT_WIDGETS_EXPORT qGLChooseDialog
	: public QDialog
{
	Q_OBJECT
public:
	typedef QDialog Superclass;
	qGLChooseDialog(QDialog* parent = 0);
	virtual ~qGLChooseDialog();

	QString GetRegistrationName();
	QString GetTransmitName();

	void setRegistrationName(QString NameRegistration);
	void setTransmitName(QString NameTransmit);
	void setchnTransmitState(bool visible);
protected slots:
	void OnCancelClick();
	void OnConfirmClick();
	void OnTransmitOpen(bool state);

protected:
	QScopedPointer<qGLChooseDialogPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qGLChooseDialog);
	Q_DISABLE_COPY(qGLChooseDialog);
};


#endif // !q_GLChooseDialog_h