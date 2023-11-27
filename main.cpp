#include "DownloadFactory.h"
#include "DownloadTask.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <signal.h>
#include <list>

static void show_usage(void)
{
    printf("description:\n"
           "-H http方式下载.\n"
           "-F ftp方式下载(可扩展，未实现).\n"
           "-s 分段下载阀值size(单位m,不设置时默认值为20),当下载文件大于阀值启用分段下载，否则不开启.\n"
           "-h help.\n"
           "e.g:\n"
           "MultiDownload -H 'download url' -s 20\n");
}

std::list<std::shared_ptr<DownloadTask>> m_gIngTask;

// 注册一个检测异常退出的函数, 模拟断点续传
void signal_handler(int signum)
{
    printf("signum: %d\n", signum);

    for (auto task : m_gIngTask)
    {
        task->terminateDownload();
    }
    exit(1);
}

int main(int argc, char **argv)
{
    int ch, type = -1;
    char download_url[200] = {0};
    long seg_shreshold_size = SEG_SHRESHOLD_SIZE;
    while ((ch = getopt(argc, argv, "H:F:s:h")) != -1)
    {
        switch (ch)
        {
        case 'H':
            strcpy(download_url, optarg);
            type = 1;
            break;
        case 'F':
            strcpy(download_url, optarg);
            show_usage();
            type = 2;
            break;
        case 's':
            seg_shreshold_size = atol(optarg);
            break;
        case '?':
        case 'h':
        case ':':
            show_usage();
            type = -1;
            break;
        default:
            printf("no option or error option");
            show_usage();
            type = -1;
            break;
        }
    }

    std::shared_ptr<DownloadTask> task;
    if (type == 1)
        task = DownloadFactory::getInstance().getDownloadTask(DownloadFactory::HTTP_TASK);
    else if (type == 2)
    {
        // task = DownloadFactory::getInstance().getDownloadTask(DownloadFactory::FTP_TASK);
    }
    else
    {
        printf("暂不支持\n");
        return 1;
    }

    if (task == nullptr)
    {
        printf("创建下载任务失败\n");
        return 1;
    }

    if (SIG_ERR == signal(SIGINT, signal_handler))
    {
        perror("signal");
        return -1;
    }

    task->setDownloadUrl(std::string(download_url));
    task->setMaxSegSize(seg_shreshold_size);

    m_gIngTask.push_back(task);

    if (DownloadCode::SUCCESS == task->startDownload())
        printf("下载成功\n");
    else
        printf("下载失败\n");

    task->finishDownload();
    m_gIngTask.remove(task);

    return 0;
}