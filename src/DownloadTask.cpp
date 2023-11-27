#include "DownloadTask.h"
#include "Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SmallTask.h"
#include "HttpSmallTask.h"
#include "LocalFile.h"
#include "MmapFile.h"
#include "ThreadPool.h"

DownloadTask::DownloadTask()
    : m_url(""),
      m_savepath(""),
      m_filesize(-1),
      m_isseg(false)
{
}

DownloadTask::~DownloadTask()
{
    m_mapSmallTasks.clear();
}

void DownloadTask::setDownloadUrl(std::string url)
{
    m_url = Utils::trim(url);

    const char *file_name = basename((char *)m_url.c_str());
    m_savepath = file_name;
    m_savepath = "./" + m_savepath;
}
void DownloadTask::setMaxSegSize(long size)
{
    MAX_SEG_SIZE = size * 1024 * 1024;
}

DownloadCode DownloadTask::startDownload(int thread_count)
{
    // 子类可能的一些预处理
    DownloadCode code = doPreSomethings();
    if (code != DownloadCode::SUCCESS)
        return code;

    // 获取下载文件大小
    code = getDownloadFileLength(m_filesize);
    if (code != DownloadCode::SUCCESS || m_filesize == -1)
        return code;

    printf("getDownloadFileLength : %d \n", m_filesize);

    // 判断是否需要预处理
    bool bpcontinue = BreakpointContinue();

    // 打开文件句柄
    m_fileptr.reset();
    // m_fileptr = std::make_shared<LocalFile>();
    m_fileptr = std::make_shared<MmapFile>();
    code = m_fileptr->openFile(m_savepath, m_filesize);
    if (code != DownloadCode::SUCCESS)
        return code;

    // 区分是断点续传还是新下载
    if (bpcontinue)
    {
        printf("开始断点续传...\n");
        code = AssignSegBreakpointTask();
    }
    else
    {
        printf("开始下载...\n");
        code = AssignSegNewTask();
    }
    if (code != DownloadCode::SUCCESS)
        return code;

    // 多线程开始下载
    zl::ThreadPool tp{THREADPOOL_COUNT};
    std::vector<std::future<void>> results;
    for (auto small_task : m_mapSmallTasks)
    {
        auto ans = tp.add(std::bind(&SmallTask::run, small_task.second.get()));
        results.push_back(std::move(ans));
    }

    for (size_t i = 0; i < results.size(); ++i)
        results[i].get();

    return DownloadCode::SUCCESS;
}

// 正常完成调用
void DownloadTask::finishDownload()
{
    m_mapSmallTasks.clear();
    // 删除可能存在的断点续传文件
    std::string backup_path = getBackupPath();
    if (access(backup_path.data(), F_OK) == 0)
        remove(backup_path.data());
}

void DownloadTask::terminateDownload()
{
    // 删除旧的断点续传文件并生成新的
    std::string backup_path = getBackupPath();
    if (access(backup_path.data(), F_OK) == 0)
    {
        // 文件存在
        printf("历史断点续传文件存在，正在删除...\n");
        if (remove(backup_path.data()) == 0)
            printf("历史断点续传文件删除成功！\n");
        else
            printf("历史断点续传文件删除失败！\n");
    }

    int fd = open(backup_path.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return;

    for (auto small_task : m_mapSmallTasks)
    {
        char range[64] = {0};
        snprintf(range, 64, "%d-%d\r\n", small_task.second->start_pos, small_task.second->end_pos);
        write(fd, range, strlen(range));
    }
    close(fd);
}

DownloadCode DownloadTask::doPreSomethings()
{
    return DownloadCode::SUCCESS;
}

DownloadCode DownloadTask::AssignSegNewTask()
{
    int additional = (m_filesize % MAX_SEG_SIZE == 0) ? 0 : 1;
    int seg_total = (m_filesize < MAX_SEG_SIZE) ? 1 : (m_filesize / MAX_SEG_SIZE + additional);
    if (seg_total > 1)
        m_isseg = true;

    if (!m_isseg)
    {
        std::shared_ptr<SmallTask> task = getSmallTask();
        task->m_url = m_url;
        task->isseg = m_isseg;
        task->seg_num = 1;
        task->start_pos = 0;
        task->end_pos = m_filesize - 1;
        task->m_fileptr = m_fileptr;
        m_mapSmallTasks.insert({1, task});
    }
    else
    {
        long seg_num = m_filesize / MAX_SEG_SIZE;
        for (int i = 0; i <= seg_num; i++)
        {
            std::shared_ptr<SmallTask> task = getSmallTask();
            if (i < seg_num)
            {
                task->start_pos = i * MAX_SEG_SIZE;
                task->end_pos = (i + 1) * MAX_SEG_SIZE - 1;
            }
            else
            {
                if (m_filesize % MAX_SEG_SIZE != 0)
                {
                    task->start_pos = i * MAX_SEG_SIZE;
                    task->end_pos = m_filesize - 1;
                }
                else
                    break;
            }
            task->m_url = m_url;
            task->isseg = m_isseg;
            task->seg_num = i + 1;
            task->m_fileptr = m_fileptr;
            m_mapSmallTasks.insert({i + 1, task});
        }
    }

    return DownloadCode::SUCCESS;
}

DownloadCode DownloadTask::AssignSegBreakpointTask()
{
    FILE *fp = fopen(getBackupPath().data(), "r");
    if (!fp)
        return BACKUP_FILE_ABNORMAL;

    // 打开文件,判断上次是否是分片下载
    int waitingdownload_size = 0;
    int start_pos, end_pos = 0;
    int index = 1;
    while (fscanf(fp, "%d-%d", &start_pos, &end_pos) == 2)
    {
        if (start_pos >= end_pos)
            continue;

        waitingdownload_size += end_pos - start_pos;

        std::shared_ptr<SmallTask> task = getSmallTask();
        task->m_url = m_url;
        task->isseg = true;
        task->seg_num = index;
        task->start_pos = start_pos;
        task->end_pos = end_pos;
        task->m_fileptr = m_fileptr;
        m_mapSmallTasks.insert({index, task});
        index++;
    }
    m_fileptr->setWaitingDownloadSize(waitingdownload_size);

    fclose(fp);

    return DownloadCode::SUCCESS;
}

bool DownloadTask::BreakpointContinue()
{
    std::string backup_path = getBackupPath();
    bool backup_exist = (access(backup_path.data(), F_OK) == 0);
    bool file_exist = (access(m_savepath.data(), F_OK) == 0);

    if (backup_exist && file_exist)
        return true;
    else
    {
        if (backup_exist)
        {
            if (remove(backup_path.data()) == 0)
                printf("历史断点续传文件删除成功！\n");
            else
                printf("历史断点续传文件删除失败！\n");
        }

        if (file_exist)
        {
            if (remove(m_savepath.data()) == 0)
                printf("历史下载文件删除成功！\n");
            else
                printf("历史下载文件删除失败！\n");
        }
    }
}