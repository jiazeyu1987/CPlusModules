#include "qGLChooseItem.h"
 
#include "ui_qGLChooseItem.h"

class qGLChooseItemPrivate : public Ui_qGLChooseItem {
    Q_DECLARE_PUBLIC(qGLChooseItem);
protected:
    qGLChooseItem* const q_ptr;

public:
    qGLChooseItemPrivate(qGLChooseItem& object);
    void HideDelete();
    void Init();
    void SetTransmitVisible(bool isVisible);
};

qGLChooseItemPrivate::qGLChooseItemPrivate(qGLChooseItem& object) : q_ptr(&object)
{
}

void qGLChooseItemPrivate::HideDelete()
{
    btnDelete1->hide();
    editRegistName->setEnabled(false);
    editTransmitName->setEnabled(false);
}

void qGLChooseItemPrivate::Init()
{
    Q_Q(qGLChooseItem);
    this->setupUi(q);
}

void qGLChooseItemPrivate::SetTransmitVisible(bool isVisible)
{
    btnTransmitIcon->setVisible(isVisible);
    editTransmitName->setVisible(isVisible);
    lblBg2->setVisible(isVisible);
}

//-----------------------------------------------------------------------------
// qGLChooseDialog methods
//-----------------------------------------------------------------------------
qGLChooseItem::qGLChooseItem(QWidget* parent)
    : Superclass(parent), d_ptr(new qGLChooseItemPrivate(*this)) {
    Q_D(qGLChooseItem);
    d->Init();
    m_RightItemData = new RightItemData();
    m_RightItemData->m_Widget = this;
    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

qGLChooseItem::~qGLChooseItem()
{
    delete m_RightItemData;
}

QLineEdit* qGLChooseItem::GetEditRegistration()
{
    Q_D(qGLChooseItem);
    return d->editRegistName;
}

QLineEdit* qGLChooseItem::GetEditTransmit()
{
    Q_D(qGLChooseItem);
    return d->editTransmitName;
}

QPushButton* qGLChooseItem::GetBtnDelete()
{
    Q_D(qGLChooseItem);
    return d->btnDelete1;
}

QPushButton* qGLChooseItem::GetBtnRegistIcon()
{
    Q_D(qGLChooseItem);
    return d->btnRegistIcon;
}

QPushButton* qGLChooseItem::GetBtnTransmitIcon()
{
    Q_D(qGLChooseItem);
    return d->btnTransmitIcon;
}

void qGLChooseItem::HideDelete()
{
    Q_D(qGLChooseItem);

    d->HideDelete();
}

void qGLChooseItem::SetEditInfo()
{
    Q_D(qGLChooseItem);
    d->editRegistName->setText(QString::fromStdString(m_RightItemData->m_NameRegistration));
    d->editTransmitName->setText(QString::fromStdString(m_RightItemData->m_NameTransmit));
}

void qGLChooseItem::SetRegistAndTransmitName(QString registName, QString transmitName)
{
    Q_D(qGLChooseItem);
    m_RightItemData->m_NameRegistration = registName.toStdString();
    m_RightItemData->m_NameTransmit = transmitName.toStdString();
    bool isOpenTransmit = !(transmitName.isEmpty());
    d->SetTransmitVisible(isOpenTransmit);
    SetEditInfo();
}

void qGLChooseItem::SetRegistrationIcon(QString path)
{
    Q_D(qGLChooseItem);
    m_RightItemData->m_NameRegistrationIcon = (path.toStdString());
    QIcon icon(path);
    d->btnRegistIcon->setIcon(icon);
    d->btnRegistIcon->setIconSize(QSize(175, 175));
}

void qGLChooseItem::SetTransmitIcon(QString path)
{
    Q_D(qGLChooseItem);
    m_RightItemData->m_NameTransmitIcon = (path.toStdString());
    QIcon icon(path);
    d->btnTransmitIcon->setIcon(icon);
    d->btnTransmitIcon->setIconSize(QSize(175, 175));
}

RightItemData* qGLChooseItem::GetItemData() {
    return m_RightItemData;
}