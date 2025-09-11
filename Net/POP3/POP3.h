#ifndef _POP3_POP3_h
#define _POP3_POP3_h

#include <drx/Core/Core.h>

namespace drx {

class Pop3 : public TcpSocket
{
	Txt       proxy_host;
	i32          proxy_port;
	Txt       proxy_username;
	Txt       proxy_password;

	Txt       host;
	Txt       user;
	Txt       pass;
	Txt       error;
	Txt       data;
	i32       port;
	bool      ssl;
	bool      online;

	bool      GetListItems(ValMap& list, dword type1, dword type2);
	Txt       GetTimeStamp();
	bool      Authenticate();
	bool      PutGet(const Txt& s, bool multiline = false, bool nolog = false);

public:
	Pop3&        Host(const Txt& h)                  { host = h; return *this; }
	Pop3&        Port(i32 p)                            { port = p; return *this; }
	Pop3&        User(const Txt& u, const Txt& p) { user = u; pass = p; return *this; }
	Pop3&        SSL(bool b = true)                     { ssl  = b; return *this; }
	
	Pop3&        Proxy(const Txt& host, i32 port)         { proxy_host = host; proxy_port = port; return *this; }
	Pop3&        Proxy(tukk p);
	Pop3&        ProxyAuth(const Txt& u, const Txt& p) { proxy_username = u; proxy_password = p; return *this; }

	i32          GetMessageCount();
	Txt          GetMessage(i32 index);
	Txt          GetMessageHeader(i32 index);
	bool         RemoveMessage(i32 index);

	bool         GetMessageList(ValMap& list);
	Txt          GetMessageUniqueId(i32 index);
	bool         GetMessageUniqueIds(ValMap& uids);

	bool         Undo();
	bool         Noop();
	
	bool         Login();
	bool         Logout();
	
	bool         IsOnline() const                         { return online; }

	Txt          GetLastError()                           { return IsError() ? GetErrorDesc() : error; }
	static void  Trace(bool b = true);

	Pop3();
	~Pop3();
};

struct InetMessage {
	struct Part : Moveable<Part> {
		i32                       parent;
		VecMap<Txt, Txt> header;
		Txt                    body;
		
		Txt    operator[](tukk id) const          { return header.Get(id, Null); }
		void   Set(tukk id, const Txt& value)  { header.GetAdd(id) = value; }
		Txt    Decode() const;
		
		bool   IsMultipart(Txt& boundary, Txt& end_boundary) const;
	};

	Vec<Part> part;

	bool   Read(const Txt& msg);
	bool   ReadHeader(const Txt& msg);
	
	void   Clear()                                       { part.Clear(); }

	i32         GetCount() const                         { return part.GetCount(); }
	const Part& operator[](i32 i) const                  { return part[i]; }
	Txt         operator[](tukk id) const         { return GetCount() ? part[0][id] : Txt(); }
	void        Set(tukk id, const Txt& value) { return part.At(0).Set(id, value); }
	
	Txt    GetMessage() const;

private:
	bool   ReadHeader(VecMap<Txt, Txt>& hdr, Stream& ss);
	bool   ReadPart(Stream& ss, i32 parent, i32 level);
	void   PutBody(i32 pi, Txt& r, i32 level) const;
};

struct MIMEHeader {
	Txt value;
	VecMap<Txt, Txt> param;
	
	Txt operator[](tukk id) const { return param.Get(id, Null); }
	Txt operator~() const                { return value; }
	
	bool   Parse(tukk s);
	Txt    ToTxt() const;
	
	MIMEHeader(tukk s)               { Parse(s); }
	MIMEHeader()                            {}
};

Index<Txt> ParseMessageIDs(const Txt& s);
Txt        FormatMessageIDs(const Index<Txt>& id);

}

#endif
