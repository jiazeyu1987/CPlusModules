#include <QString>
#include <qdialog.h>
#include "ui_NaviWidgetAblation.h"
#include "qSlicerNaviModuleExport.h"

class Q_SLICER_QTMODULES_NAVI_EXPORT NaviWidgetAblation : public QDialog
{
	Q_OBJECT

public:
	NaviWidgetAblation(QDialog* parent = 0);
	void OnBackArea();

private:
	Ui::NaviWidgetAblation* ui;

	
	void Init();

};