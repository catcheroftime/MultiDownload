#include "HttpSmallTask.h"

#include <curl/curl.h>

size_t HttpSmallTask::write_data(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HttpSmallTask *task = (HttpSmallTask *)userdata;
    size_t save_size = task->m_fileptr->writeFile(ptr, size, nmemb, userdata);
    return save_size;
}

int HttpSmallTask::progressFunc(void *userdata, double totalDownload, double nowDownload, double totalUpload, double nowUpload)
{
    return 0;
}

void HttpSmallTask::run()
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)this);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
    curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.143 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);

    if (isseg)
    {
        char range[64] = {0};
        snprintf(range, 64, "%d-%d", start_pos, end_pos);
        printf("seg_num: %ld, download from: %d to: %d\n", seg_num, start_pos, end_pos);
        curl_easy_setopt(curl, CURLOPT_RANGE, range);
    }

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
        printf("[error] curl error:%d location:%ld segment url:%s\n", res, seg_num, m_url.c_str());

    curl_easy_cleanup(curl);
}