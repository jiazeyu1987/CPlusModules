/*==============================================================================

  Program:  3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// Slicer includes
#include "qSlicerFilterTopModuleWidget.h"
#include "ui_qSlicerFilterTopModuleWidget.h"
#include "QFileDialog.h"
#include "qSlicerApplication.h"
#include "qSlicerIOManager.h"
#include "qGLMessageDialog.h"
#include "qSlicerDataDialog.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFilterTopModuleWidgetPrivate: public Ui_qSlicerFilterTopModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerFilterTopModuleWidget);
protected:
	qSlicerFilterTopModuleWidget* const q_ptr;

public:
  qSlicerFilterTopModuleWidgetPrivate(qSlicerFilterTopModuleWidget& object);
  ~qSlicerFilterTopModuleWidgetPrivate();
  void Init();
  vtkSlicerFilterTopLogic* logic() const;
  void AddItemToList(vtkMRMLScalarVolumeNode* node);
  
  void ShowFirstNode();
  vtkSlicerFilterTopLogic* logic();
};

vtkSlicerFilterTopLogic* qSlicerFilterTopModuleWidgetPrivate::logic() {
	Q_Q(qSlicerFilterTopModuleWidget);
	return  vtkSlicerFilterTopLogic::SafeDownCast(q->logic());
}

void qSlicerFilterTopModuleWidgetPrivate::Init()
{
	Q_Q(qSlicerFilterTopModuleWidget);
	this->setupUi(q);
	//QObject::connect(btnLoad, &QPushButton::clicked, q, &qSlicerFilterTopModuleWidget::onLoadPatientFiles);
	//QObject::connect(btnDelete, &QPushButton::clicked, q, &qSlicerFilterTopModuleWidget::OnRemoveClick);
	
}



void qSlicerFilterTopModuleWidgetPrivate::ShowFirstNode()
{
	Q_Q(qSlicerFilterTopModuleWidget);
	if (this->logic()->m_TopItemDataList.size() > 0) {
		auto widget = this->logic()->m_TopItemDataList.at(0);
		if (widget->m_DataNode) {
			q->OnClickWidget(widget->m_DataNode);
		}
	}
}

void qSlicerFilterTopModuleWidgetPrivate::AddItemToList(vtkMRMLScalarVolumeNode* node)
{
	Q_Q(qSlicerFilterTopModuleWidget);
	if (!node) {
		return;
	}
	const char* filename = node->filename.c_str();
	if (!filename || strcmp(filename,"")==0) {
		filename = node->GetAttribute("filename");
		if (!filename) {
			return;
		}
	}

	auto patientName = node->GetAttribute("patientName");
	node->SetAttribute("filename", filename);
	std::string spath = itksys::SystemTools::GetFilenamePath(filename);
	QString path = QString::fromStdString(spath);
	qGLFilterListItem* widget = new qGLFilterListItem(piclist);
	auto itemData = widget->m_ItemData;
	QString imgPath = path + "/cover.png";
	node->SetAttribute("ImagePath", imgPath.toStdString().c_str());
	QFile file(imgPath);
	if (file.exists()) {
		widget->SetImage(imgPath);
	}
	else {
		q->AddSignalNode("create_png", filename);
		auto appdir = QCoreApplication::applicationDirPath();
		auto imagepath = appdir + "/Resources/Icons/BrainSegment.png";
		widget->SetImage(imagepath.toLatin1());
	}
	widget->SetDiscription();
	itemData->SetMyDataNode(node);

	//初始化, 清理node的别名
	widget->SetFriendItem(nullptr);

	QListWidgetItem* item = new QListWidgetItem(piclist);
	piclist->setItemWidget(item, widget);
	//item->setSizeHint(QSize(piclist->sizeHint().width(), 179));
	item->setSizeHint(QSize(200, 230));
	this->logic()->m_TopItemDataList.push_back(widget->m_ItemData);
	QObject::connect(widget, SIGNAL(clicked(vtkMRMLScalarVolumeNode*)), q, SLOT(OnClickWidget(vtkMRMLScalarVolumeNode*)));
	QObject::connect(widget, SIGNAL(rightClicked(qGLFilterListItem*)), q, SLOT(OnRightClickWidget(qGLFilterListItem*)));
	q->OnClickWidget(node);
}

//-----------------------------------------------------------------------------
// qSlicerFilterTopModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFilterTopModuleWidgetPrivate::qSlicerFilterTopModuleWidgetPrivate(qSlicerFilterTopModuleWidget& object) : q_ptr(&object)
{

}

qSlicerFilterTopModuleWidgetPrivate::~qSlicerFilterTopModuleWidgetPrivate(){}

//-----------------------------------------------------------------------------
// qSlicerFilterTopModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerFilterTopModuleWidget::qSlicerFilterTopModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerFilterTopModuleWidgetPrivate(*this))
{
}


//-----------------------------------------------------------------------------
qSlicerFilterTopModuleWidget::~qSlicerFilterTopModuleWidget()
{
}

vtkSlicerFilterTopLogic* qSlicerFilterTopModuleWidgetPrivate::logic() const {
	Q_Q(const qSlicerFilterTopModuleWidget);
	return vtkSlicerFilterTopLogic::SafeDownCast(q->logic());
}


//-----------------------------------------------------------------------------
void qSlicerFilterTopModuleWidget::setup()
{
  Q_D(qSlicerFilterTopModuleWidget);
  this->Superclass::setup();
  d->Init();
  //d->piclist->setViewMode(QListView::IconMode);
  //d->piclist->setFlow(QListView::LeftToRight);
  d->piclist->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  d->piclist->setSpacing(5);

  

}

void qSlicerFilterTopModuleWidget::OnNodeAdd()
{
	Q_D(qSlicerFilterTopModuleWidget);
}





QStringList qSlicerFilterTopModuleWidget::filenameInDir(const QString& path, QString tail)
{
	QStringList string_list;
	//判断路径是否存在
	QDir dir(path);
	if (!dir.exists())
		return string_list;

	//查看路径中后缀为.cfg格式的文件
	QStringList filters;
	filters << QString(tail);
	dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
	dir.setNameFilters(filters);                   //设置文件名称过滤器，只为filters格式

	//统计cfg格式的文件个数
	int dir_count = dir.count();
	if (dir_count <= 0) {
		QDir tmp(path);
		bool isContainFileWithoutExtension = (tail.toLower() == "*.dcm");
		if (isContainFileWithoutExtension) {
			for (int i = 0; i < tmp.count(); i++) {
				QString tmpFileName = tmp[i];
				if (tmpFileName.split(".").size() <= 1) {
					string_list.append(path + "/" + tmpFileName);
					return string_list;
				}
			}
			int center = tmp.count() / 2;
			string_list.append(path + "/" + tmp[center]);
			return string_list;
		}
		return string_list;
	}


	//存储文件名称
	for (int i = 0; i < dir_count; i++)
	{
		QString file_name = dir[i]; //文件名称
		// ts<<file_name<<"\r\n"<<"\r\n";
		string_list.append(path + "/" + file_name);
		break;
	}
	return string_list;
}

void qSlicerFilterTopModuleWidget::LoadDcm(QString filePath)
{
	QString dicom_shortcut = QString::fromStdString("*.dcm");
	QStringList fileNames = filenameInDir(filePath, dicom_shortcut);
	if (fileNames.length() <= 0) {
		QDir tmp(filePath);
		bool isFind = false;
		for (int i = 0; i < tmp.count(); i++) {
			QString tmpFileName = tmp[i];
			if (tmpFileName.split(".").size() <= 1) {
				isFind = true;
			}
		}
		if (!isFind) {
			return;
		}
	}

	auto im = qSlicerApplication::application()->ioManager();
	for (int i = 0; i < fileNames.size(); i++) {
		im->loadFile(fileNames.at(i));
	}
	

}

void qSlicerFilterTopModuleWidget::enter() {
	this->Superclass::enter();
	Q_D(qSlicerFilterTopModuleWidget);
	auto fc = GetGlobalSaveValue("filter_top_clear");
	if (fc && strcmp(fc, "true") == 0) {
		d->piclist->clear();
		d->logic()->clear();
		SetGlobalSaveValue("filter_top_clear", "false");
	}
	qDebug() << "top filter logic modify connect";
	qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
}


void qSlicerFilterTopModuleWidget::onLogicModified(vtkObject* vtkNotUsed(caller), vtkObject* callData)
{
	Q_D(qSlicerFilterTopModuleWidget);
	// the call data should be the annotation hierarchy node that was just
	// added, passed along from the logic
	if (callData == nullptr)
	{
		return;
	}
	vtkMRMLColorNode* node = nullptr;
	node = reinterpret_cast<vtkMRMLColorNode*>(callData);
	if (node == nullptr)
	{
		return;
	}
	auto help_node = node->GetAttribute("Help_Node");
	auto type = node->GetAttribute("Help_Name");
	auto value = node->GetAttribute("Help_Info_0");
	if (help_node == nullptr) {
		return;
	}
	SafeRemove(node);
	qDebug() << "qSlicerFilterTopModuleWidget::onLogicModified:" << help_node<<type<<value;
	
	if (help_node && strcmp(help_node, "true") == 0) 
	{
		
		if (type&&value) {
			if (strcmp(type, "filter_add_node") == 0) {
				auto node = mrmlScene()->GetNodeByID(value);
				auto origin_node = node->GetAttribute("origin_node");
				if (node&&!origin_node) {
					d->AddItemToList(vtkMRMLScalarVolumeNode::SafeDownCast(node));
				}
			}
			if (strcmp(type, "remove_left_filter_node") == 0) {
				OnRemoveClick();
			}
			if (strcmp(type, "fresh_top_filter_item") == 0) {
				OnFreshItem(value);
			}
			if (strcmp(type, "fresh_left_filter_list") == 0) {
				onFreshState();
			}
			if (strcmp(type, "animate_top_click") == 0) {
				int val = QString::fromLatin1(value).toInt();
				d->piclist->setCurrentItem(d->piclist->item(val));
				auto widget = d->logic()->m_TopItemDataList[val];
				if (widget->m_DataNode) {
					OnClickWidget(widget->m_DataNode);
				}
			}
			if (strcmp(type, "function_test_filter_test_and_save_top_click") == 0) 
			{
    			d->logic()->m_TopItemDataList[0];
				if (strcmp(value, "0") == 0) {
					d->piclist->setCurrentItem(d->piclist->item(0));
					auto widget = d->logic()->m_TopItemDataList[0];
					if (widget->m_DataNode) {
						OnClickWidget(widget->m_DataNode);
					}
				}
				if (strcmp(value, "1") == 0) {
					d->piclist->setCurrentItem(d->piclist->item(1));
					auto widget = d->logic()->m_TopItemDataList[1];
					if (widget->m_DataNode) {
						OnClickWidget(widget->m_DataNode);
					}
				}
			}
			if (strcmp(type, "init_data_list") == 0) {
				InitWidgetListFromDicomCollection(true);
				d->ShowFirstNode();
				return;
			}

			if (strcmp(type, "Update_topitem_ui") == 0) {
				/*QString path(":/FilterIcons/choosed.png");
				if(m_currentItem)
					m_currentItem->setFrontLabelImage(path);*/
				onFreshState();
				return;
			}

	
		}
	}
}

