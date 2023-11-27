#ifndef HTTPTASK_H
#define HTTPTASK_H

#include "DownloadTask.h"
#include "curl/curl.h"

class HttpTask : public DownloadTask
{
public:
    virtual ~HttpTask(){};

protected:
    virtual DownloadCode getDownloadFileLength(long &size);
    virtual std::shared_ptr<SmallTask> getSmallTask();
};

#endif // HTTPTASK_H