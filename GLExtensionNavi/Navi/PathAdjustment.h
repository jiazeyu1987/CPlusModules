#include <QString>
#include <qdialog.h>
#include "ui_PathAdjustment.h"
#include "qSlicerNaviModuleExport.h"

class Q_SLICER_QTMODULES_NAVI_EXPORT PathAdjustment : public QDialog
{
	Q_OBJECT

public:
	PathAdjustment(QDialog* parent = 0);

private:
	Ui::PathAdjustment* ui;

	void Init();

};