void qSlicerFilterTopModuleWidget::OnFreshItem(const char* node_id) {
	Q_D(qSlicerFilterTopModuleWidget);
	int n = d->piclist->count();
	if (n > 0) {
		QListWidgetItem* item = d->piclist->item(n - 1);
		auto widget1 = d->piclist->itemWidget(item);
		qGLFilterListItem* widget = dynamic_cast<qGLFilterListItem*>(widget1);
		auto node = widget->m_ItemData->m_DataNode;
		if(node&&strcmp(node->GetID(),node_id)==0){
		auto patientName = node->GetAttribute("patientName");
		auto filename = node->GetAttribute("filename");
		std::string spath = itksys::SystemTools::GetFilenamePath(filename);
		QString path = QString::fromStdString(spath);
		QString imgPath = path + "/cover.png";
		node->SetAttribute("ImagePath", imgPath.toStdString().c_str());
		QFile file(imgPath);
		if (file.exists()) {
			widget->SetImage(imgPath);
		}
		widget->SetDiscription();
		}
	}
}

void qSlicerFilterTopModuleWidget::InitWidgetListFromDicomCollection(bool isBackup) {
	Q_D(qSlicerFilterTopModuleWidget);
	vtkCollection* nodes = mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode");
	vtkObject* nextObject = nullptr;
	for (nodes->InitTraversal(); (nextObject = nodes->GetNextItemAsObject()); )
	{
		vtkMRMLScalarVolumeNode* node = vtkMRMLScalarVolumeNode::SafeDownCast(nextObject);
		auto filename = node->GetAttribute("filename");
		auto origin_node = node->GetAttribute("origin_node");
		if ( !origin_node) {
			d->AddItemToList(node);
		}
	}
	onFreshState();
}

