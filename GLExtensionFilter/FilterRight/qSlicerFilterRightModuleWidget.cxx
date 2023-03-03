/*==============================================================================
 
  Program: 3D Slicer

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
#include "qdebug.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

// Slicer includes
#include "qSlicerFilterRightModuleWidget.h"
#include "ui_qSlicerFilterRightModuleWidget.h"

#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerIOManager.h"
#include "vtkSlicerFilterTopLogic.h"
#include "qGLFilterTools.h"
#include "itksys/SystemTools.hxx"
#include <QSettings>
#include "dicom.h"
#include "qSlicerSaveDataDialog.h"
#include "qGLMessageDialog.h"
#include "QThread.h"
#include "vtkMRMLDiffusionWeightedVolumeNode.h"
#include "dicom.h"
#include "qfileinfo.h"
#include "PythonQt.h"
#include "qJMainWindow.h"
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFilterRightModuleWidgetPrivate : public Ui_qSlicerFilterRightModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerFilterRightModuleWidget);
protected:
	qSlicerFilterRightModuleWidget* const q_ptr;
public:
	qSlicerFilterRightModuleWidgetPrivate(qSlicerFilterRightModuleWidget& object);
	~qSlicerFilterRightModuleWidgetPrivate();
	vtkSlicerFilterRightLogic* logic() const;

	//add item to alias name list by registName and transmitName
	void AddChooseItemToList(QString registName, QString transmitName);

	//change the name of the picturechooseitem isTransmit(judge registration or transmit)
	void ChangeChooseItemName(qGLChooseItem* item, QString newName, bool isTransmit);

	void Init();
	void InitBasicItem();
	bool HasRegistedNode();
	vtkMRMLScalarVolumeNode* GetNodeByAlias(const char* name);
	//init original node list
	void InitWidgetListFromDicomCollection();

	//add original node to alias name container
	void SetChooseInfo(qGLChooseItem* item, bool isTransmit);
	
	//set  a friend alias name container for an original node
	void SetItemFriend(qGLChooseItem* item, TopItemData* widget, bool isTransmit);
	bool CanAddAliasToNode(qGLChooseItem* item, TopItemData* widget, bool isTransmit);
	void SetTransmitIconInfo(qGLChooseItem* item,const char* path);
	void SetRegistrationIconInfo(qGLChooseItem* item, const char* path);
	//set  a friend alias name container for an original node
	void SetSelectInfo(qGLChooseItem* item, TopItemData* widget, bool isTransmit = false);

	bool isBasicSet = false;//is set basic node

	qGLChooseItem* m_BasicItem;//the choose item which is used to set basic node
	QStringList m_NameList;//store all the alias have been used, it's stored use lower string

	void CreatPicTextButton(QPushButton* btn, QString picPath, QString text);
};

//-----------------------------------------------------------------------------
// qSlicerFilterRightModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFilterRightModuleWidgetPrivate::qSlicerFilterRightModuleWidgetPrivate(qSlicerFilterRightModuleWidget& object) : q_ptr(&object)
{
	

}
qSlicerFilterRightModuleWidgetPrivate::~qSlicerFilterRightModuleWidgetPrivate() {}

vtkSlicerFilterRightLogic* qSlicerFilterRightModuleWidgetPrivate::logic() const {
	Q_Q(const qSlicerFilterRightModuleWidget);
	return vtkSlicerFilterRightLogic::SafeDownCast(q->logic());
}



void qSlicerFilterRightModuleWidgetPrivate::AddChooseItemToList(QString registName, QString transmitName)
{
	Q_Q(qSlicerFilterRightModuleWidget);
	QString tmpRegistName = registName.toLower();
	if (m_NameList.contains(tmpRegistName)) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("该配准项的别名已被使用！"), true);
		return;
	}
	QString tmpTransmitName = transmitName.toLower();
	if (m_NameList.contains(tmpTransmitName)) {
		qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("该传导项的别名已经被使用！"), true);
		return;
	}
	m_NameList.append(tmpRegistName);
	if (!transmitName.isEmpty()) m_NameList.append(tmpTransmitName);
	qGLChooseItem* item = new qGLChooseItem(listWidget);
	q->m_ItemListForTest.push_back(item);
	item->SetRegistAndTransmitName(registName, transmitName);
	QListWidgetItem* widgetItem = new QListWidgetItem(listWidget);
	widgetItem->setSizeHint(QSize(380, 220));
	listWidget->setItemWidget(widgetItem, item);
	q->fresh_right_ui_global_config();
	QObject::connect(item->GetBtnRegistIcon(), &QPushButton::clicked, q, [=]() {
		SetChooseInfo(item, false);
		});
	QObject::connect(item->GetBtnTransmitIcon(), &QPushButton::clicked, q, [=]() {
		SetChooseInfo(item, true);
		});

	QObject::connect(item->GetEditRegistration(), &QLineEdit::editingFinished, q, [=]() {
		ChangeChooseItemName(item, item->GetEditRegistration()->text(), false);
		});

	QObject::connect(item->GetEditTransmit(), &QLineEdit::editingFinished, q, [=]() {
		ChangeChooseItemName(item, item->GetEditTransmit()->text(), true);
		});
	QObject::connect(item->GetBtnDelete(), &QPushButton::clicked, q, [=]() {

		QStringList s_list = { QStringLiteral("是") ,QStringLiteral("否") };
		MessageResult result = qGLMessageDialog::ShowQuestion(QStringLiteral("删除别名"), QStringLiteral("是否要删除添加的别名处理框?"), BUTTON_OK_AND_CANCEL, s_list, true);

		if (result != MessageResult::OK) return;
		m_NameList.removeOne(item->GetNameRegistration().toLower());
		m_NameList.removeOne(item->GetNameTransmit().toLower());
		auto nodes2 = q->GetNodesByAttribute("register_name", item->GetNameRegistration().toLatin1());
		if (nodes2->size() != 0) {
			//QMessageBox msg(QMessageBox::NoIcon, QStringLiteral("提示"), QStringLiteral("使用该别名的节点已经配准, 不能修改！"), QMessageBox::Ok);
			qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("使用该别名的节点已经配准, 不能修改！"), true);
			return;
		}


		vtkSlicerFilterTopLogic* topLogic =
			vtkSlicerFilterTopLogic::SafeDownCast(logic()->GetModuleLogic("FilterTop"));
		std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;

		topLogic->m_Right2TopItemDataList.clear();
		TopItemData* targetTopItem = nullptr;
		qGLChooseItem* targetChooseItem = nullptr;
		int targetChooseItemIndex = -1;
		for (auto iter = list1.begin(); iter != list1.end(); iter++) {
			TopItemData* tmpItem = *iter;
			if (tmpItem->m_ItemFriend == item->m_RightItemData) {
				topLogic->m_Right2TopItemDataList.push_back(tmpItem);
				targetTopItem = tmpItem;
				
				break;
			}
		}
		for (int i = 0; i < listWidget->count(); i++) {
			QListWidgetItem* widgetItem = listWidget->item(i);
			if (widgetItem == nullptr) continue;
			QWidget* tmpWidget = listWidget->itemWidget(widgetItem);
			if (tmpWidget == nullptr) continue;
			auto widget = dynamic_cast<qGLChooseItem*>(tmpWidget);
			
			if (widget == item) {
				targetChooseItem = widget;
				targetChooseItemIndex = i;
			}
		}

		if (targetTopItem&&!CanAddAliasToNode(targetChooseItem, targetTopItem, targetTopItem->m_IsTransmit)) {
			//qGLMessageDialog::ShowInformation(QStringLiteral("提示"), QStringLiteral("Basic节点和已配准节点不能修改！"));
			qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("Basic节点和已配准节点不能修改！"), true);
			return;
		}

		if(targetTopItem)
		targetTopItem->SetFriendItem(nullptr, false);
		if (targetChooseItem) {
			SetRegistrationIconInfo(targetChooseItem, nullptr);
			SetTransmitIconInfo(targetChooseItem, nullptr);
			listWidget->takeItem(targetChooseItemIndex);
		}
		q->sendAddSignalNode("Update_topitem_ui","1");
		q->fresh_right_ui_global_config();
		});
}




void qSlicerFilterRightModuleWidgetPrivate::ChangeChooseItemName(qGLChooseItem* item, QString newName, bool isTransmit)
{
	QString oldName = item->GetNameRegistration().toLower();
	if (isTransmit) oldName = item->GetNameTransmit().toLower();
	if (newName.toLower() == oldName) return;
	if (newName.isEmpty()) {
		item->SetEditInfo();
		return;
	}

	if (m_NameList.contains(newName.toLower())) {
		/*QMessageBox msg(QMessageBox::NoIcon, QStringLiteral("警告"), QStringLiteral("该别名已经被使用！"), QMessageBox::Ok);
		msg.setButtonText(QMessageBox::Ok, QStringLiteral("好"));
		msg.exec();*/
		qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("该别名已经被使用！"), true);
		item->SetEditInfo();
		return;
	}

	m_NameList.removeOne(oldName);
	m_NameList.append(newName.toLower());
	if (isTransmit) item->GetNameTransmit() = newName;
	else item->GetNameRegistration() = newName;

	vtkSlicerFilterTopLogic* topLogic =
		vtkSlicerFilterTopLogic::SafeDownCast(logic()->GetModuleLogic("FilterTop"));
	std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;
	for (auto iter = list1.begin(); iter != list1.end(); iter++) {
		TopItemData* tmpItem = *iter;
		if (tmpItem->m_ItemFriend == item->m_RightItemData) {
			tmpItem->SetFriendItem(item->GetItemData(), tmpItem->m_IsTransmit);
		}
	}
}

