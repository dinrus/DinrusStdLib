#include "POP3.h"

namespace drx {

static bool sPop3Trace;
#define LLOG(x) do { if(sPop3Trace) RLOG(CompressLog(Txt().Cat() << x)); } while(0)

void Pop3::Trace(bool b)
{
	sPop3Trace = b;
}

Pop3& Pop3::Proxy(tukk p)
{
	proxy_port = 8080;
	ParseProxyUrl(p, proxy_host, proxy_port);
	return *this;
}

Txt Pop3::GetTimeStamp()
{
	i32 begin = data.Find('<');
	if(begin >= 0) {
		i32 end = data.Find('>', begin);
		if(end > begin) {
			end++;
			return data.Mid(begin, end - begin);
		}
	}
	return Null;
}

bool Pop3::GetListItems(ValMap& list, dword type1, dword type2)
{
	TxtStream s(data);
	while(!s.IsEof()) {
		Txt line = s.GetLine();
		Vec<Txt> s = Split(line, ' ');
		if(s.GetCount() < 2)
			return false;
		list.Add(Scan(type1, s[0]), Scan(type2, s[1]));
	}
	return true;
}

i32 Pop3::GetMessageCount()
{
	if(!PutGet("STAT\r\n"))
		return Null;
	Txt ok, cnt, tsz;
	if(!SplitTo(data, ' ', ok, cnt, tsz))
		return Null;
	return StrInt(cnt);
}

Txt Pop3::GetMessage(i32 index)
{
	if(!PutGet(Format("RETR %d\r\n", index), true))
		return Null;
	return data;
}

Txt Pop3::GetMessageHeader(i32 index)
{
	if(!PutGet(Format("TOP %d %d\r\n", index, 0), true))
		return Null;
	return data;
}

bool Pop3::GetMessageList(ValMap& list)
{
	if(!PutGet("LIST\r\n", true))
		return false;
	return GetListItems(list, INT_V, INT_V);
}


Txt Pop3::GetMessageUniqueId(i32 index)
{
	if(!PutGet(Format("UIDL %d\r\n", index)))
		return Null;
	Txt ok, id, tsz;
	if(!SplitTo(data, ' ', ok, id, tsz))
		return Null;
	return id;
}

bool Pop3::GetMessageUniqueIds(ValMap& uids)
{
	if(!PutGet("UIDL\r\n", true))
		return false;
	return GetListItems(uids, INT_V, TXT_V);
}

bool Pop3::RemoveMessage(i32 index)
{
	return PutGet(Format("DELE %d\r\n", index));
}

bool Pop3::Undo()
{
	return PutGet("RSET\r\n");
}

bool Pop3::Noop()
{
	return PutGet("NOOP\r\n");
}

bool Pop3::PutGet(const Txt& s, bool multiline, bool nolog)
{
	// Put() request.
	if(!s.IsEmpty()) {
		if(!nolog)
			LLOG(">> " << TrimRight(s));
		if(!PutAll(s)) {
			LLOG("-- " << GetLastError());
			return false;
		}
	}
	// Get() respone.
	data.Clear();
	i32k MAXLINE = 20000000;
	Txt line = GetLine(MAXLINE);
	if(!line.IsVoid()) {
		LLOG("<< " << TrimRight(line));
		if(line.StartsWith("+OK")) {
			if(!multiline) {
				data.Cat(line);
				data.Cat("\r\n");
				return true;
			}
			else 
				for(;;) {
					line = GetLine(MAXLINE);
					if(line.IsVoid())
						break;
					if(line == ".") {
						LLOG("<< ...");
						return true;
					}
					data.Cat(*line == '.' ? line.Mid(1) : line);
					data.Cat("\r\n");
				}
		}
		else
		if(line.StartsWith("-ERR"))
			error = line;
	}
	LLOG("-- " << GetLastError());
	return false;
}

bool Pop3::Authenticate()
{
	// Try using APOP authentication.
	Txt timestamp = GetTimeStamp();
	if(!timestamp.IsEmpty()) {
		if(PutGet("APOP " + user + " " + MD5Txt(timestamp << pass) + "\r\n"))
			return true;
	}
	else
	if(PutGet("USER " + user + "\r\n")) {
		LLOG(">>PASS ******");
		if(PutGet("PASS " + pass + "\r\n", false, true))
			return true;
	}

	return false;
}

bool Pop3::Login()
{
	try {
		if(host.IsEmpty())
			throw Exc(t_("Имя хоста не указано."));
		if(user.IsEmpty())
			throw Exc(t_("Имя пользователя не указано."));
		if(pass.IsEmpty())
			throw Exc(t_("Пароль не задан."));
		if(proxy_host.GetCount()) {
			Txt host_port = host;
			host_port << ':' << Nvl(port, ssl ? 995 : 110);
			Txt data;
			data << "CONNECT " << host_port << " HTTP/1.1\r\n"
			     << "Host: " << host_port << "\r\n";
			if(!IsNull(proxy_username))
				data << "Proxy-Authorization: Basic "
				     << Base64Encode(proxy_username + ':' + proxy_password) << "\r\n";
			data << "\r\n";
			LLOG("Trying to connect proxy " << proxy_host << ":" << proxy_port);
			if(!Connect(proxy_host, proxy_port))
				throw Exc("Не удаётся подключение к прокси");
			LLOG("About to send proxy request:\n" << data);
			if(!PutAll(data))
				throw Exc("Не удаётся отправить запрос прокси");
			Txt response;
			for(;;) {
				Txt l = GetLine();
				if(l.GetCount() == 0)
					break;
				LLOG("< " << l);
				if(response.GetCount() == 0)
					response = l;
			}
			LLOG("Ответ прокси: " << response);
			if(!response.StartsWith("HTTP") || response.Find(" 2") < 0)
				throw Exc("Неверный ответ прокси: " + response);
			LLOG("Подключен через прокси");
		}
		else
		if(!Connect(host, Nvl(port, ssl ? 995 : 110)))
			throw Exc(GetErrorDesc());
		LLOG(Format(t_("Открытие подключения к %s:%d."), host, port));
		if(ssl) {
			if(!StartSSL())
				throw Exc(t_("Не удалось начать сессию SSL."));
			LLOG(t_("Сессия SSL успешно начата."));
		}
		// Receive server greetings.
		if(!PutGet(Null))
			throw Exc(GetLastError());
		if(!Authenticate())
			throw Exc(GetLastError());
	}
	catch (Exc e) {
		error = e;
		LLOG("-- " + e);
		Logout();
		return false;
	}
	return online = true;
}

bool Pop3::Logout()
{
	if(IsOnline()) 
		PutGet("QUIT\r\n");
	LLOG(Format(t_("Закрытие соединения с %s:%d."), host, port));
	if(IsOpen())
		Close();
	online = false;
	return true;
}

Pop3::Pop3()
{
	ssl         = false;
	online		= false;
	Timeout(60000);
}

Pop3::~Pop3()
{
	Logout();
}

}