void qSlicerFilterTopModuleWidget::OnRemoveClick()
{
	Q_D(qSlicerFilterTopModuleWidget);

	QList<QListWidgetItem*> selected = d->piclist->selectedItems();
	if (selected.empty())
	{
		qGLMessageDialog::ShowWarnning(QStringLiteral("请先点击选择一个节点移除!"), true);
		return;
	}

	QStringList slist = { QStringLiteral("确定"),QStringLiteral("取消") };
	MessageResult res = qGLMessageDialog::ShowQuestion(QStringLiteral("节点移除"), QStringLiteral("是否要移除该节点？"), MessageButtonType::BUTTON_OK_AND_CANCEL, slist, true);
	if (res != MessageResult::OK) {
		return;
	}

	QListWidgetItem* selectedItem = selected.at(0);
	qGLFilterListItem* widget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(selectedItem));
	RemoveDataNodeFromList(selectedItem);

	//InitWidgetListFromDicomCollection();
	d->ShowFirstNode();
}

void qSlicerFilterTopModuleWidget::exit() {
	this->Superclass::exit();
	Q_D(qSlicerFilterTopModuleWidget);
	//d->piclist->clear();
	//d->logic()->clear();
	m_LastFilterItem = nullptr;
	onChangePageToRegister();
	qvtkDisconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
}