void qSlicerFilterRightModuleWidgetPrivate::Init()
{
	Q_Q(qSlicerFilterRightModuleWidget);
	this->setupUi(q);
	InitBasicItem();
	//QObject::connect(btnLoad, &QPushButton::clicked, q, &qSlicerFilterRightModuleWidget::onLoadPatientFiles);
	QObject::connect(btnAdd, &QPushButton::clicked, q, &qSlicerFilterRightModuleWidget::OnAddClick);
	CreatPicTextButton(this->btnAdd, ":/FilterIcons/add.png", QStringLiteral("新增标签"));
	QObject::connect(btn_next, &QPushButton::clicked, q, &qSlicerFilterRightModuleWidget::OnGotoNextPage);
	QObject::connect(btn_preview, &QPushButton::clicked, q, &qSlicerFilterRightModuleWidget::OnGotoPreviewPage);
}

void qSlicerFilterRightModuleWidgetPrivate::InitBasicItem()
{
	Q_Q(qSlicerFilterRightModuleWidget);
	m_NameList.append("basic");
	m_BasicItem = new qGLChooseItem(gBoxContainer1);
	q->m_ItemListForTest.push_back(m_BasicItem);
	m_BasicItem->SetRegistAndTransmitName("basic", "");
	m_BasicItem->move(0, 20);
	m_BasicItem->show();
	m_BasicItem->HideDelete();

	QObject::connect(m_BasicItem->GetBtnRegistIcon(), &QPushButton::clicked, q, [=]() {
		SetChooseInfo(m_BasicItem, false);
		});

	//piclist->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	//piclist->setSpacing(12);

	
}

void qSlicerFilterRightModuleWidgetPrivate::InitWidgetListFromDicomCollection()
{
}

bool qSlicerFilterRightModuleWidgetPrivate::HasRegistedNode() {
	Q_Q(qSlicerFilterRightModuleWidget);
	std::vector<vtkMRMLNode*> sliceNodes;
	q->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", sliceNodes);
	for (unsigned int i = 0; i < sliceNodes.size(); ++i)
	{
		vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(sliceNodes[i]);
		auto origin_node = volumeNode->GetAttribute("origin_node");
		if (origin_node) {
			return true;
		}
	}
	return false;
}

//bool qSlicerFilterRightModuleWidgetPrivate::HasRegistedNode() {
//	Q_Q(qSlicerFilterRightModuleWidget);
//	std::vector<vtkMRMLNode*> sliceNodes;
//	q->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", sliceNodes);
//	for (unsigned int i = 0; i < sliceNodes.size(); ++i)
//	{
//		vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(sliceNodes[i]);
//		auto origin_node = volumeNode->GetAttribute("origin_node");
//		if (origin_node) {
//			return true;
//		}
//	}
//	return false;
//}

