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
	QImage* img;
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
	Q_Q(qGLFilterListItem);
	img = new QImage; //新建一个image对象
	img->load(path); //将图像资源载入对象img，注意路径，可点进图片右键复制路径
	zlabel->setPixmap(QPixmap::fromImage(*img)); //将图片放入label，使用setPixmap,注意指针*img
}


//-----------------------------------------------------------------------------
// qGLChooseDialog methods
//-----------------------------------------------------------------------------
qGLFilterListItem::qGLFilterListItem(QWidget* parent) : Superclass(parent), d_ptr(new qGLFilterListItemPrivate(*this))
{
	Q_D(qGLFilterListItem);
	d->Init();
	m_ItemData = new TopItemData();
}

qGLFilterListItem::~qGLFilterListItem()
{
	Q_D(qGLFilterListItem);
	d->Clean();
	delete m_ItemData;
}

void qGLFilterListItem::SetDiscription() {
	Q_D(qGLFilterListItem);
	if (!m_ItemData->m_DataNode) {
		return;
	}
	const char* cname = m_ItemData->m_DataNode->GetAttribute("patientName");
	auto label_inner = QString::fromLatin1(cname);
	int len1 = label_inner.length();
	if ( len1 > 15) {
		label_inner = label_inner.mid(0, 13);
		label_inner = label_inner + "...";
	}
	d->label->setText(label_inner);
}

void qGLFilterListItem::SetImage(QString path)
{
	Q_D(qGLFilterListItem);
	m_ItemData->m_ImagePath = path.toStdString();
	d->SetImage(path);
}

void qGLFilterListItem::setFrontLabelImage(QString path)
{
	Q_D(qGLFilterListItem);
	//QImage img(path);
	//d->frontlabel->setPixmap(QPixmap::fromImage(img));
	d->frontlabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	d->frontlabel->setScaledContents(true);

	QImage* image = new QImage; //新建一个image对象
	image->load(path); //将图像资源载入对象img，注意路径，可点进图片右键复制路径
	d->frontlabel->setAttribute(Qt::WA_TranslucentBackground);
	d->frontlabel->setPixmap(QPixmap::fromImage(*image)); //将图片放入label，使用setPixmap,注意指针*img
}

void qGLFilterListItem::removeFrontLabelFrame()
{
	Q_D(qGLFilterListItem);
	d->SetFrontLabelVisibility(false);
}

void qGLFilterListItem::showFrontLabelFrame()
{
	Q_D(qGLFilterListItem);
	setFrontLabelImage(":/FilterIcons/choosed.png");
	d->SetFrontLabelVisibility(true);
}

void qGLFilterListItem::HideMarkLabel(bool ismark)
{
	Q_D(qGLFilterListItem);
	if (!d) {
		return;
	}
	if (ismark)
	{
		d->marklabel->hide();
		markLabelVisible = false;
	}
	else
	{
		d->marklabel->show();
		markLabelVisible = true;
	}
}

void qGLFilterListItem::SetFriendItem(RightItemData* item, bool isTransmit)
{
	Q_D(qGLFilterListItem);
	d->SetFrontLabelVisibility(false);
	m_ItemData->SetFriendItem(item, isTransmit);
	d->SetFrontLabelVisibility(true);
}



void qGLFilterListItem::mouseReleaseEvent(QMouseEvent* event)
{
	auto btn = event->button();
	if (btn == Qt::MouseButton::LeftButton)
		emit(clicked(m_ItemData->m_DataNode));
	else if (btn == Qt::MouseButton::RightButton)
		emit(rightClicked(this));
}
