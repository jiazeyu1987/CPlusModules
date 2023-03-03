#include "ModelAdjustView.h"
#include "vtkMRMLColorTableNode.h"
#include <vtkMRMLDiffusionWeightedVolumeNode.h>
ModelAdjustView::ModelAdjustView(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}