#ifndef MMAPFILE_H
#define MMAPFILE_H

#include "LocalFile.h"

// 利用 mmap 内存映射，直接通过 memcpy 写入文件

class MmapFile : public LocalFile
{
public:
    MmapFile();
    virtual ~MmapFile();

    virtual DownloadCode openFile(std::string filepath, long filesize);
    virtual size_t writeFile(void *ptr, size_t size, size_t memb, void *userdata);

private:
    int m_fd;
    char *m_mmap_ptr;
};

#endif // MMAPFILE_H