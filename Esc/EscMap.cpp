#include "Esc.h"

namespace drx {

#define LTIMING(x) // RTIMING(x)

VecMap<EscVal, EscVal>& EscVal::CloneMap()
{
	LTIMING("CloneMap");
	ASSERT(IsMap());
	if(map->refcount != 1) {
		EscMap *c = new EscMap;
		c->map <<= map->map;
		map->Release();
		map = c;
	}
	hash = 0;
	return map->map;
}

const VecMap<EscVal, EscVal>& EscVal::GetMap() const
{
	ASSERT(IsMap());
	return map->map;
}

void  EscVal::SetEmptyMap()
{
	Free();
	type = ESC_MAP;
	hash = 0;
	map = new EscMap;
}

EscVal EscVal::MapGet(EscVal key) const
{
	LTIMING("MapGet");
	return GetMap().Get(key, EscVal());
}

void EscVal::MapSet(EscVal key, EscVal value)
{
	LTIMING("MapSet");
	if(IsVoid())
		SetEmptyMap();
	VecMap<EscVal, EscVal>& m = CloneMap();
	i32 q = m.Find(key);
	if(q >= 0) {
		if(value.IsVoid()) {
			m.Unlink(q);
			map->count--;
		}
		else
			m[q] = value;
	}
	else
		if(!value.IsVoid()) {
			map->count++;
			m.Add(key, value);
		}
}

bool EscVal::HasNumberField(tukk id) const
{
	return IsMap() && GetMap().Find(id) >= 0;
}

i32  EscVal::GetFieldInt(tukk id) const
{
	return HasNumberField(id) ? MapGet(id).GetInt() : 0;
}

}
