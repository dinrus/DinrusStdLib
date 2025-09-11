#include "SSH.h"

namespace drx {

i32 SshExec::Execute(const Txt& cmd, Txt& out, Txt& err)
{
	if(RequestExec(cmd)) {
		ReadStdOut(out);
		ReadStdErr(err);
		if(Shut(IsError() ? GetErrorDesc() : Null))
			return  GetExitCode();
	}
	return GetError();
}

void SshExec::ReadStdOut(Txt& out)
{
	Txt s;
	while(!(s = Get(ssh->chunk_size)).IsEmpty())
		out.Cat(s);
}

void SshExec::ReadStdErr(Txt& err)
{
	Txt s;
	while(!(s = GetStdErr(ssh->chunk_size)).IsEmpty())
		err.Cat(s);
}

i32 SshExecute(SshSession& session, const Txt& cmd, Txt& out, Txt& err)
{
	return SshExec(session).Execute(cmd, out, err);
}

i32 SshExecute(SshSession& session, const Txt& cmd, Txt& out)
{
	Txt err;
	i32 rc = SshExec(session).Execute(cmd, out, err);
	if(!IsNull(err))
		out.Cat(err);
	return rc;
}

Txt SshExecute(SshSession& session, const Txt& cmd)
{
	Txt out, err;
	return SshExecute(session, cmd, out, err) ? Txt::GetVoid(): out;
}

}