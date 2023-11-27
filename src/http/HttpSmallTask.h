#ifndef HTTPSMALLTASK_H
#define HTTPSMALLTASK_H

#include "SmallTask.h"

class HttpSmallTask : public SmallTask
{
public:
    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *userdata);
    static int progressFunc(void *userdata, double totalDownload, double nowDownload, double totalUpload, double nowUpload);
    virtual void run();
};

#endif // HTTPSMALLTASK_H