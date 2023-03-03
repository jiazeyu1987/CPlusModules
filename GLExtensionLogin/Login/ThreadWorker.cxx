#include "ThreadWorker.h"
#include <PythonQt.h>
ThreadWorker::ThreadWorker(QObject* parent)
{

}

void ThreadWorker::doWork(QString path, int i)
{
    qDebug() << "receive the execute signal";
    qDebug() << "\tCurrent thread ID: " << QThread::currentThreadId();
    return
    PythonQt::init();
    PythonQtObjectPtr context = PythonQt::self()->getMainModule();
    context.evalScript(QString(
        "import slicer.util as util;util.loadVolume('%1');").arg(path));
    return;

    // ·¢ËÍ½áÊøĞÅºÅ
    qDebug() << "\tFinish the work and sent the result Ready signal\n";

    emit resultReady(i);
}