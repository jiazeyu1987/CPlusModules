#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include "SwitchControl.h"

//��־
//#include <mbilog.h>
//#include <mitkLogMacros.h>
//#include <mitkLog.h>

SwitchControl::SwitchControl(QWidget *parent)
	: QWidget(parent),
	m_nHeight(16),
	m_bChecked(false),
	m_radius(12.0),
	m_nMargin(3),
	m_checkedColor(0, 150, 136),
	m_thumbColor(Qt::white),
	m_disabledColor(190, 190, 190),
	m_background(Qt::black)
{
	// ��껬�������״ - ����
	setCursor(Qt::PointingHandCursor);

	// �����źŲ�
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	m_nX = m_nHeight / 2;
	m_nY = m_nHeight / 2;
}

SwitchControl::~SwitchControl()
{
}

// ���ƿ���
void SwitchControl::paintEvent(QPaintEvent *event)
{
	//MITK_INFO << "���ؿ���-���� " << __FUNCTION__;
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setRenderHint(QPainter::Antialiasing);

	QPainterPath path;
	QColor background;
	QColor thumbColor;
	qreal dOpacity;
	if (isEnabled()) { // ����״̬
		if (m_bChecked) { // ��״̬
			background = m_checkedColor;
			thumbColor = m_checkedColor;
			dOpacity = 0.600;
		}
		else { //�ر�״̬
			background = m_background;
			thumbColor = m_thumbColor;
			dOpacity = 0.800;
		}
	}
	else {  // ������״̬
		background = m_background;
		dOpacity = 0.260;
		thumbColor = m_disabledColor;
	}
	// ���ƴ���Բ
	painter.setBrush(background);
	painter.setOpacity(dOpacity);
	//path.addRoundedRect(QRectF(m_nMargin, m_nMargin, width() - 2 * m_nMargin, height() - 2 * m_nMargin), m_radius, m_radius);
	path.addRoundedRect(QRectF(0, 0, width(), height()), m_radius, m_radius);
	painter.drawPath(path.simplified());

	// ����С��Բ�����أ�
	painter.setBrush(thumbColor);
	painter.setOpacity(1.0);
	if (m_nX > width()) {
		m_nX = m_nX - m_nHeight;
	}
	painter.drawEllipse(QRectF(m_nX - (m_nHeight / 2) + 1, m_nY - (m_nHeight / 2), height(), height()));
	//painter.drawEllipse(QRectF(m_nX - (m_nHeight / 2) + 1, m_nY - (m_nHeight / 2), 24, 24));
}

// ��갴���¼�
void SwitchControl::mousePressEvent(QMouseEvent *event)
{
	if (isEnabled()) {
		if (event->buttons() & Qt::LeftButton) {
			event->accept();
		}
		else {
			event->ignore();
		}
	}
}

// ����ͷ��¼� - �л�����״̬������toggled()�ź�
void SwitchControl::mouseReleaseEvent(QMouseEvent *event)
{
	if (isEnabled()) {
		if ((event->type() == QMouseEvent::MouseButtonRelease) && (event->button() == Qt::LeftButton)) {
			event->accept();
			m_bChecked = !m_bChecked;
			emit toggled(m_bChecked);
			m_timer.start(10);
		}
		else {
			event->ignore();
		}
	}
}

// ��С�ı��¼�
void SwitchControl::resizeEvent(QResizeEvent *event)
{
	
	QWidget::resizeEvent(event);
}

// Ĭ�ϴ�С
QSize SwitchControl::sizeHint() const
{
	return minimumSizeHint();
}

// ��С��С
QSize SwitchControl::minimumSizeHint() const
{
	return QSize(2 * (m_nHeight + m_nMargin), m_nHeight + 2 * m_nMargin);
}

// �л�״̬ - ����
void SwitchControl::onTimeout()
{
	if (m_bChecked) {
		if (m_nX >= width() - m_nHeight) {
			m_timer.stop();
			m_nX = width() - m_nHeight;
			return;
		}
			
		m_nX += 1;
		
	}
	else {
		if (m_nX <= m_nHeight / 2)
		{
			m_timer.stop();
			m_nX = m_nHeight / 2;
			return;
		}
		m_nX -= 1;
		
	} 
	update();
}

// ���ؿ���״̬ - �򿪣�true �رգ�false
bool SwitchControl::isToggled() const
{
	return m_bChecked;
}

// ���ÿ���״̬
void SwitchControl::setToggle(bool checked)
{
	m_bChecked = checked;
	if (m_bChecked) {
		m_nX = width() - m_nHeight;
		update();
	}
	else {
		m_nX = m_nHeight / 2;
		update();
	}
}

// ���ñ�����ɫ
void SwitchControl::setBackgroundColor(QColor color)
{
	m_background = color;
}

// ����ѡ����ɫ
void SwitchControl::setCheckedColor(QColor color)
{
	m_checkedColor = color;
}

// ���ò�������ɫ
void SwitchControl::setDisbaledColor(QColor color)
{
	m_disabledColor = color;
}
