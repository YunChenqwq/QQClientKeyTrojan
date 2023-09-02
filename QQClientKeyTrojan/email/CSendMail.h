#pragma once
#pragma once
/*
**CSendMail头文件
**实现邮件的发送功能，支持多个用户接收，支持附件
**program by six_beauty
*/

#include <string>
#include <list>
#include <map>
#include "LogInfo.h"
#include <winsock2.h>
#include<winsock.h>
#pragma  comment(lib,"ws2_32.lib") 

//类型定义
const int MAX_BUFFER_SIZE = 255;                            //send和recv的缓存buffer的size
const int SERVICE_PORT = 25;                                    //端口

typedef std::map<std::string, std::string> RECEIVERS;

//CSendMail类
class CSendMail
{
public:
	CSendMail();
	~CSendMail();

	//设置邮件信息/
	// connent///
		void setServerName(const std::string server_name);        //smtp服务器地址    
	void setUserName(const std::string user_name);            //邮箱用户名
	void setUserPwd(const std::string user_pwd);            //邮箱用户密码
	// SendMail//
		void setSenderName(const std::string sender_name);        //发送者的名字
	void setSenderAddress(const std::string sender_addr);    //发送者的邮箱(mail form:)

	//邮件接收者
	void setReceiver(const std::string name, const std::string address);            //先clear再add
	void addReceiver(const std::string name, const std::string address);            //增加邮件接收者，name是收件人名字，mail是地址
	void clearReceiver();                                                            //情况邮件接收者

	//添加附件
	void AddFilePath(std::string szFilePath);                                        //添加附件路径到附件列表中，一般的smtp服务器处理附件不超过50MB 
	void DeleteFilePath(std::string szFilePath);                                    //删除附件路径，如果有的话  
	void DeleteAllPath();                                                            //删除全部附件的路径  

	// 发送邮件//
		//连接
		bool Connent();
	//邮件发送
	bool SendMail(const std::string mail_title, const std::string send_content);        //发送邮件的函数

private:
	//功能函数
	inline std::string& replace_all(string& str, const string& old_value, const string& new_value);       //其实就是CString的Replace
	std::string GetFileName(std::string& szFilePath);        //从附件的路径中获取文件名称
	std::string GetFileData(std::string szFilePath);        //以字符形式读入附件内容

	std::string Base64Encode(std::string in_str);            //把char类型转换成Base64类型  
	//获取时间
	std::string prepareDate();

	//通信recv和send的封装
	int     sendRequest(const std::string content, bool bout = false);                //返回发送了多少字节
	bool rcvResponse(const std::string expected_response);    //返回接收的结果和expected_response是否相同

	//工作函数
	bool CReateSocket();                                    //建立socket连接  
	bool Logon();                                            //登录邮箱，主要进行发邮件前的准备工作  

	bool SendHead();                                        //发送邮件头  
	bool SendTextBody();                                    //发送邮件文本正文  
	bool SendFileBody();                                    //发送邮件附件  
	bool SendEnd();                                            //发送邮件结尾 


	SOCKET _socket;
	LogInfo m_logInfo;

	// /邮件信息///
		// connent///
		std::string                    m_ServerName;        //smtp服务器地址
	std::string                    m_UserName;            //邮箱用户
	std::string                    m_UserPwd;            //邮箱用户密
	// SendMail//
		std::string                    m_SenderName;        //发送者的名    
	std::string                    m_SenderAddr;        //发送者的邮箱(mail form:)
	std::string                    m_MailTitle;        //邮件标题(subject)
	std::string                    m_TextBody;            //邮件正文

	RECEIVERS                    m_Receivers;        //邮件接收者（name,email_address)

	std::list<std::string>        m_FilePathList;        //附件路径_list

	// 邮件信息///
};

