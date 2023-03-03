#include "qGLCustomProgressBar.h"
#include <qpainter.h>

GLCustomProgressBar::GLCustomProgressBar(QWidget* parent)
    :QDialog(parent), ui(new Ui::GLCustomDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);          //���ش��ڱ�����
    setWindowModality(Qt::ApplicationModal);          //����������
    setAttribute(Qt::WA_TranslucentBackground, true); //����͸��
    bard = new QProgressBar;
    ui->horizontalLayout->addWidget(bard);
    bard->setStyleSheet(QString::fromUtf8("background-color: white;"));
    bard->setMinimum(0);
}

GLCustomProgressBar::~GLCustomProgressBar()
{
    delete ui;
}

void GLCustomProgressBar::setText(QString s)
{
    ui->label->setText(s);
}

void GLCustomProgressBar::setMaximum(int value)
{
    bard->setMaximum(value);
}

void GLCustomProgressBar::setValue(int value)
{
    bard->setValue(value);
}

void GLCustomProgressBar::reset()
{
    bard->reset();
}

void GLCustomProgressBar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0, 0, 0, 220));
}
