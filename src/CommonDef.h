#ifndef COMMONDEF_H
#define COMMONDEF_H

enum DownloadCode
{
    SUCCESS = 0,          // 成功
    GET_FILESIZE_FAILURE, // 获取下载文件大小失败
    CREATE_FILE_FAILURE,  // 本地创建文件失败
    BACKUP_FILE_ABNORMAL  // 断点续传文件异常
};

// 默认的分片Size， 单位 MB
#define SEG_SHRESHOLD_SIZE 20

// 默认的线程池中线程数量
#define THREADPOOL_COUNT 10

#endif // COMMONDEF_H