vtkMRMLScalarVolumeNode* qSlicerFilterRightModuleWidgetPrivate::GetNodeByAlias(const char* name) {
	Q_Q(qSlicerFilterRightModuleWidget); 
	std::vector<vtkMRMLNode*> sliceNodes;
	q->mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode", sliceNodes);
	for (unsigned int i = 0; i < sliceNodes.size(); ++i)
	{
		vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(sliceNodes[i]);
		auto alias_name = volumeNode->GetAttribute("alias_name");
		if (alias_name&&strcmp(alias_name, name) == 0) {
			return volumeNode;
		}
	}
	return nullptr;
}

bool qSlicerFilterRightModuleWidgetPrivate::CanAddAliasToNode(qGLChooseItem* item, TopItemData* widget, bool isTransmit)
{
	Q_Q(qSlicerFilterRightModuleWidget);
	if (!widget) {
		return false;
	}

	//判断是否该节点已经发生了配准行为
	const char* registed_node = widget->m_DataNode->GetAttribute("registed_node");
	if (registed_node) return false;
	auto registed_right_node_name = item->m_RightItemData->m_NameRegistration;
	auto registed_right_node = GetNodeByAlias(registed_right_node_name.c_str());
	if (registed_right_node) {
		auto attributeNames = registed_right_node->GetAttributeNames();
		if(registed_right_node->GetAttribute("registed_node"))return false;
	}

	const char* is_transmit = widget->m_DataNode->GetAttribute("is_transmit");
	if (is_transmit&&strcmp(is_transmit, "1")==0) {
		const char* alias_base = widget->m_DataNode->GetAttribute("alias_base");
		if (alias_base) {
			auto base_node = GetNodeByAlias(alias_base);
			if (base_node) {
				const char* registed_node = base_node->GetAttribute("registed_node");
				if (registed_node) return false;
			}
		}
	}

	//判断是否是basic并且已经发生了配准行为
	const char* aliasName = widget->m_DataNode->GetAttribute("alias_name");
	if (	aliasName&&strcmp(aliasName, "basic") == 0 ) {
		if (HasRegistedNode()) {
		return false;
		}
	}
	if (registed_right_node_name=="basic") {
		if (HasRegistedNode()) {
			return false;
		}
	}
	QString alias = item->GetNameRegistration();

	
	auto nodes = q->GetNodesByAttribute("alias_name",alias.toLatin1());
	if (nodes->size() > 0) {
		auto node = nodes->at(0);
		const char* state = widget->m_DataNode->GetAttribute("register_result");
		if (state&&strcmp(state, "register_ok")) return false;
	}
	return true;
}

void qSlicerFilterRightModuleWidgetPrivate::SetChooseInfo(qGLChooseItem* item, bool isTransmit)
{
	vtkSlicerFilterTopLogic* topLogic =
		vtkSlicerFilterTopLogic::SafeDownCast(logic()->GetModuleLogic("FilterTop"));
	TopItemData* itemData = topLogic->m_SelectedItemData;
	/*QList<QListWidgetItem*> selected = piclist->selectedItems();
	if (selected.size() != 1) {
		return;
	}
	QListWidgetItem* selectedItem = selected.at(0);
	qGLFilterListItem* widget = dynamic_cast<qGLFilterListItem*>(piclist->itemWidget(selectedItem));*/

	if (!CanAddAliasToNode(item, itemData, isTransmit))
	{
		//qGLMessageDialog::ShowInformation(QStringLiteral("提示"), QStringLiteral("Basic节点和已配准节点不能修改！"));
		qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("Basic节点和已配准节点不能修改！"), true);
		return;
	}
	std::cout << topLogic->m_SelectedItemData->m_ImagePath << std::endl;
	SetSelectInfo(item, itemData, isTransmit);
}

void qSlicerFilterRightModuleWidgetPrivate::SetItemFriend(qGLChooseItem* item, TopItemData* widget, bool isTransmit)
{
	Q_Q(qSlicerFilterRightModuleWidget);
	vtkSlicerFilterTopLogic* topLogic =
		vtkSlicerFilterTopLogic::SafeDownCast(logic()->GetModuleLogic("FilterTop"));
	std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;
	for (auto iter = list1.begin(); iter != list1.end(); iter++) {
		auto tmpItem = *iter;
		//如果有dcm节点frienditem和item相同, 且dcm节点的是否是传导也相同, 把dcm节点的friendItem设置为空
		if (tmpItem->m_ItemFriend == item->m_RightItemData && tmpItem->m_IsTransmit == isTransmit) {
			if (tmpItem == widget) {
				widget->SetFriendItem(nullptr, false);
				if (isTransmit) SetTransmitIconInfo(item, nullptr);
				else SetRegistrationIconInfo(item, nullptr);
				q->sendAddSignalNode("Update_topitem_ui", "yellow");//@sxm
				return;
			}
			else {
				tmpItem->SetFriendItem(nullptr, false);
			}
		}
	}
	if (isTransmit) {
		SetTransmitIconInfo(item,widget->m_ImagePath.c_str());
	}
	else {
		SetRegistrationIconInfo(item, widget->m_ImagePath.c_str());
	}

	widget->SetFriendItem(item->GetItemData(), isTransmit);
	if (item->GetNameRegistration() == "basic") {
		isBasicSet = true;
	}

	q->sendAddSignalNode("Update_topitem_ui", "yellow");//@sxm
}



void qSlicerFilterRightModuleWidgetPrivate::SetTransmitIconInfo(qGLChooseItem* item,const char* imagepath)
{
	Q_Q(qSlicerFilterRightModuleWidget);
	std::string prefix = "filter_right_transmit_";
	std::string key = prefix.append(item->m_RightItemData->m_NameTransmit);
	if(imagepath){
		item->SetTransmitIcon(imagepath);
		q->SetGlobalSaveValue(key.c_str(), imagepath);
	}
	else {
		item->SetTransmitIcon("");
		vtkMRMLNode* node = q->mrmlScene()->GetFirstNodeByName("saveconfig");
		node->RemoveAttribute(key.c_str());
	}
}

