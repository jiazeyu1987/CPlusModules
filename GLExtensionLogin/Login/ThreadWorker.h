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
    void doWork(QString, int);  // doWork 定义了线程要执行的操作

signals:
    void resultReady(const int result);  // 线程完成工作时发送的信号
};

#endif //DEL_ThreadWorker_H
