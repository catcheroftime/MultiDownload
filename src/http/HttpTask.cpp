#include "HttpTask.h"

#include "HttpSmallTask.h"
#include "curl/curl.h"

DownloadCode HttpTask::getDownloadFileLength(long &size)
{
    printf("url: %s\n", m_url.data());

    double file_len = 0;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, m_url.data());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
    curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);

    if (curl_easy_perform(curl) == CURLE_OK)
    {
        printf("getDownloadFileLength success\n");
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &file_len);
        curl_easy_cleanup(curl);

        size = file_len;
        return DownloadCode::SUCCESS;
    }

    printf("getDownloadFileLength error\n");
    curl_easy_cleanup(curl);
    size = -1;
    return DownloadCode::GET_FILESIZE_FAILURE;
}

std::shared_ptr<SmallTask> HttpTask::getSmallTask()
{
    return std::shared_ptr<SmallTask>(new HttpSmallTask());
}