//@ww
//int now_click, last_click = -1;
//void qSlicerFilterTopModuleWidget::OnClickWidget(vtkMRMLScalarVolumeNode* volumeNode)
//{
//	Q_D(qSlicerFilterTopModuleWidget);
//	int j = d->piclist->currentRow();
//	if (j == -1) {
//		j = 0;
//	}
//	now_click = j;
//	qGLFilterListItem* Itemwidget;
//	if (now_click != last_click) {
//		if (last_click == -1) {
//			last_click = now_click;
//			QListWidgetItem* a = d->piclist->item(now_click);
//			Itemwidget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(a));
//			m_currentItem = Itemwidget;
//			Itemwidget->HideMarkLabel(false);
//		}
//		else {
//			QListWidgetItem* a = d->piclist->item(now_click);
//			QListWidgetItem* b = d->piclist->item(last_click);
//			Itemwidget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(a));
//			m_currentItem = Itemwidget;
//			qGLFilterListItem* Itemwidget2 = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(b));
//			if(Itemwidget)
//			Itemwidget->HideMarkLabel(false);
//			if(Itemwidget2)
//			Itemwidget2->HideMarkLabel(true);
//			last_click = now_click;
//		}
//	}
//	else {
//		Itemwidget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(d->piclist->item(now_click)));
//		Itemwidget->HideMarkLabel(false);
//	}
//
//	if (Itemwidget) {
//		auto lis1 = d->logic()->m_TopItemDataList;
//		d->logic()->m_SelectedItemData = Itemwidget->m_ItemData;
//	}
//	
//	if (volumeNode) {
//		volumeNode->SetSelected(true);
//		DoSelectVolumeNode(volumeNode);
//		AddSignalNode("Filter_Node_Click", volumeNode->GetID());
//	}
//	Reinit();
//}

