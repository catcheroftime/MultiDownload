#ifndef DOWNLOADFACTORY_H
#define DOWNLOADFACTORY_H

#include "DownloadTask.h"

#include "ftp/FtpTask.h"
#include "http/HttpTask.h"

#include <memory>
class DownloadFactory
{
public:
    enum TaskType
    {
        HTTP_TASK,
        FTP_TASK
    };

    static DownloadFactory &getInstance()
    {
        static DownloadFactory one;
        return one;
    }

    std::shared_ptr<DownloadTask> getDownloadTask(const TaskType &type)
    {
        switch (type)
        {
        case HTTP_TASK:
            return std::shared_ptr<DownloadTask>(new HttpTask);
        case FTP_TASK:
            return std::shared_ptr<DownloadTask>(new FtpTask);
        default:
            return nullptr;
        }
    }

private:
    DownloadFactory() {}
    ~DownloadFactory() {}
};

#endif // DOWNLOADFACTORY_H