void qSlicerFilterRightModuleWidgetPrivate::SetRegistrationIconInfo(qGLChooseItem* item, const char* imagepath) {
	Q_Q(qSlicerFilterRightModuleWidget); 
	std::string prefix = "filter_right_registration_";
	std::string key = prefix.append(item->m_RightItemData->m_NameRegistration);

	if (imagepath) {
		item->SetRegistrationIcon(imagepath);
		q->SetGlobalSaveValue(key.c_str(), imagepath);
	}
	else {
		item->SetRegistrationIcon("");
		vtkMRMLNode* node = q->mrmlScene()->GetFirstNodeByName("saveconfig");
		node->RemoveAttribute(key.c_str());
	}
	
}

void qSlicerFilterRightModuleWidgetPrivate::SetSelectInfo(qGLChooseItem* item, TopItemData* widget, bool isTransmit)
{
	Q_Q(qSlicerFilterRightModuleWidget);
	if (!widget) {
		return;
	}
	//如果已经放到某个别名框里, 就清除该别名框
	if (widget->m_ItemFriend != nullptr) {
		if (widget->m_IsTransmit) {
			SetTransmitIconInfo(q->GetChooseItem(widget->m_ItemFriend), nullptr);
		}
		else {
			SetRegistrationIconInfo(q->GetChooseItem(widget->m_ItemFriend), nullptr);
		}
	}
	
	SetItemFriend(item, widget, isTransmit);
}

void qSlicerFilterRightModuleWidgetPrivate::CreatPicTextButton(QPushButton* btn, QString picPath, QString text)
{
	QLabel* labelPic = new QLabel;
	QLabel* labelText = new QLabel;
	labelPic->setPixmap(QPixmap(picPath));
	labelPic->setStyleSheet("background: transparent;border-radius: 4px 4px 4px 4px;");
	labelText->setText(text);
	labelText->setStyleSheet("font: 20px 'Source Han Sans CN-Regular, Source Han Sans CN';");
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addSpacing(2);
	layout->addWidget(labelPic);
	layout->addSpacing(2);
	layout->addWidget(labelText);
	layout->addStretch();
	btn->setLayout(layout);
	//btn->setStyleSheet("background: #525F7B;");
}







//-----------------------------------------------------------------------------
// qSlicerFilterRightModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerFilterRightModuleWidget::qSlicerFilterRightModuleWidget(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSlicerFilterRightModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerFilterRightModuleWidget::~qSlicerFilterRightModuleWidget()
{
	
}

//-----------------------------------------------------------------------------
void qSlicerFilterRightModuleWidget::setup()
{
	Q_D(qSlicerFilterRightModuleWidget);
	this->Superclass::setup();
	d->Init();

	

}

void qSlicerFilterRightModuleWidget::DoLoadFolder(QString dir) {
	qDebug() << "DoLoadFolder" << dir;
	QDir _dir(dir);
	QFileInfoList _fileInfoList = _dir.entryInfoList();
	auto iomanager = qSlicerApplication::application()->ioManager();
	Dicom::itkReadDicomSeriesAnyDir(dir);

	for (auto fileInfo:_fileInfoList) 
	{
		QString dirpath = dir;
		auto name1 = dirpath.append("/").append(fileInfo.fileName());
		QFileInfo fl(name1);
		if (!fl.isFile()) {
			continue;
		}
		qDebug() << "loadFile" << name1;
		iomanager->loadFile(name1);

		break;
	}
}

void qSlicerFilterRightModuleWidget::fresh_right_ui_global_config() {
	/*auto ui_conifg = GetGlobalSaveValue("right_ui_global_config");
	if (!ui_conifg) {
		return;
	}*/
	Q_D(qSlicerFilterRightModuleWidget);
	QString value = "";
	for (int i = 0; i < d->listWidget->count(); i++) {
		QListWidgetItem* widgetItem = d->listWidget->item(i);
		if (widgetItem == nullptr) continue;
		QWidget* tmpWidget = d->listWidget->itemWidget(widgetItem);
		if (tmpWidget == nullptr) continue;
		auto widget = dynamic_cast<qGLChooseItem*>(tmpWidget);
		value = value + widget->GetNameRegistration();
		value = value + "&&";
		value = value + widget->GetNameTransmit();
		if (i != d->listWidget->count() - 1) {
			value = value + "||";
		}
	}
	//qDebug() << "right_ui_global_config:" << value;
	SetGlobalSaveValue("right_ui_global_config", value.toStdString().c_str());
}

void qSlicerFilterRightModuleWidget::sendAddSignalNode(const char* name, const char* data)
{
	Q_D(qSlicerFilterRightModuleWidget);
	AddSignalNode(name, data);
}
void qSlicerFilterRightModuleWidget::OnAddClick() {
	Q_D(qSlicerFilterRightModuleWidget);
	qGLChooseDialog* choose = new qGLChooseDialog();
	m_choose = choose;
	choose->show();
	ATB(1);
	int result = choose->exec();
	if (result == 1) {
		QString tmpRegistName = choose->GetRegistrationName();
		QString tmpTransmitName = choose->GetTransmitName();
		d->AddChooseItemToList(tmpRegistName, tmpTransmitName);
	}

	if (choose) {
		delete choose;
		choose = nullptr;
	}
	
}



void qSlicerFilterRightModuleWidget::onGotoPreviewPage() {
	auto mw = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	//mw->SetPreviewPage();
}


