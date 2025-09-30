#include <drx/Esc/Esc.h>


namespace drx {

#ifdef _MSC_VER
#pragma inline_depth(255)
#pragma optimize("t", on)
#endif

#define LTIMING(x)  // RTIMING(x)

void Esc::OutOfMemory()
{
	ThrowError("Нехватка памяти");
}

void Esc::TestLimit()
{
	LTIMING("TestLimit");
	if(!IsNull(op_limit))
		if(op_limit < 0)
			ThrowError("превышен предел операций - считается замороженным");
	if(EscVal::GetTotalCount() >= EscVal::GetMaxTotalCount())
		OutOfMemory();
}

EscVal Esc::Get(const SRVal& val)
{
	LTIMING("Get");
	if(skipexp)
		return (z64)1;
	EscVal v = val.lval ? *val.lval : val.rval;
	if(val.sbs.IsArray()) {
		const Vec<EscVal>& sbs = val.sbs.GetArray();
		for(i32 i = 0; i < sbs.GetCount(); i++) {
			const EscVal& ss = sbs[i];
			if(v.IsMap()) //!!!! (problem with a[1, 2]
				v = v.MapGet(ss);
			else
			if(v.IsArray()) {
				i32 count = v.GetCount();
				if(ss.IsArray() && ss.GetArray().GetCount() >= 2) {
					EscVal v1 = ss.ArrayGet(0);
					EscVal v2 = ss.ArrayGet(1);
					i32 i = v1.GetInt();
					i32 n = count - i;
					if(ss.GetCount() == 2)
						n = v2.IsInt() ? v2.GetInt() : n;
					else {
						if(v2.IsInt()) {
							n = v2.GetInt();
							if(n < 0)
								n += count;
							n -= i;
						}
					}
					if(i >= 0 && n >= 0 && i + n <= count)
						v = v.ArrayGet(i, n);
					else
						ThrowError("срез не вписывается в диапазон");
				}
				else {
					z64 i = Int(ss, "index");
					if(i < 0)
						i += count;
					if(i >= 0 && i < count)
						v = v.ArrayGet((i32)i);
					else
						ThrowError("индекс выходит из диапазона");
				}
			}
			else
				ThrowError("неправильная индирекция");
			TestLimit();
		}
	}
	return v;
}

void Esc::Assign(EscVal& val, const Vec<EscVal>& sbs, i32 si, const EscVal& src)
{
	LTIMING("Assign");
	const EscVal& ss = sbs[si++];
	if(val.IsVoid())
		val.SetEmptyMap();
	if(val.IsMap()) {
		if(si < sbs.GetCount()) {
			EscVal x = val.MapGet(ss);
			val.MapSet(ss, 0.0);
			Assign(x, sbs, si, src);
			val.MapSet(ss, x);
		}
		else
			val.MapSet(ss, src);
		return;
	}
	else
	if(val.IsArray()) {
		if(si < sbs.GetCount()) {
			if(ss.IsArray())
				ThrowError("срез должен быть последним субскриптом");
			z64 i = Int(ss, "index");
			if(i >= 0 && i < val.GetCount()) {
				EscVal x = val.ArrayGet((i32)i);
				val.ArraySet((i32)i, 0.0);
				Assign(x, sbs, si, src);
				if(!val.ArraySet((i32)i, x))
					OutOfMemory();
				return;
			}
		}
		else {
			i32 count = val.GetCount();
			if(ss.IsArray()) {
				if(!src.IsArray() || ss.GetArray().GetCount() < 2)
					ThrowError("срезу можно присваивать только массив");
				EscVal v1 = ss.ArrayGet(0);
				EscVal v2 = ss.ArrayGet(1);
				i32 i = v1.IsInt() ? v1.GetInt() : 0;
				i32 n = count - i;
				if(ss.GetCount() == 2)
					n = v2.IsInt() ? v2.GetInt() : n;
				else {
					if(v2.IsInt()) {
						n = v2.GetInt();
						if(n < 0)
							n += count;
						n -= i;
					}
				}
				if(i >= 0 && n >= 0 && i + n <= count) {
					val.Replace(i, n, src);
					return;
				}
				else
					ThrowError("срез выходит за диапазон");
			}
			else {
				z64 i = ss.IsVoid() ? val.GetCount() : Int(ss, "index");
				if(i < 0)
					i = count + i;
				if(i >= 0 && i < INT_MAX) {
					if(!val.ArraySet((i32)i, src))
						ThrowError("вне памяти");
					return;
				}
			}
		}
	}
	ThrowError("неверная индирекция");
}

void Esc::Assign(const SRVal& val, const EscVal& src)
{
	if(skipexp)
		return;
	if(!val.lval)
		ThrowError("требуется l-значение");
	if(val.sbs.IsArray() && val.sbs.GetCount())
		Assign(*val.lval, val.sbs.GetArray(), 0, src);
	else
		*val.lval = src;
}

EscVal Esc::ExecuteLambda(const Txt& id, EscVal lambda, SRVal self, Vec<SRVal>& arg)
{
	LTIMING("ExecuteLambda");
	if(!lambda.IsLambda())
		ThrowError(Format("'%s' не является лямбдой", id));
	const EscLambda& l = lambda.GetLambda();
	if((!l.varargs && arg.GetCount() > l.arg.GetCount())
	   || arg.GetCount() < l.arg.GetCount() - l.def.GetCount())
		ThrowError("неверное число аргументом при вызове '" + id + "'");
	Esc sub(global, l.code, op_limit, l.filename, l.line);
	sub.self = Get(self);
	for(i32 i = 0; i < l.arg.GetCount(); i++) {
		sub.var.GetAdd(l.arg[i]) =
			i < arg.GetCount() ? Get(arg[i])
		                       : Evaluatex(l.def[i - (l.arg.GetCount() - l.def.GetCount())], global, op_limit);
		TestLimit();
	}
	EscVal retval;
	Array<EscVal> argvar;
	if(l.escape) {
		argvar = sub.var.PickVals();
		for(i32 i = l.arg.GetCount(); i < arg.GetCount(); i++) {
			argvar.Add(Get(arg[i]));
		}
		EscVal v = Get(self);
		EscEscape e(*this, v, argvar);
		e.id = id;
		l.escape(e);
		retval = e.ret_val;
		self = e.self;
	}
	else {
		if(l.varargs) {
			EscVal& argv = sub.var.GetAdd("argv");
			argv.SetEmptyArray();
			for(i32 i = l.arg.GetCount(); i < arg.GetCount(); i++)
				argv.ArrayAdd(Get(arg[i]));
		}
		sub.Run();
		retval = sub.return_value;
		argvar = sub.var.PickVals();
	}
	for(i32 i = 0; i < l.inout.GetCount(); i++)
		if(l.inout[i] && i < arg.GetCount() && arg[i].lval)
			Assign(arg[i], argvar[i]);
	if(self.lval)
		Assign(self, sub.self);
	return retval;
}

void Esc::Subscript(Esc::SRVal& r, Esc::SRVal _self, Txt id)
{
	LTIMING("Subscript");
	for(;;) {
		TestLimit();
		if(Char('['))
			if(Char(']'))
				r.sbs.ArrayAdd(EscVal());
			else {
				EscVal v1, v2;
				if(!IsChar(',') && !IsChar(':'))
					v1 = GetExp();
				if(Char(',')) {
					if(!IsChar(']'))
						v2 = GetExp();
					EscVal x;
					x.ArrayAdd(v1);
					x.ArrayAdd(v2);
					r.sbs.ArrayAdd(x);
				}
				else
				if(Char(':')) {
					if(!IsChar(']'))
						v2 = GetExp();
					EscVal x;
					x.ArrayAdd(v1);
					x.ArrayAdd(v2);
					x.ArrayAdd(EscVal());
					r.sbs.ArrayAdd(x);
				}
				else
					r.sbs.ArrayAdd(v1);
				PassChar(']');
			}
		else
		if(Char('.')) {
			_self = r;
			r.sbs.ArrayAdd(id = ReadId());
		}
		else
		if(Char('(')) {
			LTIMING("call fn");
			Vec<SRVal> arg;
			if(!Char(')'))
				for(;;) {
					LTIMING("make args");
					Exp(arg.Add());
					if(Char(')')) break;
					PassChar(',');
				}
			if(!IsChar2('!', '=') && Char('!')) {
				Term(_self);
				EscVal g = Get(_self);
				if(!_self.lval || (!g.IsVoid() && !g.IsMap()))
					ThrowError("мап с l-м значением или l значение void ожидались справа от !");
				if(g.IsVoid()) {
					EscVal v;
					v.SetEmptyMap();
					Assign(_self, v);
				}
			}
			if(!skipexp)
				try {
					r = ExecuteLambda(id, Get(r), _self, arg);
				}
				catch(Exc e) {
					throw Error(Format("%s.%s(): %s", Get(r).GetTypeName(), id, e));
				}
		}
		else
			return;
	}
}

void Esc::Subscript(Esc::SRVal& r)
{
	Subscript(r, SRVal(), Txt());
}

void Esc::Term(SRVal& r)
{
	r.sbs = EscVal();

	op_limit--;
	TestLimit();
	if(Char2('0', 'x') || Char2('0', 'X')) {
		r = ReadNumber64(16);
		return;
	}
	if(Char2('0', 'b') || Char2('0', 'B')) {
		r = ReadNumber64(2);
		return;
	}
	if(IsChar2('0', '.')) {
		r = ReadDouble();
		return;
	}
	if(Char('0')) {
		r = IsNumber() ? ReadNumber64(8) : 0;
		return;
	}
	if(IsNumber()) {
		// СДЕЛАТЬ: z64 !
		r = ReadDouble();
		return;
	}
	if(IsTxt()) {
		r = EscVal(ToUtf32(ReadTxt()));
		return;
	}
	if(IsChar('\'')) {
		WTxt s = ToUtf32(ReadTxt('\''));
		if(s.GetLength() != 1)
			ThrowError("неверный символьный литерал");
		r = (z64)s[0];
		return;
	}
	if(Char('@')) {
		r = ReadLambda(*this);
		Subscript(r);
		return;
	}
	if(Id("void")) {
		r = EscVal();
		return;
	}
	if(Char('{')) {
		EscVal map;
		map.SetEmptyMap();
		if(!Char('}'))
			for(;;) {
				EscVal v = GetExp();
				PassChar(':');
				map.MapSet(v, GetExp());
				if(Char('}'))
					break;
				PassChar(',');
				TestLimit();
			}
		r = map;
		Subscript(r);
		return;
	}
	if(Char('[')) {
		EscVal array;
		array.SetEmptyArray();
		if(!Char(']'))
			for(;;) {
				array.ArrayAdd(GetExp());
				if(Char(']'))
					break;
				PassChar(',');
				TestLimit();
			}
		r = array;
		Subscript(r);
		return;
	}
	if(Char('(')) {
		Exp(r);
		PassChar(')');
		Subscript(r);
		return;
	}

	SRVal _self;
	bool  _global = false;
	if(Char('.')) {
		if(!self.IsMap())
			ThrowError("доступ к члену из нечленского кода");
		_self.lval = &self;
	}
	else
	if(Char(':'))
		_global = true;
	if(IsId()) {
		Txt id = ReadId();
		EscVal method;
		i32 locali = var.Find(id);
		i32 ii;

		if(id == "self") {
			if(!self.IsMap())
				ThrowError("self в нечленском коде");
			_self.lval = &self;
			r = self;
		}
		else
		if(!_self.lval && !_global && locali < 0 && IsChar('(') &&
		   self.IsMap() && (method = self.MapGet(id)).IsLambda()) {
		    _self.lval = &self;
			r = method;
		}
		else
		if(!_self.lval && !_global && locali < 0 && IsChar('(') &&
		   (ii = global.Find(id)) >= 0 && global[ii].IsLambda()) {
			r = global[ii];
		}
		else
		if(_self.lval) {
			r = _self;
			r.sbs.ArrayAdd(id);
		}
		else
		if(_global)
			r.lval = &global.GetPut(id);
		else
			r.lval = &var.GetPut(id);

		try {
			Subscript(r, _self, id);
		}
		catch(CParser::Error e) {
			throw CParser::Error(id + ": " + e);
		}
	}
	else
		ThrowError("неверное выражение");
}

Txt Lims(const Txt& s)
{
	return s.GetLength() > 80 ? s.Mid(0, 80) : s;
}

double Esc::Number(const EscVal& a, tukk oper)
{
	if(!a.IsNumber())
		ThrowError(Txt().Cat() << "ожидался номер для '" << oper << "', а получен " << Lims(a.ToTxt()));
	return a.GetNumber();
}

z64 Esc::Int(const EscVal& a, tukk oper)
{
	if(!a.IsNumber())
		ThrowError(Txt().Cat() << "ожидалось целое для '" << oper << "', а получен " << Lims(a.ToTxt()));
	return a.GetInt64();
}

double Esc::Number(const Esc::SRVal& a, tukk oper)
{
	return Number(Get(a), oper);
}

z64 Esc::Int(const Esc::SRVal& a, tukk oper)
{
	return Int(Get(a), oper);
}

void Esc::Unary(Esc::SRVal& r)
{
	if(Char2('+', '+')) {
		Unary(r);
		EscVal v = Get(r);
		if(v.IsInt64())
			Assign(r, Int(v, "++") + 1);
		else
			Assign(r, Number(v, "++") + 1);
	}
	else
	if(Char2('-', '-')) {
		Unary(r);
		EscVal v = Get(r);
		if(v.IsInt64())
			Assign(r, Int(v, "--") - 1);
		else
			Assign(r, Number(v, "--") - 1);
	}
	else
	if(Char('-')) {
		Unary(r);
		EscVal v = Get(r);
		if(v.IsInt64())
			r = -Int(v, "-");
		else
			r = -Number(v, "-");
	}
	else
	if(Char('+')) {
		Unary(r);
		EscVal v = Get(r);
		if(v.IsInt64())
			r = Int(v, "+");
		else
			r = Number(v, "+");
	}
	else
	if(Char('!')) {
		Unary(r);
		r = (z64)!IsTrue(Get(r));
	}
	else
	if(Char('~')) {
		Unary(r);
		r = ~Int(Get(r), "~");
	}
	else
		Term(r);

	if(Char2('+', '+')) {
		EscVal v = Get(r);
		if(v.IsInt64())
			Assign(r, Int(v, "++") + 1);
		else
			Assign(r, Number(v, "++") + 1);
		r = v;
	}
	if(Char2('-', '-')) {
		EscVal v = Get(r);
		if(v.IsInt64())
			Assign(r, Int(v, "--") - 1);
		else
			Assign(r, Number(v, "--") - 1);
		r = v;
	}
}

EscVal Esc::MulArray(EscVal array, EscVal times)
{
	EscVal r;
	r.SetEmptyArray();
	for(i32 n = times.GetInt(); n > 0; n >>= 1) {
		if(n & 1)
			if(!r.Append(array))
				OutOfMemory();
		if(!array.Append(array))
			OutOfMemory();
		TestLimit();
	}
	return r;
}

void Esc::Mul(Esc::SRVal& r)
{
	Unary(r);
	for(;;)
		if(!IsChar2('*', '=') && Char('*')) {
			EscVal x = Get(r);
			SRVal w;
			Unary(w);
			EscVal y = Get(w);
			if(x.IsArray() && y.IsInt())
				r = MulArray(x, y);
			else
			if(y.IsArray() && x.IsInt())
				r = MulArray(y, x);
			else
			if(x.IsInt64() && y.IsInt64())
				r = Int(x, "*") * Int(y, "*");
			else
				r = Number(x, "*") * Number(y, "*");
		}
		else
		if(!IsChar2('/', '=') && Char('/')) {
			SRVal w;
			Unary(w);
			EscVal x = Get(r);
			EscVal y = Get(w);
			double b = Number(y, "/");
			if(b == 0)
				ThrowError("деление на ноль");
			r = Number(x, "/") / b;
		}
		else
		if(!IsChar2('%', '=') && Char('%')) {
			SRVal w;
			Unary(w);
			z64 b = Int(w, "%");
			if(b == 0)
				ThrowError("деление на ноль");
			r = Int(r, "%") % b;
		}
		else
			return;
}

void Esc::Add(Esc::SRVal& r)
{
	Mul(r);
	for(;;)
		if(!IsChar2('+', '=') && Char('+')) {
			EscVal v = Get(r);
			SRVal w;
			Mul(w);
			EscVal b = Get(w);
			if(v.IsArray() && b.IsArray()) {
				if(!v.Replace(v.GetCount(), 0, b))
					OutOfMemory();
				r = v;
			}
			else
			if(!(v.IsArray() && b.IsVoid())) {
				if(v.IsInt64() && b.IsInt64())
					r = Int(v, "+") + Int(b, "+");
				else
					r = Number(v, "+") + Number(b, "+");
			}
		}
		else
		if(!IsChar2('-', '=') && Char('-')) {
			SRVal w;
			Mul(w);
			EscVal v = Get(r);
			EscVal b = Get(w);
			if(v.IsInt64() && b.IsInt64())
				r = Int(v, "-") - Int(b, "-");
			else
				r = Number(v, "-") - Number(b, "-");
		}
		else
			return;
}

void Esc::Shift(Esc::SRVal& r)
{
	Add(r);
	for(;;)
		if(Char2('<', '<')) {
			EscVal v = Get(r);
			SRVal w;
			Add(w);
			EscVal b = Get(w);
			if(v.IsArray() && b.IsArray()) {
				if(!v.Replace(v.GetCount(), 0, b))
					OutOfMemory();
				Assign(r, v);
			}
			else
			if(!(v.IsArray() && b.IsVoid()))
				r = Int(v, "<<") << Int(b, "<<");
		}
		else
		if(Char2('>', '>')) {
			SRVal w;
			Add(w);
			r = Int(r, ">>") >> Int(w,  ">>");
		}
		else
			return;
}

double Esc::DoCompare(const EscVal& a, const EscVal& b, tukk op)
{
	LTIMING("DoCompare");
	if(a.IsInt64() && b.IsInt64())
		return SgnCompare(a.GetInt64(), b.GetInt64());
	if(a.IsNumber() && b.IsNumber())
		return SgnCompare(a.GetNumber(), b.GetNumber());
	if(a.IsArray() && b.IsArray()) {
		const Vec<EscVal>& x = a.GetArray();
		const Vec<EscVal>& y = b.GetArray();
		i32 i = 0;
		for(;;) {
			if(i >= x.GetCount())
				return i < y.GetCount() ? -1 : 0;
			if(i >= y.GetCount())
				return i < x.GetCount() ? 1 : 0;
			double q = DoCompare(x[i], y[i], op);
			if(q) return q;
			i++;
		}
	}
	if(a.IsVoid() && b.IsVoid())
		return 0;
	if(!a.IsVoid() && b.IsVoid())
		return 1;
	if(a.IsVoid() && !b.IsVoid())
		return -1;
	ThrowError("неверные значения для сравнения " + a.GetTypeName() + ' ' + op + ' ' + b.GetTypeName());
	return 0;
}

double Esc::DoCompare(const SRVal& a, tukk op)
{
	SRVal w;
	Shift(w);
	return DoCompare(Get(a), Get(w), op);
}

void Esc::Compare(Esc::SRVal& r)
{
	Shift(r);
	for(;;)
		if(Char2('>', '='))
			r = DoCompare(r, ">=") >= 0;
		else
		if(Char2('<', '='))
			r = DoCompare(r, "<=") <= 0;
		else
		if(Char('>'))
			r = DoCompare(r, ">") > 0;
		else
		if(Char('<'))
			r = DoCompare(r, "<") < 0;
		else
			return;
}

void Esc::Equal(Esc::SRVal& r)
{
	Compare(r);
	for(;;)
		if(Char2('=', '=')) {
			SRVal w;
			Compare(w);
			r = Get(r) == Get(w);
		}
		else
		if(Char2('!', '=')) {
			SRVal w;
			Compare(w);
			r = Get(r) != Get(w);
		}
		else
			return;
}

void Esc::BinAnd(Esc::SRVal& r)
{
	Equal(r);
	while(!IsChar2('&', '&') && Char('&')) {
		SRVal w;
		Equal(w);
		r = Int(r, "&") & Int(w, "&");
	}
}

void Esc::BinXor(Esc::SRVal& r)
{
	BinAnd(r);
	while(Char('^')) {
		SRVal w;
		BinAnd(w);
		r = Int(r, "^") ^ Int(w, "^");
	}
}

void Esc::BinOr(Esc::SRVal& r)
{
	BinXor(r);
	while(!IsChar2('|', '|') && Char('|')) {
		SRVal w;
		BinXor(w);
		r = Int(r, "|") | Int(w, "|");
	}
}

void Esc::And(Esc::SRVal& r)
{
	BinOr(r);
	if(IsChar2('&', '&')) {
		bool b = IsTrue(Get(r));
		while(Char2('&', '&')) {
			SRVal w;
			if(b) {
				BinOr(w);
				b = b && IsTrue(Get(w));
			}
			else {
				skipexp++;
				BinOr(w);
				skipexp--;
			}
		}
		r = b;
	}
}

void Esc::Or(Esc::SRVal& r)
{
	And(r);
	if(IsChar2('|', '|')) {
		bool b = IsTrue(Get(r));
		while(Char2('|', '|')) {
			SRVal w;
			if(b) {
				skipexp++;
				And(w);
				skipexp--;
			}
			else {
				And(w);
				b = b || IsTrue(Get(w));
			}
		}
		r = b;
	}
}

void Esc::Cond(Esc::SRVal& r)
{
	Or(r);
	if(Char('?')) {
		bool t = IsTrue(Get(r));
		SRVal dummy;
		if(t) {
			Cond(r);
			PassChar(':');
			skipexp++;
			Cond(dummy);
			skipexp--;
		}
		else {
			skipexp++;
			Cond(dummy);
			skipexp--;
			PassChar(':');
			Cond(r);
		}
	}
}

void Esc::Assign(Esc::SRVal& r)
{
	Cond(r);
	if(Char('=')) {
		SRVal w;
		Assign(w);
		Assign(r, Get(w));
	}
	else
	if(Char2('+', '=')) {
		EscVal v = Get(r);
		SRVal w;
		Cond(w);
		EscVal b = Get(w);
		if(v.IsArray() && b.IsArray()) {
			if(!v.Replace(v.GetCount(), 0, b))
				OutOfMemory();
			Assign(r, v);
		}
		else
		if(!(v.IsArray() && b.IsVoid())) {
			if(v.IsInt64() && b.IsInt64())
				Assign(r, Int(v, "+=") + Int(b, "+="));
			else
				Assign(r, Number(v, "+=") + Number(b, "+="));
		}
	}
	else
	if(Char2('-', '=')) {
		SRVal w;
		Cond(w);
		EscVal v = Get(r);
		EscVal b = Get(w);
		if(v.IsInt64() && b.IsInt64())
			Assign(r, Int(v, "-=") - Int(b, "-="));
		else
			Assign(r, Number(v, "-=") - Number(b, "-="));
	}
	else
	if(Char2('*', '=')) {
		SRVal w;
		Cond(w);
		EscVal x = Get(r);
		EscVal y = Get(w);
		if(x.IsInt64() && y.IsInt64())
			Assign(r, Int(x, "*=") * Int(y, "*="));
		else
			Assign(r, Number(x, "*=") * Number(y, "*="));
	}
	else
	if(Char2('/', '=')) {
		SRVal w;
		Cond(w);
		EscVal v = Get(r);
		EscVal b = Get(w);
		double q = Number(v, "/=");
		if(q == 0)
			ThrowError("деление на ноль");
		Assign(r, Number(b, "/=") / q);
	}
	else
	if(Char2('%', '=')) {
		SRVal w;
		Cond(w);
		z64 a = Int(r, "%=");
		z64 b = Int(w, "%=");
		if(b == 0)
			ThrowError("деление на ноль");
		Assign(r, a % b);
	}
}

i32 Esc::stack_level = 50;

void Esc::Exp(Esc::SRVal& r)
{
	LTIMING("Exp");
	Spaces();
	stack_level--;
	if(stack_level <= 0)
		ThrowError("переполнение стэка");
	Assign(r);
	stack_level++;
}

EscVal Esc::GetExp() {
	SRVal r;
	Exp(r);
	return Get(r);
}

void Esc::SkipTerm()
{
	if(IsEof())
		ThrowError("ожидался конец файла");
	CParser::SkipTerm();
	Spaces();
}

void Esc::SkipExp()
{
	i32 level = 0;
	for(;;) {
		if(IsChar(';'))
			return;
		if(IsChar(')') && level == 0)
			return;
		if(Char(')'))
			level--;
		else
		if(Char('('))
			level++;
		else
			SkipTerm();
		if(IsEof())
			ThrowError("неожиданный конец файла");
	}
}

void SkipBlock(CParser& p)
{
	i32 level = 1;
	while(level > 0 && !p.IsEof()) {
		if(p.Char('{')) level++;
		else
		if(p.Char('}')) level--;
		else
			p.SkipTerm();
	}
}

void Esc::SkipStatement()
{
	stack_level--;
	if(stack_level <= 0)
		ThrowError("переполнение стэка");
	if(Id("if")) {
		PassChar('(');
		SkipExp();
		PassChar(')');
		SkipStatement();
		if(Id("else"))
			SkipStatement();
	}
	else
	if(Id("for")) {
		PassChar('(');
		if(!IsChar(';'))
			SkipExp();
		PassChar(';');
		if(!IsChar(';'))
			SkipExp();
		PassChar(';');
		if(!IsChar(')'))
			SkipExp();
		PassChar(')');
		SkipStatement();
	}
	else
	if(Id("while") || Id("switch")) {
		PassChar('(');
		SkipExp();
		PassChar(')');
		SkipStatement();
	}
	else
	if(Id("do")) {
		SkipBlock(*this);
		PassId("while");
		PassChar('(');
		SkipExp();
		PassChar(')');
		PassChar(';');
	}
	else
	if(Char('{'))
		SkipBlock(*this);
	else {
		SkipExp();
		PassChar(';');
	}
	stack_level++;
}

bool  Esc::PCond()
{
	PassChar('(');
	bool c = IsTrue(GetExp());
	PassChar(')');
	return c;
}

void Esc::FinishSwitch()
{
	while(no_break && no_return && no_continue) {
		if(Id("case")) {
			SRVal r;
			Exp(r);
			PassChar(':');
		}
		else
		if(Id("default"))
			PassChar(':');
		else
			if(Char('}'))
				return;
		DoStatement();
	}
	while(!Char('}'))
		SkipStatement();
}

void  Esc::DoStatement()
{
	op_limit--;
	TestLimit();
	if(Id("if"))
		if(PCond()) {
			DoStatement();
			if(Id("else"))
				SkipStatement();
		}
		else {
			SkipStatement();
			if(Id("else"))
				DoStatement();
		}
	else
	if(Id("do")) {
		loop++;
		Pos pos = GetPos();
		do {
			SetPos(pos);
			DoStatement();
			PassId("while");
			no_continue = true;
		}
		while(PCond() && no_break && no_return);
		PassChar(';');
		no_break = true;
		loop--;
	}
	else
	if(Id("while")) {
		loop++;
		Pos pos = GetPos();
		for(;;) {
			SetPos(pos);
			if(!PCond() || !no_break || !no_return || !no_continue) {
				SkipStatement();
				break;
			}
			DoStatement();
			no_continue = true;
		}
		no_break = true;
		loop--;
	}
	else
	if(Id("for")) {
		loop++;
		PassChar('(');
		SRVal var;
		if(!IsChar(';'))
			Exp(var);
		if(Id("in") || Char(':')) {
			EscVal range = GetExp();
			PassChar(')');
			Pos stmt = GetPos();
			i32 i = 0;
			for(;;) {
				SetPos(stmt);
				if(range.IsArray()) {
					if(i >= range.GetCount())
						break;
					Assign(var, (z64)i);
				}
				else
				if(range.IsMap()) {
					const VecMap<EscVal, EscVal>& map = range.GetMap();
					if(i >= map.GetCount())
						break;
					if(map.IsUnlinked(i)) {
						i++;
						continue;
					}
					Assign(var, map.GetKey(i));
				}
				if(!no_break || !no_return || !no_continue) {
					SkipStatement();
					break;
				}
				DoStatement();
				no_continue = true;
				i++;
			}
			SkipStatement();
		}
		else {
			PassChar(';');
			Pos cond;
			if(!IsChar(';')) {
				cond = GetPos();
				SkipExp();
			}
			PassChar(';');
			Pos after;
			if(!IsChar(')')) {
				after = GetPos();
				SkipExp();
			}
			PassChar(')');
			Pos stmt = GetPos();
			for(;;) {
				bool c = true;
				if(cond.ptr) {
					SetPos(cond);
					c = IsTrue(GetExp());
				}
				SetPos(stmt);
				if(!c || !no_break || !no_return || !no_continue) {
					SkipStatement();
					break;
				}
				DoStatement();
				no_continue = true;
				if(after.ptr) {
					SetPos(after);
					SRVal r;
					Exp(r);
				}
			}
		}
		no_break = true;
		loop--;
	}
	else
	if(Id("break")) {
		if(!loop)
			ThrowError("неуместное 'break'");
		no_break = false;
		PassChar(';');
	}
	else
	if(Id("continue")) {
		if(!loop)
			ThrowError("неуместное 'continue'");
		no_continue = false;
		PassChar(';');
	}
	else
	if(Id("case"))
		ThrowError("неуместное 'case'");
	else
	if(Id("default"))
		ThrowError("неуместное 'default'");
	else
	if(Id("else"))
		ThrowError("неуместное 'else'");
	else
	if(Id("return")) {
		no_return = false;
		if(!Char(';')) {
			return_value = GetExp();
			PassChar(';');
		}
		else
			return_value = EscVal();
	}
	else
	if(Id("switch")) {
		loop++;
		PassChar('(');
		EscVal a = GetExp();
		PassChar(')');
		PassChar('{');
		while(!Char('}')) {
			if(Id("case")) {
				EscVal b = GetExp();
				PassChar(':');
				if(a == b) {
					FinishSwitch();
					break;
				}
			}
			else
			if(Id("default")) {
				PassChar(':');
				FinishSwitch();
				break;
			}
			else
				SkipStatement();
		}
		loop--;
		no_break = true;
	}
	else
	if(Char('#')) {
		i32 type = 0;
		if(Char('.'))
			type = 1;
		else
		if(Char(':'))
			type = 2;
		Txt id = ReadId();
		EscVal l = ReadLambda(*this);
		if(type == 1) {
			if(self.IsVoid())
				ThrowError("нет экземпляра");
			self.MapSet(id, l);
		}
		else
		if(type == 2)
			global.GetAdd(id) = l;
		else
			var.GetAdd(id) = l;
	}
	else
	if(Char('{')) {
		while(!Char('}') && no_break && no_return && no_continue)
			DoStatement();
	}
	else
	if(!Char(';')) {
		SRVal v;
		Exp(v);
		PassChar(';');
	}
}

void  Esc::Run()
{
	no_return = no_break = no_continue = true;
	loop = 0;
	skipexp = 0;
	while(!IsEof() && no_return && no_break && no_continue)
		DoStatement();
}

}
