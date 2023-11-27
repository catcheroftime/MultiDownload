#ifndef SMALLTASK_H
#define SMALLTASK_H

#include <string>
#include <memory>
#include "LocalFile.h"

class SmallTask
{
public:
    SmallTask(){};
    virtual ~SmallTask(){};

    virtual void run(){};

public:
    std::string m_url;
    /**
     * 是否分片下载
     */
    bool isseg;

    /**
     * 分段号
     */
    long seg_num;
    /**
     * 分段开始位置
     */
    long start_pos;
    /**
     * 分段结束位置
     */
    long end_pos;

    std::shared_ptr<LocalFile> m_fileptr;
};

#endif // SMALLTASK_H