#include "SDMessage.h"

CSDMessage::CSDMessage()
{
	memset(&m_Head, 0, sizeof(m_Head));
}

CSDMessage::~CSDMessage()
{
	ClearContent();
}

int	CSDMessage::GetContentCount()
{
	return (int)m_aContent.size();
}

const char * CSDMessage::GetContent(int index)
{
	if (index <0 || index >= (int)m_aContent.size())
		return "";

	if (m_aContent[index])
		return m_aContent[index];

	return "";
}

void CSDMessage::SetContent(int index, const char * cs_content)
{
	if (index < 0)
		return;

	for (int i = (int)m_aContent.size(); i<=index; ++i)
		m_aContent.push_back(NULL);

	char * Content;
	if (cs_content == NULL || *cs_content=='\0')
		Content = NULL;
	else
	{
		Content = new char[(int)strlen(cs_content) +1];
		strcpy(Content, cs_content);
	}

	if (m_aContent[index] != NULL)
		delete[] m_aContent[index];

	m_aContent[index] = Content;
}


void CSDMessage::AddContent(const char * cs_content)
{
	SetContent((int)m_aContent.size(), cs_content);
}

void CSDMessage::AddContent(int i_content)
{
	char buf[32];
	itoa(i_content, buf, 10);
	SetContent((int)m_aContent.size(), buf);
}

void CSDMessage::RemoveContent(int index, int count)
{	
	if (index <0)
		return;

	int n, i;
	for (n=0, i=index ; n<count && i < (int)m_aContent.size(); ++n,++i)
	{
		if (m_aContent[i])
			delete m_aContent[i];
	}
	
	m_aContent.erase( m_aContent.begin() + index, m_aContent.begin() + index + n);	
}

void CSDMessage::ClearContent()
{
	RemoveContent(0, (int)m_aContent.size());
}

int CSDMessage::GetSize()
{
	int size = sizeof(MsgHead);

	int i,n,ln;
	for (i=0, n = (int)m_aContent.size(); i<n; ++i)
	{
		ln = (m_aContent[i] == NULL) ? 0 : (int)strlen(m_aContent[i]);
		if (i < n-1)
 			size += ln + 1;
		else
			size += ln;
	}

	return size;
}

void CSDMessage::ToBuf(unsigned char * buf)
{
	MsgHead Head;
	char * pBuf = (char *)buf;

	Head.msg_cmd	= ntohs(m_Head.msg_cmd);
	Head.trans_id	= ntohl(m_Head.trans_id);
	Head.game_type	= ntohs(m_Head.game_type);
	Head.major_v	= ntohs(m_Head.major_v);
	Head.minor_v	= ntohs(m_Head.minor_v);
	Head.area_code	= ntohs(m_Head.area_code);
	Head.mark_code	= ntohs(m_Head.mark_code);

	memcpy(pBuf, &Head, sizeof(MsgHead));
	pBuf += sizeof(MsgHead);

	int i,n,ln;
	for (i=0, n=(int)m_aContent.size(); i<n; ++i)
	{
		ln = (m_aContent[i] == NULL) ? 0 : (int)strlen(m_aContent[i]);
		if (ln)
			memcpy(pBuf, m_aContent[i], ln);
		if (i < n-1)
		{
			pBuf += ln;
			*(pBuf++) = '\0';
		}
	}
}

void CSDMessage::FromBuf(const char * buf, int length)
{
	if (length < sizeof(MsgHead))
		return;

	ClearContent();

	MsgHead Head;
	memcpy((void *)&Head, buf, sizeof(MsgHead));

	m_Head.msg_cmd		= ntohs(Head.msg_cmd);
	m_Head.trans_id		= ntohl(Head.trans_id);
	m_Head.game_type	= ntohs(Head.game_type);
	m_Head.major_v		= ntohs(Head.major_v);
	m_Head.minor_v		= ntohs(Head.minor_v);
	m_Head.area_code	= ntohs(Head.area_code);
	m_Head.mark_code	= ntohs(Head.mark_code);


	int ReadLength  = sizeof(MsgHead);
	string str;

	for (; ReadLength < length; ++ReadLength)
	{
		if (buf[ReadLength] == '\0')
		{
			AddContent(str.c_str());
			str = "";
		}else
			str+=buf[ReadLength];
	}
	if (str.length()>0)
		AddContent(str.c_str());

	CutTail();
}

void CSDMessage::CopyHead(CSDMessage & a)
{
	memcpy(&m_Head, &(a.m_Head), sizeof(MsgHead));
}

void CSDMessage::CopyHead(MsgHead & head)
{
	memcpy(&m_Head, &(head), sizeof(MsgHead));
}

void CSDMessage::CopyContent(CSDMessage & a)
{
	ClearContent();
    int i,n;
	for (i=0, n=a.GetContentCount(); i<n; ++i)
	{
		AddContent(a.GetContent(i));
	}
}

CSDMessage & CSDMessage::operator = (CSDMessage & a)
{
	CopyHead(a);
	CopyContent(a);
	return *this;
}

void CSDMessage::CutTail()
{	
	int i,n = 0;
	for (i = (int)m_aContent.size()-1; i>=0; --i)
	{
		if (m_aContent[i] == NULL || (int)strlen(m_aContent[i])==0)
		{
			++n;
			continue;
		}
		break;
	}

	if (n)
		RemoveContent(i+1, n);
}

long CSDMessage::ntohl(long a)
{
	long r = 0;
	for (int i=0; i<sizeof(a);++i)
	{
		r<<=8;
		r|= (a>>(i<<3))&0x0ff;
	}

	return r;
}

short CSDMessage::ntohs (short a)
{
	short r = 0;
	for (int i=0; i<sizeof(a);++i)
	{
		r<<=8;
		r|= (a>>(i<<3))&0x0ff;
	}

	return r;
}