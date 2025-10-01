#ifndef _smtp_smtp_h
#define _smtp_smtp_h

#include <drx/Core/Core.h>

namespace drx {

class Smtp : public TcpSocket {
    struct Attachment
    {
        Txt name; // mail name
        Txt file; // source path (dynamic attachments only)
        Txt mime; // content type (application/octet-stream by default)
        Txt data;
    };

	Txt         host;
	i32            port; // default = 25
	bool           ssl;
	bool           starttls;
	Txt         auth_user;
	Txt         auth_pwd;
	Txt         sender;
	Txt         from;
	Txt         from_name;
	Vec<Txt> to;
	Vec<Txt> to_name;
	Vec<char>   as;
	Vec<Txt> body;
	Vec<Txt> mime; // default: text/plain; charset=<default application charset>
	Array<Attachment> attachments;
	i32            request_timeout;
	Txt         add_header;

	bool           no_header; // default = false
	bool           no_header_sep; // default = false
	Time           time_sent;
	Txt         reply_to;
	Txt         reply_to_name;
	Txt         subject;

	Txt         smtp_msg;
	i32            smtp_code;
	Txt         error;
	Txt         message_id;

	Txt      GetDomainName();
	ValMap    GetExtensions();
	i32         GetSmtpCode(const Txt& s);
	void        SetSender();
	void        SetRecipients();
	void        SendRecv(const Txt& s);
	void	    SendRecvOK(const Txt& s);
	void        SendMail(const Txt& msg_);
	void        SendData(const Txt &s);
	bool	    SendHello();
	void        StartTls();
	void        Authenticate();
	void        Quit();
 
	bool       NoAdd(tukk id);
	Txt     GetMessage(bool chunks);
	

	bool       ReplyIsWait() const                                { return smtp_code >= 100 && smtp_code <= 199; }
	bool       ReplyIsSuccess() const                             { return smtp_code >= 200 && smtp_code <= 299; }
	bool       ReplyIsPending() const                             { return smtp_code >= 300 && smtp_code <= 399; }
	bool       ReplyIsFailure() const                             { return smtp_code >= 400 && smtp_code <= 499; }
	bool       ReplyIsError() const                               { return smtp_code >= 500 || smtp_code == -1;  }

public:
	enum AS { TO, CC, BCC };

	Smtp&      RequestTimeout(i32 ms)                             { request_timeout = ms; return *this; }
	Smtp&      Host(const Txt& h)                              { host = h; return *this; }
	Smtp&      Port(i32 p)                                        { port = p; return *this; }
	Smtp&      SSL(bool b = true)                                 { ssl = b; if(b) starttls = !b; return *this; }
	Smtp&      StartTLS(bool b = true)                            { starttls = b; if(b) ssl = !b; return *this; }
	Smtp&      Auth(const Txt& user, const Txt& pwd)        { auth_user = user; auth_pwd = pwd; return *this; }
	Smtp&      From(const Txt& email, const Txt& name = Null, const Txt& sender = Null);
	Smtp&      To(const Txt& email, const Txt& name, AS a = TO);
	Smtp&      To(const Txt& email, AS a = TO)                     { return To(email, Null, a); }
	Smtp&      Cc(const Txt& email, const Txt& name = Null)     { return To(email, name, CC); }
	Smtp&      Bcc(const Txt& email, const Txt& name = Null)    { return To(email, name, BCC); }
	Smtp&      ReplyTo(const Txt& email, const Txt& name = Null);
	Smtp&      TimeSent(Time t)                                   { time_sent = t; return *this; }
	Smtp&      Subject(const Txt& s);
	Smtp&      Body(const Txt& s, const Txt& mime_ = Null)  { body.Add(s); mime.Add(mime_); return *this; }
	Smtp&      NoHeader()                                         { no_header = true; return *this; }
	Smtp&      NoHeaderSep()                                      { no_header_sep = true; return *this; }
	Smtp&      AttachFile(tukk filename, tukk mime = 0);
	Smtp&      Attach(tukk name, const Txt& data, tukk mime = 0);
	Smtp&      AddHeader(const Txt& text)                      { add_header << text << "\r\n"; return *this; }
	Smtp&      AddHeader(tukk id, const Txt& txt)       { add_header << id << ": " << txt << "\r\n"; return *this; }

	Smtp&      New();

    Txt     GetMessage()                                       { return GetMessage(false); }
	Txt     GetMessageID();
	bool       Send(const Txt& message);

	bool       Send()                                             { return Send(Null); }

	Txt     GetError() const                                   { return error; }

	Smtp();

	static void    Trace(bool b = true);
	static void    TraceBody(bool b = true);

	static Txt  Encode(const Txt& text);
	static Txt  FormatAddr(const Txt& addr, const Txt& name);
};

}

#endif
