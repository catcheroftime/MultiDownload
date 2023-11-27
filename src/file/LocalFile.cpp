#include "LocalFile.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "SmallTask.h"

LocalFile::LocalFile() : m_fp(nullptr), m_file_size(0), m_downloaded_size(0), m_lastpercent(0) {}

LocalFile::~LocalFile()
{
    if (m_fp)
        fclose(m_fp);
}

DownloadCode LocalFile::openFile(std::string filepath, long filesize)
{
    m_fp = fopen(filepath.c_str(), "wb");
    if (NULL == m_fp)
    {
        printf("[error] create %s file failed!\n", filepath.c_str());
        return DownloadCode::CREATE_FILE_FAILURE;
    }

    fseek(m_fp, 0, SEEK_SET);

    m_file_size = filesize;
    m_downloaded_size = 0;
    m_waitingdownload_size = filesize;
    return DownloadCode::SUCCESS;
}

size_t LocalFile::writeFile(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    SmallTask *t = (SmallTask *)userdata;
    size_t written;

    if (t->isseg)
    {
        this->m_seg_locker.lock();
        fseek(t->m_fileptr->m_fp, t->start_pos, SEEK_SET);

        if (t->start_pos + size * nmemb <= t->end_pos)
        {
            written = fwrite(ptr, size, nmemb, t->m_fileptr->m_fp);
            t->start_pos += written;
        }
        else
        {
            written = fwrite(ptr, 1, t->end_pos - t->start_pos + 1, t->m_fileptr->m_fp);
            t->start_pos = t->end_pos;
        }
        this->m_seg_locker.unlock();
    }
    else
    {
        written = fwrite(ptr, size, nmemb, t->m_fileptr->m_fp);
        t->start_pos += written;
    }

    calculateProgress(written);

    return written;
}

void LocalFile::calculateProgress(size_t written)
{
    m_downloaded_size += written;

    int percent = (int)(m_downloaded_size * 100 / m_waitingdownload_size);
    if (m_lastpercent != percent)
    {
        m_lastpercent = percent;
        printf("percent: %d%%\n", percent);
    }
}