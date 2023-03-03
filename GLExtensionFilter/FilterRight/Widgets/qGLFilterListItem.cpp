#include "qGLFilterListItem.h"

#include "ui_qGLFilterListItem.h"

class qGLFilterListItemPrivate : public Ui_qGLFilterListItem {
    Q_DECLARE_PUBLIC(qGLFilterListItem);
protected:
    qGLFilterListItem* const q_ptr;
public:
	qGLFilterListItemPrivate(qGLFilterListItem& object);

    void Clean();

    void Init();

    void SetFrontLabelVisibility(bool b);
    void SetImage(QString path);
public:
    QImage *img;
};

qGLFilterListItemPrivate::qGLFilterListItemPrivate(qGLFilterListItem& object) : q_ptr(&object)
{
}


void qGLFilterListItemPrivate::Clean()
{
    if (img) {
        delete img;
        img = nullptr;
    }
}

void qGLFilterListItemPrivate::Init()
{
    Q_Q(qGLFilterListItem);
    this->setupUi(q);
    img = NULL;

    zlabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    zlabel->setScaledContents(true);

	marklabel->hide();
}

void qGLFilterListItemPrivate::SetFrontLabelVisibility(bool b)
{
    frontlabel->setVisible(b);
}

void qGLFilterListItemPrivate::SetImage(QString path)
{
    img = new QImage; //�½�һ��image����
    img->load(path); //��ͼ����Դ�������img��ע��·�����ɵ��ͼƬ�Ҽ�����·��
    zlabel->setPixmap(QPixmap::fromImage(*img)); //��ͼƬ����label��ʹ��setPixmap,ע��ָ��*img
}


//-----------------------------------------------------------------------------
// qGLChooseDialog methods
//-----------------------------------------------------------------------------
qGLFilterListItem::qGLFilterListItem(QWidget* parent) : Superclass(parent), d_ptr(new qGLFilterListItemPrivate(*this))
{
    Q_D(qGLFilterListItem);
    d->Init();
	m_IsTransmit = false;

	m_DataNode = nullptr;
	m_ItemFriend = nullptr;
	m_RegisterNode = nullptr;
}

qGLFilterListItem::~qGLFilterListItem()
{
	Q_D(qGLFilterListItem);
	d->Clean();
}

void qGLFilterListItem::SetImage(QString path)
{
    Q_D(qGLFilterListItem);
    m_ImagePath = path;
    d->SetImage(path);
}


void qGLFilterListItem::MaskMarkLabel(bool ismark)
{
	Q_D(qGLFilterListItem);
		if (ismark)
			d->marklabel->hide();
		else
			d->marklabel->show();
}

void qGLFilterListItem::SetFriendItem(qGLChooseItem* item, bool isTransmit)
{
	Q_D(qGLFilterListItem);
	m_ItemFriend = item;
	if (item == nullptr) {
		d->SetFrontLabelVisibility(false);
		if (m_RegisterNode) {
			m_RegisterNode->RemoveAttribute("register_name");
		}
		if (m_DataNode) {
			m_DataNode->RemoveAttribute("alias_name");
			m_DataNode->RemoveAttribute("is_transmit");
			m_DataNode->RemoveAttribute("alias_transmit");
		}
		m_IsTransmit = false;
		return;
	}
	m_IsTransmit = isTransmit;
	m_AliasName = item->m_NameRegistration;
	if (isTransmit) m_AliasName = item->m_NameTransmit;

	std::string tmpAlias = m_AliasName.toStdString();
	auto info = tmpAlias.c_str();
	std::string transmit_alias = item->m_NameTransmit.toStdString();

	if (m_DataNode) {
		m_DataNode->SetAttribute("alias_name", info);
		m_DataNode->SetAttribute("is_transmit", m_IsTransmit?"1":"0");//is transmit
		if (m_IsTransmit && transmit_alias != "") {
			m_DataNode->SetAttribute("alias_transmit", transmit_alias.c_str());// the transmit node name of the registration node
		}
	}
	if (m_RegisterNode) {
		m_RegisterNode->SetAttribute("register_name", info);
	}

	d->SetFrontLabelVisibility(true);
}

void qGLFilterListItem::SetMyDataNode(vtkMRMLScalarVolumeNode* dn)
{
	m_DataNode = dn;
}

void qGLFilterListItem::SetRegisterNode(vtkMRMLScalarVolumeNode* dn)
{
	m_RegisterNode = dn;
}

void qGLFilterListItem::mouseReleaseEvent(QMouseEvent* event)
{
	auto btn = event->button();
	if (btn == Qt::MouseButton::LeftButton)
		emit(clicked(m_DataNode));
	else if (btn == Qt::MouseButton::RightButton)
		emit(rightClicked(this));
}
