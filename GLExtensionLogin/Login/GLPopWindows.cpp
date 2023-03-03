#include <qapplication.h>
#include <GLPopWindows.h>

GLPopWindows::GLPopWindows(QWidget *parent) :QDialog(parent), ui(new Ui::GLPopWindows())
{
	ui->setupUi(this);

	//���ð�͸������ by zbs
	this->setWindowFlags(Qt::FramelessWindowHint);          //���ش��ڱ�����
	this->setWindowModality(Qt::ApplicationModal);          //����������
	this->setAttribute(Qt::WA_TranslucentBackground, true); //����͸��
	this->setAttribute(Qt::WA_DeleteOnClose);               //�رռ�����
	connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(OnCloseClicked()));
}

void GLPopWindows::OnCloseClicked()
{
	QDialog::accept();
	//MessageResult::OK;
	close();
}

//�����ػ�
void GLPopWindows::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QColor(0, 0, 0, 180));  //QColor���һ��������������͸����
}
GLPopWindows::~GLPopWindows()
{
	delete ui;
}

void GLPopWindows::SetTitle(QString title)
{
	ui->lblTitle->setText(title);
}

void GLPopWindows::SetContentText(QString mainMsg, QString addMsg)
{
	ui->lblMainMsg->setText(mainMsg);
}

int GLPopWindows::ShowInformation(const QString & title, const QString & contentText, bool isModelWindow)
{
	GLPopWindows *msgBox = new GLPopWindows;
	msgBox->SetTitle(title);
	msgBox->SetContentText(contentText);
	if (isModelWindow)
		return msgBox->exec();
	else
		msgBox->show();
	return -1;
}