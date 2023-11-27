#include "MmapFile.h"

#include "SmallTask.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

MmapFile::MmapFile() : m_fd(-1), m_mmap_ptr(nullptr) {}
MmapFile::~MmapFile()
{
    if (m_mmap_ptr)
        munmap(m_mmap_ptr, m_file_size);

    if (m_fd)
        close(m_fd);
}

DownloadCode MmapFile::openFile(std::string filepath, long filesize)
{
    m_fd = open(filepath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (m_fd == -1)
    {
        printf("[error] create %s file failed!\n", filepath.c_str());
        return DownloadCode::CREATE_FILE_FAILURE;
    }

    if (-1 == lseek(m_fd, filesize - 1, SEEK_SET))
    {
        perror("lseek");
        return DownloadCode::CREATE_FILE_FAILURE;
    }
    if (1 != write(m_fd, "", 1))
    {
        perror("write");
        return DownloadCode::CREATE_FILE_FAILURE;
    }

    m_file_size = filesize;
    m_waitingdownload_size = filesize;
    m_downloaded_size = 0;

    m_mmap_ptr = (char *)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if (m_mmap_ptr == MAP_FAILED)
    {
        perror("mmap");
        return DownloadCode::CREATE_FILE_FAILURE;
    }

    return DownloadCode::SUCCESS;
}

size_t MmapFile::writeFile(void *ptr, size_t size, size_t memb, void *userdata)
{
    SmallTask *t = (SmallTask *)userdata;
    auto fileptr = (MmapFile *)t->m_fileptr.get();
    memcpy(fileptr->m_mmap_ptr + t->start_pos, ptr, size * memb);

    t->start_pos += size * memb;

    calculateProgress(size * memb);

    return size * memb;
}