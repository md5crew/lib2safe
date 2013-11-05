#include "safeapi.h"

SafeApi::SafeApi(QString host) :
    ticker(1000),
    maxThreads(MAX_THREADS),
    maxFileThreads(MAX_FILE_THREADS)
{
    this->host = host;
}

void SafeApi::freeWorker(ulong worker_id)
{
    mutex.lock();
    SafeWorker *worker = workersPool.take(worker_id);
    delete worker;
    mutex.unlock();
}

void SafeApi::clearState()
{
    this->lastToken.clear();
    this->lastUserId.clear();
    this->lastRootDir.clear();
    this->lastLogin.clear();
}

bool SafeApi::reportError(ulong id, const QJsonDocument& response)
{
    QJsonValue success = response.object().value("success");
    if(success.toString() == FALSE) {
        int code = response.object().value("error_code").toString().toInt();
        QString text = response.object().value("error_msg").toString();
        emit errorRaised(id, code, text);
        return true;
    } else {
        // no error actually
        return false;
    }
}

ulong SafeApi::getId()
{
    mutex.lock();
    ++ticker;
    mutex.unlock();
    return ticker;
}

void SafeApi::networkError(const QString& text)
{
    qDebug() << "[ERROR] network error:\n" << text;
}

SafeWorker *SafeApi::createWorker(QString cmd)
{
    SafeWorker *worker = new SafeWorker(this->host);
    connect(worker, &SafeWorker::error, [=](const QString& text){
        freeWorker(worker->getId());
        networkError(text);
    });

    worker->setCmd(cmd);
    return worker;
}

void SafeApi::routeWorker(SafeWorker *worker)
{
    mutex.lock();
    if(workersPool.count() < maxThreads) {
        workersPool.insert(worker->getId(), worker);
        worker->run();
    } else {
        workersQueue.enqueue(worker);
    }
    mutex.unlock();
}

void SafeApi::processWorkerQueue()
{
    mutex.lock();
    int free = maxThreads - workersPool.count();
    int avail = workersQueue.count();
    int min = (free < avail ? free : avail);

    SafeWorker* workers[min];
    for(int i = 0; i < min; ++i) {
        SafeWorker *worker = workersQueue.dequeue();
        workersPool.insert(worker->getId(), worker);
        workers[i] = worker;
    }
    mutex.unlock();

    for(int i = 0; i < min; ++i) {
        workers[i]->run();
    }
}
