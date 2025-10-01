#include <drx/Esc/Esc.h>


namespace drx {

#define LTIMING(x)  // RTIMING(x)

Txt EscTypeName(i32 sv_type)
{
	switch(sv_type)
	{
	case ESC_VOID:   return "void";
	case ESC_DOUBLE: return "double";
	case ESC_INT64:  return "z64";
	case ESC_ARRAY:  return "array";
	case ESC_MAP:    return "map";
	case ESC_LAMBDA: return "lambda";
	default:         return Format("unknown(%d)", sv_type);
	}
}

i32 EscVal::total;
i32 EscVal::max_total = 1000000;

void EscVal::Free()
{
	LTIMING("Free");
	if(type == ESC_ARRAY)
		array->Release();
	if(type == ESC_MAP)
		map->Release();
	if(type == ESC_LAMBDA)
		lambda->Release();
	type = ESC_VOID;
	hash = 0;
}

EscVal::~EscVal()
{
	LTIMING("~EscVal");
	total--;
	Free();
}

void EscVal::Assign(const EscVal& s)
{
	LTIMING("Assign");
	type = s.type;
	hash = s.hash;
	switch(type) {
	case ESC_ARRAY:
		array = s.array;
		array->Retain();
		break;
	case ESC_MAP:
		map = s.map;
		map->Retain();
		break;
	case ESC_LAMBDA:
		lambda = s.lambda;
		lambda->Retain();
		break;
	case ESC_DOUBLE:
		number = s.number;
		break;
	case ESC_INT64:
		i64_ = s.i64_;
		break;
	}
}

EscVal& EscVal::operator=(const EscVal& s)
{
	LTIMING("Sval=Sval");
	Free();
	Assign(s);
	return *this;
}

EscVal::EscVal(const EscVal& s)
{
	LTIMING("Sval(Sval)");
	Assign(s);
	total++;
}

EscLambda& EscVal::CreateLambda()
{
	Free();
	lambda = new EscLambda;
	type = ESC_LAMBDA;
	hash = 0;
	return *lambda;
}

const EscLambda& EscVal::GetLambda() const
{
	ASSERT(IsLambda());
	return *lambda;
}

i32 EscVal::GetCount() const
{
	switch(type) {
	case ESC_VOID:
		return 0;
	case ESC_ARRAY:
		return array->array.GetCount();
	case ESC_MAP:
		return map->count;
	}
	return 1;
}

hash_t EscVal::GetHashVal() const
{
	LTIMING("GetHashVal");
	if(hash == 0) {
		switch(type) {
		case ESC_VOID:
			hash = 1;
			break;
		case ESC_DOUBLE:
			hash = DRX::GetHashVal(number) | 0x80000;
			break;
		case ESC_INT64:
			hash = DRX::GetHashVal(i64_) | 0x80000;
			break;
		case ESC_ARRAY:
			for(i32 i = 0; i < array->array.GetCount(); i++)
				hash = hash ^ array->array[i].GetHashVal();
			hash |= 0x40000;
			break;
		case ESC_MAP:
			for(i32 i = 0; i < map->map.GetCount(); i++)
				if(!map->map[i].IsVoid())
					hash ^= map->map.GetKey(i).GetHashVal() ^ map->map[i].GetHashVal();
			hash |= 0x8000000;
			break;
		case ESC_LAMBDA:
			hash = DRX::GetHashVal(lambda->code) | 0x4000000;
			break;
		}
	}
	return hash;
}

template <class T>
bool EqVector(const T& a, const T& b)
{
	if(a.GetCount() != b.GetCount())
		return false;
	for(i32 i = 0; i < a.GetCount(); i++)
		if(a[i] != b[i])
			return false;
	return true;
}

bool EscVal::operator==(const EscVal& a) const
{
	LTIMING("operator==");
	i32 i;
	switch(type) {
	case ESC_VOID:
		return a.type == ESC_VOID;
	case ESC_INT64:
		if(a.type == ESC_INT64)
			return i64_ == a.i64_;
	case ESC_DOUBLE:
		return a.IsNumber() && GetNumber() == a.GetNumber();
	case ESC_ARRAY:
		if(a.type != ESC_ARRAY) return false;
		if(array->array.GetCount() != a.array->array.GetCount())
			return false;
		return EqVector(array->array, a.array->array);
	case ESC_MAP: {
		if(a.type != ESC_MAP) return false;
		const VecMap<EscVal, EscVal>& x = GetMap();
		const VecMap<EscVal, EscVal>& y = a.GetMap();
		for(i = 0; i < x.GetCount(); i++)
			if(!x.IsUnlinked(i)) {
				i32 q = y.Find(x.GetKey(i));
				if(q < 0)
					return false;
				if(x[i] != y[q])
					return false;
			}
		for(i = 0; i < y.GetCount(); i++)
			if(!y.IsUnlinked(i))
				if(x.Find(y.GetKey(i)) < 0)
					return false;
		return true;
	}
	case ESC_LAMBDA:
		return lambda->code == a.lambda->code &&
		       EqVector(lambda->arg, a.lambda->arg) && EqVector(lambda->inout, a.lambda->inout);
	}
	return false;
}

Txt EscVal::ToTxt(i32 maxlen, i32 indent_step, bool hex, i32 indent) const
{
	Txt ind(' ', indent);
	TxtBuf r;
	Txt s;
	i32 i;
	switch(type) {
	case ESC_DOUBLE:
		{
			s = ind;
			if((z64)number == number)
				s << FormatInt64((z64)number);
			else
				s << Format("%.8g", number);
			if(hex && FitsInInt64(number) && (z64)number == number)
				s << " 0x" << Format64Hex((z64)number);
			if(hex && number >= 0 && number < 65536 && (i32)number == number)
				s << ' ' << AsCTxt(ToUtf8(WTxt((i32)number, 1)));
			return s;
		}
	case ESC_INT64:
		{
			s = ind;
			s << FormatInt64(i64_);
			if(hex)
				s << " 0x" << Format64Hex(i64_);
			if(hex && i64_ >= 0 && i64_ < 65536)
				s << ' ' << AsCTxt(ToUtf8(WTxt((i32)i64_, 1)));
			return s;
		}
	case ESC_ARRAY:
		{
			const Vec<EscVal>& a = GetArray();
			i32 i;
			i32 c = min(100, a.GetCount());
			for(i = 0; i < a.GetCount(); i++) {
				if(!a[i].IsInt())
					break;
				i32 c = a[i].GetInt();
				if(c >= 32 && c < 256)
					s.Cat(c);
				else
					break;
			}
			if(i < a.GetCount()) {
				r << ind << "[ ";
				for(i = 0; i < c; i++) {
					if(i)
						r << ", ";
					r << array->array[i].ToTxt(maxlen, indent_step, hex, 0);
				}
				r << " ]";
				if(r.GetLength() >= maxlen) {
					r.Clear();
					r << ind << "[\n";
					for(i = 0; i < c; i++) {
						if(i)
							r << ",\n";
						r << array->array[i].ToTxt(maxlen, indent_step, hex, indent + indent_step);
					}
					r << '\n' << ind << "]";
				}
				s = r;
			}
			else {
				r << ind << '\"' << s << '\"';
				s = r;
			}
			if(a.GetCount() > 100)
				s << ind << "\n...более ста (100) элементов";
			return s;
		}
	case ESC_LAMBDA:
		r << ind << "@(";
		for(i = 0; i < lambda->arg.GetCount(); i++) {
			if(i)
				r << ", ";
			if(lambda->inout[i])
				r << "&";
			r << lambda->arg[i];
		}
		r << ")\n" << lambda->code;
		return Txt(r);
	case ESC_MAP:
		r << ind << "{ ";
		i32 c = min(map->map.GetCount(), 100);
		bool q = false;
		for(i = 0; i < c; i++) {
			if(q)
				r << ", ";
			if(!map->map.IsUnlinked(i)) {
				r << map->map.GetKey(i).ToTxt(maxlen, indent_step, hex, 0)
				  << ":" << map->map[i].ToTxt(maxlen, indent_step, hex, 0);
				q = true;
			}
		}
		r << " }";
		if(r.GetLength() >= maxlen) {
			r.Clear();
			r << ind << "{\n";
			q = false;
			for(i = 0; i < c; i++) {
				if(q)
					r << ",\n";
				if(!map->map.IsUnlinked(i)) {
					r << map->map.GetKey(i).ToTxt(maxlen, indent_step, hex, indent + indent_step)
					  << ":\n" << map->map[i].ToTxt(maxlen, indent_step, hex, indent + indent_step);
					q = true;
				}
			}
			r << '\n' << ind << "}";
		}
		s = r;
		if(map->map.GetCount() > 100)
			s << ind << "\n...более ста (100) элементов";
		return s;
	}
	return "void";
}

double EscVal::GetNumber() const
{
	if(type == ESC_INT64)
		return (double)i64_;
	if(type == ESC_DOUBLE)
		return number;
	return 0;
}

z64 EscVal::GetInt64() const
{
	if(type == ESC_INT64)
		return i64_;
	if(type == ESC_DOUBLE && number >= (double)INT64_MIN && number <= (double)INT64_MAX)
		return (z64)number;
	return 0;
}

bool EscVal::IsInt() const
{
	if(IsInt64()) {
		z64 n = GetInt64();
		return n >= INT_MIN && n <= INT_MAX;
	}
	if(IsNumber()) {
		double n = GetNumber();
		return n >= INT_MIN && n <= INT_MAX;
	}
	return false;
}

i32 EscVal::GetInt() const
{
	return IsInt() ? (i32)GetInt64() : 0;
}

bool IsTrue(const EscVal& a)
{
	if(a.IsNumber())
		return a.GetNumber();
	return a.GetCount();
}

}
