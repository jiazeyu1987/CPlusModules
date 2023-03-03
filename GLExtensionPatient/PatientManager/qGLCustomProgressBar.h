#pragma once

#include <qdialog.h>
#include <qprogressbar.h>
#include "ui_qGLCustomProgressBar.h"

class GLCustomProgressBar : public QDialog
{
	Q_OBJECT

public:
	explicit GLCustomProgressBar(QWidget* parent = 0);
	~GLCustomProgressBar();

	void setText(QString s);
	void setMaximum(int value);
	void setValue(int value);
	void reset();

private:
	void paintEvent(QPaintEvent* event);

private:
	Ui::GLCustomDialog* ui;

	QProgressBar* bard;
};
