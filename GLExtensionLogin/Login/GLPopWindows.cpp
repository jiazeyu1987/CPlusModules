#include <qapplication.h>
#include <GLPopWindows.h>

GLPopWindows::GLPopWindows(QWidget *parent) :QDialog(parent), ui(new Ui::GLPopWindows())
{
	ui->setupUi(this);

	//设置半透明背景 by zbs
	this->setWindowFlags(Qt::FramelessWindowHint);          //隐藏窗口标题栏
	this->setWindowModality(Qt::ApplicationModal);          //阻滞主窗口
	this->setAttribute(Qt::WA_TranslucentBackground, true); //设置透明
	this->setAttribute(Qt::WA_DeleteOnClose);               //关闭即销毁
	connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(OnCloseClicked()));
}

void GLPopWindows::OnCloseClicked()
{
	QDialog::accept();
	//MessageResult::OK;
	close();
}

//背景重绘
void GLPopWindows::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QColor(0, 0, 0, 180));  //QColor最后一个参数代表背景的透明度
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