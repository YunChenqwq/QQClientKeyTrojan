/*
**CSendMailԴ�ļ�
**ʵ���ʼ��ķ��͹��ܣ�֧�ֶ���û����գ�֧�ָ���
**program by six_beauty
*/

//#include <afx.h>
#include "CSendMail.h"
#include "time.h"
#include <sstream>
#include <fstream>
#pragma comment(lib,"WSOCK32")  
#pragma comment(lib, "ws2_32")


const std::string _AppOctStrmContent_encode_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//���ʵ��

CSendMail::CSendMail(void)
{
}


CSendMail::~CSendMail(void)
{
	clearReceiver();
	DeleteAllPath();
}

//����
bool CSendMail::Connent()
{
	//�ʼ���Ϣ�����ж�
	if (m_ServerName.empty() || m_UserName.empty() || m_UserPwd.empty())
	{
		m_logInfo.logInfo("Connect ʧ�ܣ����������ʼ���½��Ϣ��");
		return false;
	}

	if (!CReateSocket())//��������  
	{
		m_logInfo.logInfo("��������ʧ�ܣ�");
		return false;
	}

	if (!Logon())//��������  
	{
		m_logInfo.logInfo("��½ʧ�ܣ�");
		return false;
	}
	return true;
}


//�����ʼ��ĺ�����
bool CSendMail::SendMail(const std::string mail_title, const std::string send_content)
{
	//������ֵ
	m_MailTitle = mail_title;
	m_TextBody = send_content;

	if (m_SenderName.empty() || m_SenderAddr.empty() || m_Receivers.empty())
	{
		m_logInfo.logInfo("[SendMail]�ʼ��������ô��������ʼ�����������Ϣ�Ƿ�������");
		return false;
	}
	if (!SendHead())//�����ʼ�ͷ  
	{
		m_logInfo.logInfo("�����ʼ�ͷʧ�ܣ�");
		return false;
	}

	if (!SendTextBody())//�����ʼ��ı�����  
	{
		return false;
	}

	if (!SendFileBody())//���͸���  
	{
		return false;
	}

	if (!SendEnd())//�����ʼ������ر�sock  
	{
		return false;
	}

	return true;
}


//�����ʼ���Ϣ /
void CSendMail::setServerName(const std::string server_name)        //smtp��������ַ    
{
	m_ServerName = server_name;
}

void CSendMail::setUserName(const std::string user_name)            //�����û���
{
	m_UserName = user_name;
}

void CSendMail::setUserPwd(const std::string user_pwd)                //�����û�����
{
	m_UserPwd = user_pwd;
}

void CSendMail::setSenderName(const std::string sender_name)        //�����ߵ�����
{
	m_SenderName = sender_name;
}

void CSendMail::setSenderAddress(const std::string sender_addr)    //�����ߵ�����(mail form:)
{
	m_SenderAddr = sender_addr;
}


void CSendMail::addReceiver(const std::string name, const std::string address)
{
	m_Receivers.insert(RECEIVERS::value_type(name, address));
}

void CSendMail::setReceiver(const std::string name, const std::string address)
{
	m_Receivers.clear();
	m_Receivers.insert(RECEIVERS::value_type(name, address));
}

void CSendMail::clearReceiver()
{
	m_Receivers.clear();
}

void CSendMail::AddFilePath(std::string szFilePath)//��Ӹ���·��  
{
	for (std::list<std::string>::iterator itrList = m_FilePathList.begin(); itrList != m_FilePathList.end(); ++itrList)
	{
		if (itrList->compare(szFilePath) == 0)
		{
			//�Ѿ�����
			return;
		}
	}
	//��δ����
	m_FilePathList.push_back(szFilePath);
}

void CSendMail::DeleteFilePath(std::string szFilePath)//ɾ������·��  
{
	for (std::list<std::string>::iterator itrList = m_FilePathList.begin(); itrList != m_FilePathList.end();)
	{
		if (itrList->compare(szFilePath) == 0)
		{
			itrList = m_FilePathList.erase(itrList);
		}
		else
		{
			itrList++;
		}
	}
}

void CSendMail::DeleteAllPath(void)
{
	m_FilePathList.clear();
}


//���ܺ���///