//@sxm
void qSlicerFilterTopModuleWidget::OnClickWidget(vtkMRMLScalarVolumeNode* volumeNode)
{
	Q_D(qSlicerFilterTopModuleWidget);
	int curRow = d->piclist->currentRow();
	if (volumeNode) {
		AddSignalNode("Filter_Node_Click", volumeNode->GetID());
	}
	if (curRow < 0)
	{
		return;
	}
	QListWidgetItem* curWidgetItem = d->piclist->item(curRow);
	qGLFilterListItem* curFilterItem = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(curWidgetItem));
	curFilterItem->HideMarkLabel(false);
	if (m_LastFilterItem != curFilterItem && m_LastFilterItem)
	{
		m_LastFilterItem->HideMarkLabel(true);
	}
	m_LastFilterItem = curFilterItem;
	if (curFilterItem) {
		auto lis1 = d->logic()->m_TopItemDataList;
		d->logic()->m_SelectedItemData = curFilterItem->m_ItemData;
	}
	if (volumeNode) {
		volumeNode->SetSelected(true);
		DoSelectVolumeNode(volumeNode);
		
	}
	Reinit();
}
void qSlicerFilterTopModuleWidget::DoLoadFolder(QString dir) {

	QDir _dir(dir);
	QFileInfoList _fileInfoList = _dir.entryInfoList(QStringList() << "*.dcm");
	auto iomanager = qSlicerApplication::application()->ioManager();
	Dicom::itkReadDicomSeriesAnyDir(dir);
	for (auto fileInfo : _fileInfoList)
	{
		QString dirpath = dir;
		auto name1 = dirpath.append("/").append(fileInfo.fileName());
		iomanager->loadFile(name1);
		break;
	}
}

void qSlicerFilterTopModuleWidget::OnRightClickWidget(qGLFilterListItem* filterItem)
{
	Q_D(qSlicerFilterTopModuleWidget);
	QMessageBox messageBox(QMessageBox::NoIcon,
		QStringLiteral("拆分图像"), QStringLiteral("是否要拆分图像?"),
		QMessageBox::Yes | QMessageBox::No, NULL); ;
	messageBox.setButtonText(QMessageBox::Yes, QStringLiteral("是"));
	messageBox.setButtonText(QMessageBox::No, QStringLiteral("否"));

	int result = messageBox.exec();
	if (result != QMessageBox::Yes) return;
	auto filename = filterItem->m_ItemData->m_DataNode->GetAttribute("filename");
	if (!filename) {
		return;
	}
	auto fromPath = QString::fromStdString(itksys::SystemTools::GetFilenamePath(std::string(filename)));
	//QString fromPath = filterItem->m_ItemData->m_DataNode->GetAttribute("Path");
	if (fromPath == "") return;
	QString toPath = fromPath;
	toPath.append("_2");
	QDir dir(toPath);
	if (dir.exists()) {
		QMessageBox msg(QMessageBox::NoIcon, QStringLiteral("警告"), QStringLiteral("该节点已经拆分过了！"), QMessageBox::Ok);
		msg.setButtonText(QMessageBox::Ok, QStringLiteral("好"));
		msg.exec();
		return;
	}
	dir.mkdir(toPath);
	QList<QListWidgetItem*> selected = d->piclist->selectedItems();
	if (selected.size() != 1) {
		return;
	}
	QListWidgetItem* selectedItem = selected.at(0);
	qGLFilterListItem* widget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(selectedItem));
	RemoveDataNodeFromList(selectedItem);
	SplitFileFold(fromPath, toPath);

	QStringList list;
	list.append(fromPath);
	list.append(toPath);

	for (int i = 0; i < list.length(); i++) {
		/*QString dicom_shortcut = QString::fromStdString("*.dcm");
		QStringList fileNames = GetFileNameInDir(list[i], dicom_shortcut, true);
		if (fileNames.length() <= 0) {
			return;
		}
		auto iomanager = qSlicerApplication::application()->ioManager();
		iomanager->loadFile(fileNames[0]);*/
		DoLoadFolder(list[i]);
	}
	d->ShowFirstNode();
}

void qSlicerFilterTopModuleWidget::onChangePageToRegister() {
	Q_D(qSlicerFilterTopModuleWidget);
	for (int i = 0; i < d->logic()->m_TopItemDataList.size(); i++) {
		auto item = d->logic()->m_TopItemDataList.at(i);
		if (item->m_AliasName!="") {
			if (item->m_DataNode) {
				item->m_DataNode->SetName(item->m_AliasName.c_str());
			}
		}
	}
}

qGLFilterListItem* qSlicerFilterTopModuleWidget::GetWidgetByItem(TopItemData* item1) {
	Q_D(qSlicerFilterTopModuleWidget);
	for (int j = 0; j < d->piclist->count(); j++)
	{
		auto item = d->piclist->item(j);
		auto widget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(item));
		if (widget->m_ItemData == item1) {
			return widget;
		}
	}
	return nullptr;
}

