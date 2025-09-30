#include "Esc.h"


namespace drx {

#define LTIMING(x) // RTIMING(x)

void LambdaArgs(CParser& p, EscLambda& l)
{
	p.PassChar('(');
	if(!p.Char(')'))
		for(;;) {
			if(p.Char3('.', '.', '.')) {
				l.varargs = true;
				p.PassChar(')');
				break;
			}
			l.inout.Add(p.Char('&'));
			l.arg.Add(p.ReadId());
			if(p.Char('=')) {
				tukk s = p.GetPtr();
				i32 level = 0;
				for(;;) {
					if((p.IsChar(')') || p.IsChar(',')) && level == 0)
						break;
					if(p.Char(')'))
						level--;
					else
					if(p.Char('('))
						level++;
					else
						p.SkipTerm();
					if(p.IsEof())
						p.ThrowError("неожиданный конец файла при чтении дкефолтного значения из аргумента "
						             + l.arg.Top());
				}
				l.def.Add(Txt(s, p.GetPtr()));
			}
			else
				if(l.def.GetCount())
					p.ThrowError("отсутствует дефолтное значение у аргумента " + l.arg.Top());
			if(p.Char(')'))
				break;
			p.PassChar(',');
		}
	l.inout.Shrink();
	l.arg.Shrink();
}

EscVal ReadLambda(CParser& p)
{
	EscVal lambda;
	EscLambda& l = lambda.CreateLambda();
	LambdaArgs(p, l);
	tukk t = p.GetPtr();
	l.filename = p.GetFName();
	l.line = p.GetLine();
	if(!p.Char('{'))
		p.ThrowError("отсутствует '{'");
	SkipBlock(p);
	l.code = Txt(t, p.GetPtr());
	return lambda;
}

EscVal ReadLambda(tukk s)
{
	CParser p(s);
	return ReadLambda(p);
}

Txt EscEscape::InCall()
{
	return IsNull(id) ? Txt() : " при вызове '" + id + "'";
}

Txt EscEscape::DumpType(i32 i)
{
	if(i < arg.GetCount())
		return Txt().Cat() << " (" << arg[i].GetTypeName() << " присутствует)";
	else
		return " (недостаточно аргументов)";
}

void  EscEscape::CheckNumber(i32 i)
{
	if(i < arg.GetCount() && arg[i].IsNumber())
		return;
	ThrowError(Txt().Cat() << "ожидалось число в качестве параметра " << i + 1 << InCall()
	           << DumpType(i));
}

double EscEscape::Number(i32 i)
{
	if(i >= arg.GetCount())
		ThrowError("слишком мало параметров" + InCall());
	return esc.Number(arg[i], "parameter" + InCall());
}

i32 EscEscape::Int(i32 i)
{
	if(i >= arg.GetCount())
		ThrowError("слошком мало параметров" + InCall());
	return (i32)esc.Int(arg[i], "parameter" + InCall());
}

void  EscEscape::CheckArray(i32 i)
{
	if(i < arg.GetCount() && arg[i].IsArray())
		return;
	ThrowError(Txt().Cat() << "ожидался массив в качестве параметра " << i + 1 << InCall()
	           << DumpType(i));
}

void  EscEscape::CheckMap(i32 i)
{
	if(i < arg.GetCount() && arg[i].IsMap())
		return;
	ThrowError(Txt().Cat() << "ожидался мап в качестве параметра " << i + 1 << InCall());
}

void Escape(ArrayMap<Txt, EscVal>& globals, tukk function, Event<EscEscape&> escape)
{
	CParser p(function);
	EscVal& v = globals.GetPut(p.ReadId());
	EscLambda& l = v.CreateLambda();
	l.escape = escape;
	LambdaArgs(p, l);
}

void Escape(ArrayMap<Txt, EscVal>& globals, tukk function, void (*escape)(EscEscape& e))
{
	Escape(globals, function, callback(escape));
}

void  EscVal::Escape(tukk method, Event<EscEscape&> escape)
{
	CParser p(method);
	Txt id = p.ReadId();
	EscVal v;
	EscLambda& l = v.CreateLambda();
	l.escape = escape;
	LambdaArgs(p, l);
	MapSet(id, v);
}

void  EscVal::Escape(tukk method, EscHandle *h, Event<EscEscape&> escape)
{
	CParser p(method);
	Txt id = p.ReadId();
	EscVal v;
	EscLambda& l = v.CreateLambda();
	l.escape = escape;
	l.handle = h;
	h->Retain();
	LambdaArgs(p, l);
	if(IsVoid())
		SetEmptyMap();
	MapSet(id, v);
}

void Scan(ArrayMap<Txt, EscVal>& global, tukk file, tukk filename)
{
	LTIMING("Scan");
	CParser p(file, filename);
	while(!p.IsEof()) {
		EscVal& v = global.GetPut(p.ReadId());;
		v = ReadLambda(p);
	}
}

EscVal Execute(ArrayMap<Txt, EscVal>& global, EscVal *self,
                 const EscVal& lambda, Vec<EscVal>& arg, i32 op_limit)
{
	const EscLambda& l = lambda.GetLambda();
	if(arg.GetCount() != l.arg.GetCount()) {
		Txt argnames;
		for(i32 i = 0; i < l.arg.GetCount(); i++)
			argnames << (i ? ", " : "") << l.arg[i];
		throw CParser::Error(Format("неверное число аргументов (%d передано, а ожидалось: %s)", arg.GetCount(), argnames));
	}
	EscVal ret;
	{
		Esc sub(global, l.code, op_limit, l.filename, l.line);
		if(self)
			sub.self = *self;
		for(i32 i = 0; i < l.arg.GetCount(); i++)
			sub.var.GetPut(l.arg[i]) = arg[i];
		sub.Run();
		if(self)
			*self = sub.self;
		ret = sub.return_value;
	}
	return ret;
}

EscVal Execute(ArrayMap<Txt, EscVal>& global, EscVal *self,
                 tukk name, Vec<EscVal>& arg, i32 op_limit)
{
	if(!self->IsMap())
		return EscVal();
	const VecMap<EscVal, EscVal>& m = self->GetMap();
	i32 ii = m.Find(Txt(name));
	if(ii >= 0 && m[ii].IsLambda())
		return Execute(global, self, m[ii], arg, op_limit);
	return EscVal();
}

EscVal Execute(ArrayMap<Txt, EscVal>& global, tukk name, i32 op_limit)
{
	i32 ii = global.Find(Txt(name));
	Vec<EscVal> arg;
	if(ii >= 0 && global[ii].IsLambda())
		return Execute(global, NULL, global[ii], arg, op_limit);
	return EscVal();
}

EscVal Evaluatex(tukk expression, ArrayMap<Txt, EscVal>& global, i32 oplimit)
{
	Esc sub(global, expression, oplimit, "", 0);
	for(i32 i = 0; i < global.GetCount(); i++)
		sub.var.Add(global.GetKey(i), global[i]);
	EscVal v;
	v = sub.GetExp();
	for(i32 i = 0; i < sub.var.GetCount(); i++)
		global.GetAdd(sub.var.GetKey(i)) = sub.var[i];
	return v;
}

EscVal Evaluate(tukk expression, ArrayMap<Txt, EscVal>& global, i32 oplimit)
{
	try {
		return Evaluatex(expression, global, oplimit);
	}
	catch(CParser::Error&) {}
	return EscVal();
}

Txt   Expand(const Txt& doc, ArrayMap<Txt, EscVal>& global,
                i32 oplimit, Txt (*format)(const Val& v))
{
	Txt out;
	tukk term = doc;
	bool cond = true;
	while(*term)
		if(term[0] == '<' && term[1] == ':') {
			term += 2;
			try {
				Esc sub(global, term, oplimit, "", 0);
				for(i32 i = 0; i < global.GetCount(); i++)
					sub.var.Add(global.GetKey(i), global[i]);
				EscVal v;
				if(*term == '{') {
					sub.Run();
					v = sub.return_value;
				}
				else
				if(sub.Char('!')) {
					EscVal& v = global.GetPut(sub.ReadId());
					v = ReadLambda(sub);
				}
				else
				if(sub.Char('?'))
					cond = IsTrue(sub.GetExp());
				else
				if(sub.Char('/'))
					cond = !cond;
				else
				if(sub.Char('.'))
					cond = true;
				else
					v = sub.GetExp();
				if(cond)
					out << format(StdValFromEsc(v));
				sub.Spaces();
				term = sub.GetPtr();
				if(term[0] != ':' || term[1] != '>')
					throw CParser::Error("отсутствует :>" + Txt(term));
				term += 2;
			}
			catch(CParser::Error& e) {
				out << "(#ОШИБКА: " << e << "#)";
			}
		}
		else {
			if(cond)
				out.Cat(*term);
			term++;
		}
	return out;
}

}
