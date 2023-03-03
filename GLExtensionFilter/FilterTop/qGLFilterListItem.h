#ifndef q_GLFilterListItem_H
#define q_GLFilterListItem_H
 
#include "qevent.h"
#include "qwidget.h"

#include "qSlicerFilterTopModuleExport.h"


#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerFilterTopLogic.h"
class qGLFilterListItemPrivate;



class Q_SLICER_QTMODULES_FILTERTOP_EXPORT qGLFilterListItem
	: public QWidget
{
	Q_OBJECT
public:
	typedef QWidget Superclass;
	qGLFilterListItem(QWidget* parent = 0);
	virtual ~qGLFilterListItem();
	TopItemData* m_ItemData = nullptr;
	const char* atm = nullptr;
	const char* atm1 = nullptr;
	const char* atm2 = nullptr;
	void SetImage(QString path);
	void HideMarkLabel(bool ismark);
	void SetDiscription();
	void SetFriendItem(RightItemData* item, bool isTransmit = false);
	void mouseReleaseEvent(QMouseEvent* event);
	void setFrontLabelImage(QString path);
	void removeFrontLabelFrame();
	void showFrontLabelFrame();
	bool markLabelVisible=false;
protected slots:

Q_SIGNALS:
	void clicked(vtkMRMLScalarVolumeNode*);
	void rightClicked(qGLFilterListItem*);

protected:
	QScopedPointer<qGLFilterListItemPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qGLFilterListItem);
	Q_DISABLE_COPY(qGLFilterListItem);

};


#endif // !q_GLFilterListItem_H