void qSlicerFilterRightModuleWidget::onLogicModified(vtkObject* vtkNotUsed(caller), vtkObject* callData)
{
	Q_D(qSlicerFilterRightModuleWidget);
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
	SafeRemove(node);
	if (help_node && strcmp(help_node, "true") == 0) {
		
		if (type && value) {
			if (strcmp(type, "top_filter_widget_delete_transmiticon") == 0) {
				mrmlScene()->RemoveNode(node);
				vtkSlicerFilterTopLogic* topLogic =
					vtkSlicerFilterTopLogic::SafeDownCast(d->logic()->GetModuleLogic("FilterTop"));
				std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;
				for (auto iter = list1.begin(); iter != list1.end(); iter++) {
					TopItemData* tmpItem = *iter;
					if (tmpItem->m_ImagePath == value) {
						if (tmpItem->m_ItemFriend) {
							GetChooseItem(tmpItem->m_ItemFriend)->SetTransmitIcon("");
							return;
						}
					}
				}
			}
			if (strcmp(type, "top_filter_widget_delete_registrationicon") == 0) {
				mrmlScene()->RemoveNode(node);
				vtkSlicerFilterTopLogic* topLogic =
					vtkSlicerFilterTopLogic::SafeDownCast(d->logic()->GetModuleLogic("FilterTop"));
				std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;
				for (auto iter = list1.begin(); iter != list1.end(); iter++) {
					TopItemData* tmpItem = *iter;
					if (tmpItem->m_ImagePath == value) {
						if (tmpItem->m_ItemFriend) {
							GetChooseItem(tmpItem->m_ItemFriend)->SetRegistrationIcon("");
							return;
						}
					}
				}
			}
			if (strcmp(type, "animate_right_click") == 0) {
				qGLChooseItem* item = GetChooseItem(value);
				d->SetChooseInfo(item, false);
			}
			if (strcmp(type, "load_data_from_filter")==0) {
				auto filePath = value;
				QDir directory(filePath);
				QStringList vec;
				QFileInfoList _fileInfoList = directory.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
				for (int i = 0; i < _fileInfoList.size(); i++)
				{
					//DoLoadFolder(_fileInfoList.at(i).filePath().toStdString().c_str());
					vec << _fileInfoList.at(i).filePath();
				}
				DoLoadPath(vec.join(";"));
				/*for (const QFileInfo& finfo : directory.entryInfoList())
				{
					QDir dir(finfo.absoluteFilePath());
					QFileInfo file(finfo.absoluteFilePath());
					if (file.isDir())
					{
						if (dir.dirName().size() > 0)
						{
							vec << finfo.filePath();
						}
					}
				}*/
				//DoLoadPath(vec.join(";"));
			}
			if (strcmp(type, "set_item_friend_dwi") == 0) {
				qDebug() << "set_item_friend_dwi";
				if (!value) {
					return;
				}
				auto baseline_node = mrmlScene()->GetNodeByID(value);
				if (!baseline_node) {
					return;
				}
				qGLChooseItem* item = GetChooseItem("DWI");
				TopItemData* ti = GetTopItemData(vtkMRMLScalarVolumeNode::SafeDownCast(baseline_node));
				qDebug() << item;
				qDebug() << vtkMRMLScalarVolumeNode::SafeDownCast(baseline_node);
				qDebug() << ti;
				d->SetSelectInfo(item, ti, false);
			}
		}
	}
}

void qSlicerFilterRightModuleWidget::OnNodeAdd()
{
	Q_D(qSlicerFilterRightModuleWidget);
	vtkMRMLScalarVolumeNode* node = d->logic()->m_NodeList.at(d->logic()->m_NodeList.size() - 1);

	AddSignalNode("filter_add_node", node->GetID());
}




void qSlicerFilterRightModuleWidget::enter() {
	this->Superclass::enter();
	Q_D(qSlicerFilterRightModuleWidget);
	AddSignalNode("layout_3", QStringList());
	SetGlobalSaveValue("filter_set", "1");
	SetGlobalSaveValue("current_page", "2");
	QTimer::singleShot(0, this, &qSlicerFilterRightModuleWidget::DoAnalyseParasFromPatientModule);
}

void qSlicerFilterRightModuleWidget::DoSetInitMultiWidgetState() {
	auto layoutmanager = qSlicerApplication::application()->layoutManager();
	for (int i = 0; i < layoutmanager->threeDViewCount(); i++) {
		auto widget = layoutmanager->threeDWidget(i);
		auto view = layoutmanager->threeDWidget(i)->threeDView();
		widget->threeDController()->set3DAxisLabelVisible(false);
		widget->threeDController()->set3DAxisVisible(false);
		widget->threeDController()->setBlackBackground();
	}
}

