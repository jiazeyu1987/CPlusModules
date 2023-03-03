#include "qGLChooseDialog.h"

#include "qmessagebox.h"

#include "ui_qGLChooseDialog.h"

#include "qGLMessageDialog.h"

class qGLChooseDialogPrivate : public Ui_qGLChooseDialog
{
    Q_DECLARE_PUBLIC(qGLChooseDialog);
protected:
    qGLChooseDialog* const q_ptr;

public:
    qGLChooseDialogPrivate(qGLChooseDialog& object);
    
    bool CanCommitInfo();

    void Init();    
    void SetTransmitVisible(bool isVisible);

    bool m_IsOpenTransmit;
    QString m_NameRegistration;
    QString m_NameTransmit;
};

// --------------------------------------------------------------------------
qGLChooseDialogPrivate::qGLChooseDialogPrivate(qGLChooseDialog& object) : q_ptr(&object)
{
}

bool qGLChooseDialogPrivate::CanCommitInfo()
{
    m_NameRegistration = lblRegistName->text().simplified();
    if (m_NameRegistration.isEmpty()) {
        qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("配准项名字不能为空！"),true);
        return false;
    }
    if (m_IsOpenTransmit) {
        m_NameTransmit = lblTransmitName->text().simplified();
        if (m_NameTransmit.isEmpty()) {
            qGLMessageDialog::ShowWarnning(QStringLiteral("警告"), QStringLiteral("传导项名字为空！"), true);
            return false;
        }
    }
    else {
        m_NameTransmit = "";
    }
    return true;
}

// --------------------------------------------------------------------------
void qGLChooseDialogPrivate::Init()
{
    Q_Q(qGLChooseDialog);
    this->setupUi(q);

    SetTransmitVisible(false);
    QObject::connect(btnCancel, &QPushButton::clicked, q, &qGLChooseDialog::OnCancelClick);
    QObject::connect(btnConfirm, &QPushButton::clicked, q, &qGLChooseDialog::OnConfirmClick);
    QObject::connect(chbTransmit, &QCheckBox::toggled, q, &qGLChooseDialog::OnTransmitOpen);

    q->setWindowFlags(Qt::FramelessWindowHint);
}

void qGLChooseDialogPrivate::SetTransmitVisible(bool isVisible)
{
    m_IsOpenTransmit = isVisible;
    lblTransmit->setVisible(isVisible);
    lblTransmitName->setVisible(isVisible);
}

//-----------------------------------------------------------------------------
// qGLChooseDialog methods
//-----------------------------------------------------------------------------
qGLChooseDialog::qGLChooseDialog(QDialog* parent)
    : Superclass(parent) , d_ptr(new qGLChooseDialogPrivate(*this)) {
    Q_D(qGLChooseDialog);
    d->Init();

    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

qGLChooseDialog::~qGLChooseDialog()
{
}

QString qGLChooseDialog::GetRegistrationName()
{
    Q_D(qGLChooseDialog);
    return d->m_NameRegistration;
}

QString qGLChooseDialog::GetTransmitName()
{
    Q_D(qGLChooseDialog);
    return d->m_NameTransmit;
}

void qGLChooseDialog::setchnTransmitState(bool visible)
{
	Q_D(qGLChooseDialog);
	d->chbTransmit->setChecked(visible);
}
void qGLChooseDialog::setRegistrationName(QString NameRegistration)
{
	Q_D(qGLChooseDialog);
	d->lblRegistName->setText(NameRegistration);
}
void qGLChooseDialog::setTransmitName(QString NameTransmit)
{
    Q_D(qGLChooseDialog);
	d->lblTransmitName->setText(NameTransmit);
}


void qGLChooseDialog::OnCancelClick()
{
   reject();
   close();

}

void qGLChooseDialog::OnConfirmClick()
{
    Q_D(qGLChooseDialog);

    if (d->CanCommitInfo() == false)
    {
        return;
    }

   accept();
   close();
}

void qGLChooseDialog::OnTransmitOpen(bool isVisible)
{
    Q_D(qGLChooseDialog);
    
    d->SetTransmitVisible(isVisible);
}
