//
// ThreadWorker.h
//

#ifndef DEL_ThreadWorker_H
#define DEL_ThreadWorker_H

#include <QObject>
#include <QDebug>
#include <QThread>

class ThreadWorker : public QObject
{
    Q_OBJECT

public:
    explicit ThreadWorker(QObject* parent = nullptr);

public slots:
    void doWork(QString, int);  // doWork �������߳�Ҫִ�еĲ���

signals:
    void resultReady(const int result);  // �߳���ɹ���ʱ���͵��ź�
};

#endif //DEL_ThreadWorker_H
