#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include <string>
#include <map>
#include <memory>
#include "CommonDef.h"
#include "LocalFile.h"
#include "SmallTask.h"

class DownloadTask
{
public:
    DownloadTask();
    virtual ~DownloadTask();

    void setDownloadUrl(std::string url);
    void setMaxSegSize(long size);

    DownloadCode startDownload(int thread_count = 0);

    // 正常完成调用
    void finishDownload();

    // 停止或者异常退出调用
    void terminateDownload();

protected:
    virtual DownloadCode doPreSomethings();
    virtual DownloadCode AssignSegNewTask();
    virtual DownloadCode AssignSegBreakpointTask();

    virtual DownloadCode getDownloadFileLength(long &size) = 0;
    virtual std::shared_ptr<SmallTask> getSmallTask() = 0;

private:
    std::string getDownloadUrl() { return m_url; };
    std::string getSavePath() { return m_savepath; };
    long getMaxSegSize() { return MAX_SEG_SIZE; };
    std::string getBackupPath() { return m_savepath + ".backup"; };

    bool BreakpointContinue();

protected:
    std::string m_url;
    std::string m_savepath;

    /**
     * 文件大小
     */
    long m_filesize;

    /**
     * 分段最大阀值
     */
    long MAX_SEG_SIZE;

    /**
     * 分段下载标志
     */
    bool m_isseg;

    std::shared_ptr<LocalFile> m_fileptr;
    std::map<int, std::shared_ptr<SmallTask>> m_mapSmallTasks;
};

#endif // DOWNLOADTASK_H
