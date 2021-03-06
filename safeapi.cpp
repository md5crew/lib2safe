#include "safeapi.h"

SafeApi::SafeApi(QString host, QObject *parent) :
    QObject(parent),
    ticker(1000),
    host(host),
    maxThreads(MAX_THREADS),
    maxFileThreads(MAX_FILE_THREADS)
{ }

SafeApi::~SafeApi()
{
    foreach (auto worker, fileWorkersPool) {
        worker->disconnect();
        worker->deleteLater();
    }
    this->disconnect();
    this->deleteLater();
}

void SafeApi::freeWorker(ulong worker_id)
{
    mutex.lock();
    if(workersPool.contains(worker_id))
        workersPool.take(worker_id)->deleteLater();
    mutex.unlock();
}

void SafeApi::freeFileWorker(ulong worker_id)
{
    mutex.lock();
    if(fileWorkersPool.contains(worker_id))
        fileWorkersPool.take(worker_id)->deleteLater();
    mutex.unlock();
}

void SafeApi::clearState()
{
    this->apiState.clear();
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
    qWarning() << "[ERROR] network error:\n" << text;
}

SafeWorker *SafeApi::createWorker(QString cmd)
{
    SafeWorker *worker = new SafeWorker(this->host);
    connect(worker, &SafeWorker::error, [=](const QString& text){
        networkError(text);
        freeWorker(worker->getId());
    });

    worker->setCmd(cmd);
    return worker;
}

SafeWorker *SafeApi::createFileWorker(QString cmd)
{
    SafeWorker *worker = new SafeWorker(this->host);
    connect(worker, &SafeWorker::error, [=](const QString& text){
        networkError(text);
        freeFileWorker(worker->getId());
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

void SafeApi::routeFileWorker(SafeWorker *worker)
{
    mutex.lock();
    if(fileWorkersPool.count() < maxFileThreads) {
        fileWorkersPool.insert(worker->getId(), worker);
        if(worker->getCmd() == CALL_PUSH_FILE) {
            worker->pushFile();
        } else {
            worker->pullFile();
        }
    } else {
        fileWorkersQueue.enqueue(worker);
    }
    mutex.unlock();
}

void SafeApi::processWorkersQueue()
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

void SafeApi::processFileWorkersQueue()
{
    mutex.lock();
    int free = maxFileThreads - fileWorkersPool.count();
    int avail = fileWorkersQueue.count();
    int min = (free < avail ? free : avail);

    SafeWorker* workers[min];
    for(int i = 0; i < min; ++i) {
        SafeWorker *worker = fileWorkersQueue.dequeue();
        fileWorkersPool.insert(worker->getId(), worker);
        workers[i] = worker;
    }
    mutex.unlock();

    for(int i = 0; i < min; ++i) {
        if(workers[i]->getCmd() == CALL_PUSH_FILE) {
            workers[i]->pushFile();
        } else {
            workers[i]->pullFile();
        }
    }
}
