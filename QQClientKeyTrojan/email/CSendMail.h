#pragma once
#pragma once
/*
**CSendMailͷ�ļ�
**ʵ���ʼ��ķ��͹��ܣ�֧�ֶ���û����գ�֧�ָ���
**program by six_beauty
*/

#include <string>
#include <list>
#include <map>
#include "LogInfo.h"
#include <winsock2.h>
#include<winsock.h>
#pragma  comment(lib,"ws2_32.lib") 

//���Ͷ���
const int MAX_BUFFER_SIZE = 255;                            //send��recv�Ļ���buffer��size
const int SERVICE_PORT = 25;                                    //�˿�

typedef std::map<std::string, std::string> RECEIVERS;

//CSendMail��
class CSendMail
{
public:
	CSendMail();
	~CSendMail();

	//�����ʼ���Ϣ/
	// connent///
		void setServerName(const std::string server_name);        //smtp��������ַ    
	void setUserName(const std::string user_name);            //�����û���
	void setUserPwd(const std::string user_pwd);            //�����û�����
	// SendMail//
		void setSenderName(const std::string sender_name);        //�����ߵ�����
	void setSenderAddress(const std::string sender_addr);    //�����ߵ�����(mail form:)

	//�ʼ�������
	void setReceiver(const std::string name, const std::string address);            //��clear��add
	void addReceiver(const std::string name, const std::string address);            //�����ʼ������ߣ�name���ռ������֣�mail�ǵ�ַ
	void clearReceiver();                                                            //����ʼ�������

	//��Ӹ���
	void AddFilePath(std::string szFilePath);                                        //��Ӹ���·���������б��У�һ���smtp������������������50MB 
	void DeleteFilePath(std::string szFilePath);                                    //ɾ������·��������еĻ�  
	void DeleteAllPath();                                                            //ɾ��ȫ��������·��  

	// �����ʼ�//
		//����
		bool Connent();
	//�ʼ�����
	bool SendMail(const std::string mail_title, const std::string send_content);        //�����ʼ��ĺ���

private:
	//���ܺ���
	inline std::string& replace_all(string& str, const string& old_value, const string& new_value);       //��ʵ����CString��Replace
	std::string GetFileName(std::string& szFilePath);        //�Ӹ�����·���л�ȡ�ļ�����
	std::string GetFileData(std::string szFilePath);        //���ַ���ʽ���븽������

	std::string Base64Encode(std::string in_str);            //��char����ת����Base64����  
	//��ȡʱ��
	std::string prepareDate();

	//ͨ��recv��send�ķ�װ
	int     sendRequest(const std::string content, bool bout = false);                //���ط����˶����ֽ�
	bool rcvResponse(const std::string expected_response);    //���ؽ��յĽ����expected_response�Ƿ���ͬ

	//��������
	bool CReateSocket();                                    //����socket����  
	bool Logon();                                            //��¼���䣬��Ҫ���з��ʼ�ǰ��׼������  

	bool SendHead();                                        //�����ʼ�ͷ  
	bool SendTextBody();                                    //�����ʼ��ı�����  
	bool SendFileBody();                                    //�����ʼ�����  
	bool SendEnd();                                            //�����ʼ���β 


	SOCKET _socket;
	LogInfo m_logInfo;

	// /�ʼ���Ϣ///
		// connent///
		std::string                    m_ServerName;        //smtp��������ַ
	std::string                    m_UserName;            //�����û�
	std::string                    m_UserPwd;            //�����û���
	// SendMail//
		std::string                    m_SenderName;        //�����ߵ���    
	std::string                    m_SenderAddr;        //�����ߵ�����(mail form:)
	std::string                    m_MailTitle;        //�ʼ�����(subject)
	std::string                    m_TextBody;            //�ʼ�����

	RECEIVERS                    m_Receivers;        //�ʼ������ߣ�name,email_address)

	std::list<std::string>        m_FilePathList;        //����·��_list

	// �ʼ���Ϣ///
};

