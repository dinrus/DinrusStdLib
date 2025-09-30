extern StaticMutex ValCacheMutex;

LRUCache<Val>& TheValCache();

typedef LRUCache<Val>::Maker ValMaker;

Val MakeVal(ValMaker& m);

bool IsValCacheActive();

void AdjustValCache();
void ShrinkValCache();

void SetupValCache(i32 limit_low, i32 limit_high, double ratio);

template <class P>
i32 ValCacheRemove(P what)
{
	Mutex::Lock __(ValCacheMutex);
	return TheValCache().Remove(what);
}

template <class P>
i32 ValCacheRemoveOne(P what)
{
	Mutex::Lock __(ValCacheMutex);
	return TheValCache().Remove(what);
}

template <class P>
void ValCacheAdjustSize(P getsize)
{
	Mutex::Lock __(ValCacheMutex);
	TheValCache().AdjustSize(getsize);
}
