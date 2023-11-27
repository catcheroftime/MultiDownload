#ifndef FTPTASK_H
#define FTPTASK_H

#include "DownloadTask.h"

class FtpTask : public DownloadTask
{
    virtual DownloadCode getDownloadFileLength(long &size)
    {
        return DownloadCode::SUCCESS;
    }

    virtual std::shared_ptr<SmallTask> getSmallTask()
    {
        return nullptr;
    }
};

#endif // FTPTASK_H