void qSlicerFilterTopModuleWidget::onFreshState() {
	Q_D(qSlicerFilterTopModuleWidget);
	for (int i = 0; i < d->logic()->m_TopItemDataList.size(); i++) {
		auto item = d->logic()->m_TopItemDataList.at(i);
		if (item->m_DataNode) {
			auto filter_setted = item->m_DataNode->GetAttribute("alias_name");
			if (filter_setted) {
				GetWidgetByItem(item)->showFrontLabelFrame();
			}
			else {
				GetWidgetByItem(item)->removeFrontLabelFrame();
			}
			GetWidgetByItem(item)->SetDiscription();
		}
	}
	
}

void qSlicerFilterTopModuleWidget::SplitFileFold(QString path, QString toPath)
{
	QStringList tmpList = GetFileNameInDir(path, "*.Dcm");
	int count = tmpList.size();

	for (int i = count / 2; i < count; i++) {
		QString sourceFile = path + "/" + tmpList[i];
		QString targetFile = toPath + "/" + tmpList[i];
		bool result = QFile::copy(sourceFile, targetFile);
		QFile::remove(sourceFile);
	}
}

QStringList qSlicerFilterTopModuleWidget::GetFileNameInDir(const QString& path, QString tail, bool isNeedTotalPath)
{
	QStringList filesList;
	QDir dir(path);
	if (!dir.exists())
		return filesList;

	QStringList filters;
	filters << QString(tail);
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	dir.setNameFilters(filters);
	dir.setSorting(QDir::NoSort);
	auto entryList = dir.entryList();

	int fileNum = entryList.size();
	if (fileNum <= 0)
		return filesList;
	QCollator collator;
	collator.setNumericMode(true);

	std::sort(
		entryList.begin(),
		entryList.end(),
		[&collator](const QString& file1, const QString& file2)
		{
			return collator.compare(file1, file2) < 0;
		});

	if (!isNeedTotalPath) return entryList;
	for (int i = 0; i < fileNum; i++)
	{
		QString file_name = entryList[i];
		filesList.append(path + "/" + file_name);
	}
	return filesList;
}


void qSlicerFilterTopModuleWidget::RemoveDataNodeFromList(QListWidgetItem* item)
{
	Q_D(qSlicerFilterTopModuleWidget);
	int row = d->piclist->row(item);
	auto widget = dynamic_cast<qGLFilterListItem*>(d->piclist->itemWidget(item))->m_ItemData;
	if (widget->m_DataNode) d->logic()->RemoveVolumeNode(widget->m_DataNode);
	if (widget->m_RegisterNode) d->logic()->RemoveVolumeNode(widget->m_RegisterNode);
	
	if (widget->m_DataNode->GetAttribute("dti_node_id")) {
		auto node = mrmlScene()->GetNodeByID(widget->m_DataNode->GetAttribute("dti_node_id"));
		SafeRemove(node);
		if (widget->m_DataNode->GetAttribute("registed_node")) {
			auto node1 = mrmlScene()->GetNodeByID(widget->m_DataNode->GetAttribute("registed_node"));
			SafeRemove(node1);
		}
		if (widget->m_DataNode->GetAttribute("transform_id")) {
			auto node2 = mrmlScene()->GetNodeByID(widget->m_DataNode->GetAttribute("transform_id"));
			SafeRemove(node2);
		}
	}
	if (widget->m_DataNode->GetAttribute("dwi_node_id")) {
		auto node = mrmlScene()->GetNodeByID(widget->m_DataNode->GetAttribute("dwi_node_id"));
		SafeRemove(node);
	}

	if (widget->m_ItemFriend) {
		if (widget->m_IsTransmit) {
			//widget->m_ItemFriend->SetTransmitIcon("");
			AddSignalNode("top_filter_widget_delete_transmiticon", widget->m_ImagePath.c_str());
		}
		else {
			//widget->m_ItemFriend->SetRegistrationIcon("");
			AddSignalNode("top_filter_widget_delete_registrationicon", widget->m_ImagePath.c_str());
		}
	}
	if(m_LastFilterItem->m_ItemData == widget) {
		m_LastFilterItem = nullptr;
	}
	d->piclist->takeItem(row);

	using std::swap;
	std::swap(d->logic()->m_TopItemDataList[row], d->logic()->m_TopItemDataList.back());
	d->logic()->m_TopItemDataList.pop_back();

	
	delete item;
}

