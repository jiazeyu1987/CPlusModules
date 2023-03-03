#include "NaviWidgetAblation.h"


void NaviWidgetAblation::Init() 
{
    ui->setupUi(this);
    connect(ui->btn_Areaback, SIGNAL(clicked()), this, SLOT(OnBackArea()));
}

NaviWidgetAblation::NaviWidgetAblation(QDialog* parent) 
    : QDialog (parent), ui(new Ui::NaviWidgetAblation){
    Init();
}

void NaviWidgetAblation::OnBackArea() {
}