//ʵ��CString��Replace
string& CSendMail::replace_all(string& str, const string& old_value, const string& new_value)
{
	while (true)
	{
		string::size_type pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

//�Ӹ�����·���л�ȡ�ļ�����
std::string CSendMail::GetFileName(std::string& szFilePath)
{
	replace_all(szFilePath, "/", "\\");
	string szFileName = szFilePath.substr(szFilePath.rfind("\\") + 1, szFilePath.length());
	return szFileName;
}

//���ַ���ʽ���븽������
std::string CSendMail::GetFileData(std::string szFilePath)
{
	std::string szBuffer;
	if (szFilePath.empty())
	{
		m_logInfo.logInfo("[SendFileBody]Error:����·��Ϊ�գ�");
		return szBuffer;
	}

	ifstream ifFile(szFilePath.c_str(), ios::binary | ios::in);
	if (!ifFile)
	{
		m_logInfo.logInfo("[SendFileBody]Error:�򿪸���·������");
		return szBuffer;
	}
	ifFile.seekg(0, ios::beg);
	std::ostringstream tmp;
	tmp << ifFile.rdbuf();
	szBuffer = tmp.str();
	ifFile.close();

	return szBuffer;
}

//��char����ת����Base64���� 
std::string CSendMail::Base64Encode(std::string in_str)
{
	std::string out_str;
	unsigned char c1, c2, c3;
	int i = 0;
	int len = in_str.length();

	while (i < len)
	{
		// read the first byte
		c1 = in_str[i++];
		if (i == len)       // pad with "="
		{
			out_str += _AppOctStrmContent_encode_chars[c1 >> 2];
			out_str += _AppOctStrmContent_encode_chars[(c1 & 0x3) << 4];
			out_str += "==";
			break;
		}

		// read the second byte
		c2 = in_str[i++];
		if (i == len)       // pad with "="
		{
			out_str += _AppOctStrmContent_encode_chars[c1 >> 2];
			out_str += _AppOctStrmContent_encode_chars[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
			out_str += _AppOctStrmContent_encode_chars[(c2 & 0xF) << 2];
			out_str += "=";
			break;
		}

		// read the third byte
		c3 = in_str[i++];
		// convert into four bytes string
		out_str += _AppOctStrmContent_encode_chars[c1 >> 2];
		out_str += _AppOctStrmContent_encode_chars[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
		out_str += _AppOctStrmContent_encode_chars[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
		out_str += _AppOctStrmContent_encode_chars[c3 & 0x3F];
	}

	return out_str;
}

int CSendMail::sendRequest(const std::string content, bool bout)
{
	int len_s = send(_socket, content.c_str(), content.length(), 0);
	if (len_s < 0)
	{
		m_logInfo.logInfo("[ERROR]SEND:%s", content.c_str());
		return false;
	}
	//�����Ϣ
	if (bout)
	{
		m_logInfo.logInfo("[INFO]SEND:%s", content.c_str());
	}
	return len_s;
}

bool CSendMail::rcvResponse(const std::string expected_response)
{
	int recv_bytes = 0;
	char response_buffer[MAX_BUFFER_SIZE];
	if ((recv_bytes = recv(_socket, response_buffer, MAX_BUFFER_SIZE, 0)) < 0)
	{
		m_logInfo.logInfo("[ERROR]RECV:%s", expected_response.c_str());
		return false;
	}
	//�����Ϣ
	std::string response(response_buffer, recv_bytes);
	m_logInfo.logInfo("[INFO]RECV(%s):%s", expected_response.c_str(), response.c_str());
	if (response.substr(0, 3) != expected_response)
	{
		//-------------------------------here
		//return false;
	}
	return true;
}

std::string CSendMail::prepareDate()
{
	char date_string[MAX_BUFFER_SIZE];

	time_t seconds;

	time(&seconds);
	//strftime(date_string, MAX_BUFFER_SIZE,
	//	"%a, %d %b %y %H:%M:%S +0800",
	//	localtime(&seconds));          // +0800 maybe hard code
	tm tms;
	localtime_s(&tms, &seconds);
	strftime(date_string, MAX_BUFFER_SIZE, "%a, %d %b %y %H:%M:%S +0800", &tms);
	return date_string;
}

//��������//

bool CSendMail::CReateSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		m_logInfo.logInfo("WSAStartup����ʧ��!");
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (_socket == INVALID_SOCKET)
	{
		m_logInfo.logInfo("socket����ʧ��!");
		return false;
	}

	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVICE_PORT);//���ʼ�һ�㶼��25�˿�  

	struct hostent* hp = gethostbyname(m_ServerName.c_str());//ʹ������  
	//GetAddrInfoW

	if (hp == NULL)
	{
		DWORD dwErrCode = GetLastError();
		return false;
	}
	servaddr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);


	int ret = connect(_socket, (sockaddr*)&servaddr, sizeof(servaddr));//��������  
	if (ret == SOCKET_ERROR)
	{
		DWORD dwErr = GetLastError();
		return false;
	}
	if (!rcvResponse("220"))
		return false;
	return true;
}

bool CSendMail::Logon()
{
	char local_host[MAX_BUFFER_SIZE];
	if (gethostname(local_host, MAX_BUFFER_SIZE) != 0)
	{
		m_logInfo.logInfo("Get local host name error!");
		return false;
	}

	std::string msg;

	msg = "HELO ";
	msg += std::string(local_host) + "\r\n";
	sendRequest(msg);
	if (!rcvResponse("250"))
	{
		return false;
	}

	msg = "AUTH LOGIN\r\n";
	sendRequest(msg);
	if (!rcvResponse("334"))
	{
		return false;
	}


	msg = Base64Encode(m_UserName) + "\r\n";
	sendRequest(msg);
	if (!rcvResponse("334"))
	{
		return false;
	}

	msg = Base64Encode(m_UserPwd) + "\r\n";
	sendRequest(msg);
	if (!rcvResponse("235"))
	{
		return false;
	}

	return true;//��¼�ɹ�  
}

///SendMail
//�����ʼ�ͷ 
bool CSendMail::SendHead()
{
	std::string msg;

	msg = "MAIL FROM:<";
	msg += m_SenderAddr + ">\r\n";
	sendRequest(msg);
	if (!rcvResponse("250"))
	{
		m_logInfo.logInfo("�ʼ���ַ����%s", m_SenderAddr.c_str());
		return false;
	}
	//�������receiver
	for (RECEIVERS::iterator itrRec = m_Receivers.begin(); itrRec != m_Receivers.end(); itrRec++)
	{
		msg = "RCPT TO: <";
		msg += itrRec->second + ">\r\n";
		sendRequest(msg);
		if (!rcvResponse("250"))
		{
			return false;
		}
	}
	msg = "DATA\r\n";
	sendRequest(msg);
	cout << "before 354" << endl;
	if (!rcvResponse("354"))
	{
		return false;
	}

	//����Headers
	msg = "From:\"" + m_SenderName + "\"<" + m_SenderAddr + ">\r\n";

	//����receiver
	msg += "To: ";
	for (RECEIVERS::iterator itrRec = m_Receivers.begin(); itrRec != m_Receivers.end(); itrRec++)
	{
		std::string szRecv;
		szRecv = "\"" + itrRec->first + "\"<" + itrRec->second + ">, ";
		msg += szRecv;
	}
	msg += "\r\n";

	msg += "Date: ";
	msg += prepareDate() + "\r\n";

	msg += "Subject: ";
	msg += m_MailTitle + "\r\n";

	msg += "X-Mailer: six_beauty \r\n";

	msg += "MIME-Version: 1.0\r\n";
	msg += "Content-type: multipart/mixed;  boundary=\"INVT\"\r\n\r\n";

	msg += "\r\n";
	sendRequest(msg);

	return true;
}

bool CSendMail::SendTextBody()
{
	std::string msg;
	msg = "--INVT\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n";
	msg += m_TextBody;
	msg += "\r\n\r\n";
	int len_s = sendRequest(msg, true);

	if (len_s != msg.length())
	{
		m_logInfo.logInfo("�����ʼ����ĳ���Ӧ�÷��ͳ��ȣ�%d����ʵ�ʷ��ͳ��ȣ�%d��", msg.length(), len_s);
		return false;
	}

	return true;
}


bool CSendMail::SendFileBody()
{
	std::string msg;
	//�������͸����ļ�
	for (std::list<std::string>::iterator itrList = m_FilePathList.begin(); itrList != m_FilePathList.end(); itrList++)
	{
		std::string filePath = *itrList;
		std::string fileName = GetFileName(filePath);
		std::string szContent = GetFileData(filePath);

		msg = "--INVT\r\nContent-Type: application/octet-stream;\r\n  name=\"";
		msg += fileName;
		msg += "\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n  filename=\"";
		msg += fileName;
		msg += "\"\r\n\r\n";
		sendRequest(msg, true);

		int npos = 0, len = szContent.length();
		while (npos < len)
		{
			std::string szBuffer = Base64Encode(szContent.substr(npos, min(len - npos, 3000)));
			szBuffer += "\r\n";
			sendRequest(szBuffer);
			npos += min(len - npos, 3000);
		}
	}

	return true;
}

bool CSendMail::SendEnd()
{
	std::string msg;

	msg = "--INVT--\r\n.\r\n";
	sendRequest(msg, true);

	msg = "QUIT\r\n";
	sendRequest(msg, true);

	closesocket(_socket);
	WSACleanup();

	return true;
}