void qSlicerFilterRightModuleWidget::exit() {
	this->Superclass::exit();
	Q_D(qSlicerFilterRightModuleWidget);
	qvtkDisconnect(d->logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
	qvtkDisconnect(d->logic(), vtkSlicerFilterRightLogic::NodeAddEvent, this, SLOT(OnNodeAdd()));

	//Navigation模块负责接收判断
}


void qSlicerFilterRightModuleWidget::DoAnalyseParasFromPatientModule() {
	Q_D(qSlicerFilterRightModuleWidget);
	/**
		@newAnalysisFilePath
		路径列表
		@patientAnalyseId
		分析ID
		@init_analyse
		是不是还没有开始分析
	*/
	const char* val1 = GetGlobalSaveValue("newAnalysisFilePath");
	const char* val2 = GetGlobalSaveValue("patientAnalyseId");                                                                                       
	const char* val3 = GetGlobalSaveValue("init_analyse");
	auto im = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	if (val3 && strcmp(val3, "99") == 0){
		qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
		qvtkReconnect(logic(), vtkSlicerFilterRightLogic::NodeAddEvent, this, SLOT(OnNodeAdd()));
		return;
	}
	
	//只有从分析界面过来的操作会触发加载数据
	if (val3 && strcmp(val3, "1") == 0) 
	{
		DoResetUI();
		qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
		qvtkReconnect(logic(), vtkSlicerFilterRightLogic::NodeAddEvent, this, SLOT(OnNodeAdd()));
		QString s_pathlist = QString::fromLocal8Bit(val1);
		d->AddChooseItemToList("T2", "PCA");
		d->AddChooseItemToList("Flair", "");
		d->AddChooseItemToList("CT", "PET");
		d->AddChooseItemToList("DWI", "");
		DoLoadPath(s_pathlist);
		
		SetGlobalSaveValue("init_analyse", "99");
	}
	//还原数据(打开最近分析)
	if (val3 && strcmp(val3, "2") == 0) {
		SetGlobalSaveValue("init_analyse", "99");
		qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
		qvtkReconnect(logic(), vtkSlicerFilterRightLogic::NodeAddEvent, this, SLOT(OnNodeAdd()));
		im->SetProgress(12);
		qApp->processEvents();
		DoResetUI();
		im->SetProgress(20);
		qApp->processEvents();
		auto fileName = GetMRBFileName();
		const char* fileNameChar = fileName.toLatin1();
		QFile file(fileName);
		bool isBackup = false;
		if (file.exists()) {
			AddSignalNode("fresh_navigation", "true");
			vtkMRMLNode* node = mrmlScene()->GetFirstNodeByName("saveconfig");
			mrmlScene()->RemoveNode(node);
			auto iomanager = qSlicerApplication::application()->ioManager();
			iomanager->loadFile(fileName);
			const char* custom_config = GetGlobalSaveValue("right_ui_global_config");
			if (!custom_config) {
				d->AddChooseItemToList("T2", "PCA");
				d->AddChooseItemToList("Flair", "");
				d->AddChooseItemToList("CT", "PET");
				d->AddChooseItemToList("DWI", "");
			}
			else {
				QString qc = QString::fromLocal8Bit(custom_config);
				QStringList qclist = qc.split("||");
				for (int i = 0; i < qclist.size(); i++) {
					QString qcitem = qclist.at(i);
					QStringList qcitemlist = qcitem.split("&&");
					d->AddChooseItemToList(qcitemlist.at(0), qcitemlist.at(1));
				}
			}

			
			AddSignalNode("init_data_list", "true");
			SetGlobalSaveValue("init_analyse", "99");
			
		}
		else {
			d->AddChooseItemToList("T2", "PCA");
			d->AddChooseItemToList("Flair", "");
			d->AddChooseItemToList("CT", "PET");
			d->AddChooseItemToList("DWI", "");
			AddSignalNode("init_data_list", "false");;
		}
		im->SetProgress(80);
		
		auto keys = GetGlobalKeys();
		for (int i = 0; i < keys.size(); i++) {
			std::string key1 = keys.at(i);
			std::string pre1 = "filter_right_registration_";
			std::string pre2 = "filter_right_transmit_";
			int i1 = key1.find(pre1);
			int i2 = key1.find(pre2);
			if (i1 != -1) {
				std::string realkey = key1.substr(pre1.length(), key1.length());
				auto value = GetGlobalSaveValue(key1.c_str());
				qGLChooseItem* item = GetChooseItem(realkey);
				TopItemData* ti = GetTopItemData(std::string(value));
				d->SetSelectInfo(item, ti, false);
			}
			if (i2 != -1) {
				std::string realkey = key1.substr(pre2.length(), key1.length());
				auto value = GetGlobalSaveValue(key1.c_str());
				qGLChooseItem* item = GetChooseItem(realkey);
				TopItemData* ti = GetTopItemData(std::string(value));
				d->SetSelectInfo(item, ti, true);
			}
		}
		qvtkReconnect(logic(), vtkCommand::ModifiedEvent, this, SLOT(onLogicModified(vtkObject*, vtkObject*)));
		qvtkReconnect(logic(), vtkSlicerFilterRightLogic::NodeAddEvent, this, SLOT(OnNodeAdd()));
		auto current_page = GetGlobalSaveValue("current_page");
		HideAllMarkupiducialNodes();
		//用来解决头架识别加载之后消失的BUG
		vtkMRMLNode* node = nullptr;
		vtkCollectionSimpleIterator it;
		for (mrmlScene()->GetNodes()->InitTraversal(it);
			(node = (vtkMRMLNode*)mrmlScene()->GetNodes()->GetNextItemAsObject(it));)
		{
			if (node->GetAttribute("polydata_model_transform_to_segmentation_save_bug")) {
				node->SetAttribute("save_bug_flag", "1");
			}
		}
		if (current_page) {
			
			QString current_pagestr = QString::number(std::stoi(current_page) + 1);
			auto cn = mrmlScene()->AddNewNodeByClass("vtkMRMLColorNode");
			cn->SetAttribute("value", current_pagestr.toStdString().c_str());
			mrmlScene()->InvokeEvent(1000005, cn);
			mrmlScene()->RemoveNode(cn);

			/*PythonQt::init();
			PythonQtObjectPtr context = PythonQt::self()->getMainModule();
			context.evalScript(QString(
				"import slicer.util as util;widget = util.getModuleWidget('%1'); widget.SetPage(%2);").arg("PUltraSoundPlan").arg(current_pagestr));*/
		
		}
		qDebug() << "current page is :" << current_page;
		HideAllMarkupiducialNodes();
		im->SetProgress(100);
		DoSetInitMultiWidgetState();
		AddSignalNode("fresh_left_filter_list", "1");

		


	}
	
}

void qSlicerFilterRightModuleWidget::HideAllMarkupiducialNodes() {
	Q_D(qSlicerFilterRightModuleWidget);
	std::vector<vtkMRMLNode*> viewNodes;
	auto nodes = mrmlScene()->GetNodesByClass("vtkMRMLMarkupsFiducialNode", viewNodes);
	for (int i = 0; i < viewNodes.size(); i++) {
		vtkMRMLDisplayableNode* node = vtkMRMLDisplayableNode::SafeDownCast(viewNodes[i]);
		auto displayNode = node->GetDisplayNode();
		if (!displayNode) {
			node->CreateDefaultDisplayNodes();
			displayNode = node->GetDisplayNode();
			displayNode->SetVisibility(false);
		}
	}

}

void qSlicerFilterRightModuleWidget::DoLoadPath(QString s_pathlist) {
	Q_D(qSlicerFilterRightModuleWidget);
	qDebug() << "DoLoadPath" << s_pathlist;
	QStringList pathlist = s_pathlist.split(";");
	//@sxm
	auto im = dynamic_cast<qJMainWindow*>(qSlicerApplication::application()->mainWindow());
	im->ShowProgressBar(QStringLiteral("正在加载数据"));
	int currentvalue =0;
	int d1 = (100 - currentvalue) / (pathlist.size());
	im->SetProgressStep(d1-1);
	for (int i = 0; i < pathlist.size(); i++) {
		QString path1 = pathlist.at(i);
		DoLoadFolder(path1.toStdString().c_str());
		im->UpdateProgress();
	}

	

	auto nodes = mrmlScene()->GetNodesByClass("vtkMRMLScalarVolumeNode");
	vtkMRMLNode* node;
	vtkCollectionSimpleIterator it;
	for (nodes->InitTraversal(it);
		(node = (vtkMRMLNode*)nodes->GetNextItemAsObject(it));)
	{
		//if (node->GetAttribute("filename"))
		{
			auto scanType = node->GetAttribute("scanType");
			auto patientName = node->GetAttribute("patientName");
			if (QString::fromLatin1(scanType).toLower().indexOf("ct") != -1) {
				qGLChooseItem* item = GetChooseItem("CT");
				TopItemData* ti = GetTopItemData(vtkMRMLScalarVolumeNode::SafeDownCast(node));
				d->SetSelectInfo(item, ti, false);
			}
			else {
				if (QString::fromLatin1(patientName).toLower().indexOf("t1") != -1) {
					qGLChooseItem* item = GetChooseItem("basic");
					TopItemData* ti = GetTopItemData(vtkMRMLScalarVolumeNode::SafeDownCast(node));
					d->SetSelectInfo(item, ti, false);
				}
				if (QString::fromLatin1(patientName).toLower().indexOf("flair") != -1) {
					qGLChooseItem* item = GetChooseItem("Flair");
					TopItemData* ti = GetTopItemData(vtkMRMLScalarVolumeNode::SafeDownCast(node));
					d->SetSelectInfo(item, ti, false);
				}
			}
		}
	}

	im->SetProgress(100);
}


void qSlicerFilterRightModuleWidget::OnGotoPreviewPage() {
	{
		AddSignalNode("OnExitAnalyseBtn", "1");
	}
}

void qSlicerFilterRightModuleWidget::OnGotoNextPage() {
	auto cn = mrmlScene()->GetFirstNodeByClass("vtkMRMLColorNode");
	cn->SetAttribute("value", "4");
	mrmlScene()->InvokeEvent(1000005, cn);
}

QString qSlicerFilterRightModuleWidget::GetMRBFileName()
{
	Q_D(qSlicerFilterRightModuleWidget);
	const char* index = GetGlobalSaveValue("patientAnalyseId");
	QString fileName = "";
	//if (!index) 
	if(true)
	{
		const char* filepath = GetGlobalSaveValue("newAnalysisFilePath");
		fileName = QString::fromLocal8Bit(filepath);
		auto project_path = qJMainWindow::GetProjectBasePath();
		fileName = project_path + "/Resources/save.mrb";
	}
	else {
		QString curDirPath = QCoreApplication::applicationDirPath();
		QString backupPath = QStringLiteral("%1/%2").arg(curDirPath).arg("bak");
		fileName = QStringLiteral("%1/%2.mrb").arg(backupPath).arg(index);
	}
	return fileName;
}

TopItemData* qSlicerFilterRightModuleWidget::GetTopItemData(std::string alias) {
	Q_D(qSlicerFilterRightModuleWidget);
	vtkSlicerFilterTopLogic* topLogic =
		vtkSlicerFilterTopLogic::SafeDownCast(d->logic()->GetModuleLogic("FilterTop"));
	std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;
	for (auto iter = list1.begin(); iter != list1.end(); iter++) {
		TopItemData* tmpItem = *iter;
		if (tmpItem->m_ImagePath == alias) {
			return tmpItem;
		}
	}
	return nullptr;
}

TopItemData* qSlicerFilterRightModuleWidget::GetTopItemData(vtkMRMLScalarVolumeNode* node) {
	Q_D(qSlicerFilterRightModuleWidget);
	vtkSlicerFilterTopLogic* topLogic =
		vtkSlicerFilterTopLogic::SafeDownCast(d->logic()->GetModuleLogic("FilterTop"));
	std::vector<TopItemData*> list1 = topLogic->m_TopItemDataList;
	for (auto iter = list1.begin(); iter != list1.end(); iter++) {
		TopItemData* tmpItem = *iter;
		if (tmpItem->m_DataNode == node) {
			return tmpItem;
		}
	}
	return nullptr;
}

qGLChooseItem* qSlicerFilterRightModuleWidget::GetChooseItem(std::string alias) {
	for (int i = 0; i < m_ItemListForTest.size(); i++) {
		auto item = m_ItemListForTest.at(i);
		if (QString::fromStdString(item->m_RightItemData->m_NameRegistration).toLower() == QString::fromStdString(alias).toLower() ||
			QString::fromStdString(item->m_RightItemData->m_NameTransmit).toLower() == QString::fromStdString(alias).toLower()) {
			return item;
		}
	}
	return nullptr;
}

qGLChooseItem* qSlicerFilterRightModuleWidget::GetChooseItem(RightItemData* itemData) {
	auto widget = itemData->m_Widget;
	if (widget) {
		return reinterpret_cast<qGLChooseItem*>(widget);
	}
	return nullptr;
}


void qSlicerFilterRightModuleWidget::DoResetUI() {
	Q_D(qSlicerFilterRightModuleWidget);
	//d->piclist->clear();
	d->listWidget->clear();
	d->m_NameList.clear();
	m_ItemListForTest.clear();
	d->InitBasicItem();
}

void qSlicerFilterRightModuleWidget::onLoadPatientFiles() {
	Q_D(qSlicerFilterRightModuleWidget);
	qSlicerIO::IOProperties properties;
	properties["fileName"] = "E:/123.mrml";

	bool success = qSlicerApplication::application()->coreIOManager()->saveNodes(QString("SceneFile"), properties, nullptr);




	return;
	auto iomanager = qSlicerApplication::application()->ioManager();
	QString filePath = QFileDialog::getExistingDirectory();
	
	if (filePath.isEmpty())
		return;

	/*if (m_BeforeOriginNodeDataSet) {
		for (auto iter = m_BeforeOriginNodeDataSet->begin(); iter != m_BeforeOriginNodeDataSet->end(); ++iter)
		{
			mitk::DataNode* node = (*iter).GetPointer();
			QString path1 = dynamic_cast<mitk::StringProperty*>(node->GetProperty("path"))->GetValue();
			if (path1 == filePath) {
				GLMessageBox msg;
				msg.queRenInfo(QStringLiteral("警告"), QStringLiteral("该文件夹已经导入！"));
				return;
			}
		}
	}*/
	

	QString dicom_shortcut = QString::fromStdString("*.dcm");
	QStringList fileNames = qGLFilterTools::GetFileNameInDir(filePath, dicom_shortcut, true);
	pathOfNodeAdded = filePath;
	if (fileNames.length() <= 0) {
		return;
	}
	iomanager->loadFile(fileNames[0]);
}



bool qSlicerFilterRightModuleWidget::AutoTestBreak(int i) {
	//Q_D(qSlicerFilterRightModuleWidget);

	//bool JTest = false;
	//if (JTest) {
	//	AddSignalNode("goto_page", "3");
	//	return true;
	//}

	//auto xml_root = qSlicerApplication::application()->layoutManager()->m_XMLRoot;
	//if (!xml_root) {
	//	return false;
	//}
	//auto xml_filter = xml_root->GetChild("filter");
	//if (!xml_filter) {
	//	return false;
	//}
	//itk::DOMNode::ChildrenListType groupList;
	//xml_filter->GetChildren("group", groupList);

	//auto r_save = xml_filter->GetChild("load_save");
	//if (r_save) {
	//	auto init_page = r_save->GetChild("init_page");
	//	if (init_page) {
	//		auto s_next_page = init_page->GetTextChild()->GetText();
	//		AddSignalNode("goto_page", s_next_page.c_str());
	//	}
	//	qSlicerApplication::application()->layoutManager()->m_XMLRoot = nullptr;
	//	return false;
	//}

	//if (i == 0) {
	//	int delay = 0;
	//	itk::DOMNode::ChildrenListType clickList;
	//	clickList.clear();
	//	groupList.at(i)->GetChildren("click", clickList);
	//	for (auto& c : clickList)
	//	{
	//		std::string s_delay = GetXMLText(c, "delay");
	//		std::string screen_x = GetXMLText(c, "x");
	//		std::string screen_y = GetXMLText(c, "y");
	//		std::string s_text = GetXMLText(c, "text");
	//		std::string s_testid = GetXMLText(c, "testId");
	//		std::string screenshot = GetXMLText(c, "savescreen");
	//		delay += QString::fromStdString(s_delay).toInt();

	//		if (screen_x != "" && screen_y != "") {
	//			double x = QString::fromStdString(screen_x).toDouble();
	//			double y = QString::fromStdString(screen_y).toDouble();
	//			auto pressEvent = new QMouseEvent(QEvent::MouseButtonPress, QPoint(x, y), QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	//			auto releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(x, y), QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

	//			QTimer::singleShot(delay, [=]() {
	//				//QApplication::sendEvent(w2, pressEvent);
	//				//QApplication::sendEvent(w2, releaseEvent);

	//				::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
	//				::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
	//				::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);

	//				});
	//		}

	//		if (screenshot == "ss")
	//		{
	//			QTimer::singleShot(delay + 1000, [=]() {
	//				qSlicerApplication::SaveScreenShot(QString::fromStdString(s_testid));
	//				});
	//		}

	//	}//for
	//	/*auto exit = xml_root->GetChild("filter")->GetChild("exit");
	//	if (exit)
	//	{
	//		auto exitmark = exit->GetTextChild()->GetText();
	//		if (exitmark == "0") {

	//			qSlicerApplication::application()->layoutManager()->m_XMLRoot = nullptr;

	//		}
	//	}*/
	//}
	//if (i == 1) {
	//	int delay = 0;

	//	itk::DOMNode::ChildrenListType clickList;
	//	clickList.clear();
	//	groupList.at(i)->GetChildren("click", clickList);
	//	for (auto& c : clickList)
	//	{
	//		std::string s_delay = GetXMLText(c, "delay");
	//		std::string screen_x = GetXMLText(c, "x");
	//		std::string screen_y = GetXMLText(c, "y");
	//		std::string s_text = GetXMLText(c, "text");
	//		std::string s_clearUi = GetXMLText(c, "clearUi");
	//		std::string s_testid = GetXMLText(c, "testId");
	//		std::string screenshot = GetXMLText(c, "savescreen");
	//		delay += QString::fromStdString(s_delay).toInt();

	//		if (screen_x != "" && screen_y != "") {
	//			double x = QString::fromStdString(screen_x).toDouble();
	//			double y = QString::fromStdString(screen_y).toDouble();
	//			auto pressEvent = new QMouseEvent(QEvent::MouseButtonPress, QPoint(x, y), QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	//			auto releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(x, y), QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

	//			QTimer::singleShot(delay, [=]() {
	//				//QApplication::sendEvent(w2, pressEvent);
	//				//QApplication::sendEvent(w2, releaseEvent);

	//				::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
	//				::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);
	//				::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, x / 1920 * 65535, y / 1080 * 65535, 0, 0);

	//				if (screen_x == "850" && screen_y == "385")
	//				{ //配准项坐标
	//					m_choose->setRegistrationName(QString::fromStdString(s_text));
	//				}

	//				if (screen_x == "850" && screen_y == "445")
	//				{ //配准参考项坐标
	//					m_choose->setTransmitName(QString::fromStdString(s_text));
	//				}
	//				});
	//		}
	//		if(s_clearUi=="yes")
	//		{//刷新UI
	//			QTimer::singleShot(delay + 1000, [=]() {
	//				m_choose->setRegistrationName("");
	//				m_choose->setTransmitName("");
	//				m_choose->setchnTransmitState(false);
	//				});
	//		}
	//		if (screenshot == "ss")
	//		{
	//			QTimer::singleShot(delay + 1000, [=]() {
	//				qSlicerApplication::SaveScreenShot(QString::fromStdString(s_testid));
	//				});
	//		}

	//	}//for

	//	auto exit = xml_root->GetChild("filter")->GetChild("exit");
	//	if (exit)
	//	{
	//		auto exitmark = exit->GetTextChild()->GetText();
	//		if (exitmark == "0") {
	//			QTimer::singleShot(delay + 5000, [=]() {
	//				//AddSignalNode("exit_with_save", "2");
	//				});
	//		}
	//		qSlicerApplication::application()->layoutManager()->m_XMLRoot = nullptr;
	//	}
	//}
	return false;
}
