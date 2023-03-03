/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef GL_PopWindows_H
#define GL_PopWindows_H

#include <QString>
#include "ui_GLPopWindows.h"
//#include "org_gl_comm_Export.h"
#include "qSlicerLoginModuleExport.h"
#include "qpainter.h"

class Q_SLICER_QTMODULES_LOGIN_EXPORT GLPopWindows : public QDialog
{
	Q_OBJECT

public:
	explicit GLPopWindows(QWidget *parent = 0);
	~GLPopWindows();

	static int ShowInformation(const QString & title, const QString & contentText, bool isModelWindow = true);
		
protected slots:
	void OnCloseClicked();


private:
	void paintEvent(QPaintEvent *event);
	void SetTitle(QString title);
	void SetContentText(QString mainMsg, QString addMsg = NULL);
private:
	Ui::GLPopWindows* ui;
	//MessageType messageType;
	
};

#endif /* define GL_PopWindows_H */
