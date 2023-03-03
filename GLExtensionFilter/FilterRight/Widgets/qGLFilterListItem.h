#ifndef q_GLFilterListItem_H
#define q_GLFilterListItem_H

#include "qevent.h"
#include "qwidget.h"

#include "qGLChooseItem.h"

#include "qSlicerFilterRightModuleWidgetsExport.h"


#include "vtkMRMLScalarVolumeNode.h"

class qGLFilterListItemPrivate;

class Q_SLICER_MODULE_FILTERRIGHT_WIDGETS_EXPORT qGLFilterListItem
	: public QWidget
{
	Q_OBJECT
public:
	typedef QWidget Superclass;
	qGLFilterListItem(QWidget* parent = 0);
	virtual ~qGLFilterListItem();

	bool m_IsTransmit;
	QString m_ImagePath;
	qGLChooseItem* m_ItemFriend;
	QString m_AliasName;

	vtkMRMLScalarVolumeNode* m_DataNode;
	vtkMRMLScalarVolumeNode* m_RegisterNode;

	void SetImage(QString path);

	void MaskMarkLabel(bool ismark);

	void SetFriendItem(qGLChooseItem* item, bool isTransmit = false);
	void SetMyDataNode(vtkMRMLScalarVolumeNode*);
	void SetRegisterNode(vtkMRMLScalarVolumeNode*);
	void mouseReleaseEvent(QMouseEvent* event);
	//void MaskMarkLabel(bool ismask);
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