#include "PathAdjustment.h"


void PathAdjustment::Init()
{
    ui->setupUi(this);
}

PathAdjustment::PathAdjustment(QDialog* parent)
    : QDialog(parent), ui(new Ui::PathAdjustment) {
    Init();
}
