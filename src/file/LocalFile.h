#ifndef LOCALFILE_H
#define LOCALFILE_H

#include <string>
#include "CommonDef.h"

class locker
{
public:
    locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    bool trylock()
    {
        return pthread_mutex_trylock(&m_mutex) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

private:
    pthread_mutex_t m_mutex;
};

// 利用 fseek, fwrite 加锁的形式写入文件

class LocalFile
{
public:
    LocalFile();
    virtual ~LocalFile();

    void setWaitingDownloadSize(long filesize) { m_waitingdownload_size = filesize; };

    virtual DownloadCode openFile(std::string filepath, long filesize);
    virtual size_t writeFile(void *ptr, size_t size, size_t nmemb, void *userdata);

protected:
    void calculateProgress(size_t written);

protected:
    FILE *m_fp;

    /**
     * 分段下载写文件锁
     */
    locker m_seg_locker;

    /**
     * 本地文件大小
     */
    long m_file_size;

    /**
     * 需要下载的文件大小
     */
    long m_waitingdownload_size;

    /**
     * 已经下载的文件大小
     */
    long m_downloaded_size;

    int m_lastpercent;
};

#endif // LOCALFILE_H