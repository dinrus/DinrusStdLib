#include "../Core.h"

#define LLOG(x) // LOG(x)

namespace drx {

StaticMutex ValCacheMutex;

std::atomic<bool> sValCacheFinished;

struct ValMakeCacheClass : LRUCache<Val> {
	~ValMakeCacheClass() { sValCacheFinished = true; }
};

LRUCache<Val>& TheValCache()
{
	static ValMakeCacheClass m;
	return m;
}

bool IsValCacheActive()
{
	return !sValCacheFinished;
}

i32 ValCacheMaxSize = 4000000;

i32 ValCacheMaxSizeLimitLow = 1024*1024;
i32 ValCacheMaxSizeLimitHigh = 0;
double ValCacheRatio = 0.125;

void AdjustValCache()
{
	Mutex::Lock __(ValCacheMutex);
	zu64 total, available;
	GetSystemMemoryStatus(total, available);
	if(ValCacheMaxSizeLimitHigh == 0)
		ValCacheMaxSizeLimitHigh = INT_MAX;
	ValCacheMaxSize = clamp((i32)min((z64)(ValCacheRatio * available), (z64)2000*1024*1024),
	                          ValCacheMaxSizeLimitLow, ValCacheMaxSizeLimitHigh);
	LLOG("New MakeVal max size " << ValCacheMaxSize << " high limit " << ValCacheMaxSizeLimitHigh);
	ShrinkValCache();
}

void ShrinkValCache()
{
	Mutex::Lock __(ValCacheMutex);
	if(!ValCacheMaxSizeLimitHigh)
		AdjustValCache();
	TheValCache().Shrink(ValCacheMaxSize, 20000);
	LLOG("MakeVal cache size after shrink: " << TheValCache().GetSize());
}

void SetupValCache(i32 limit_low, i32 limit_high, double ratio)
{
	Mutex::Lock __(ValCacheMutex);

	ValCacheMaxSizeLimitLow = 1000000;
	ValCacheMaxSizeLimitHigh = 256000000;
	ValCacheRatio = 0.125;
}

Val MakeVal(ValMaker& m)
{
	Mutex::Lock __(ValCacheMutex);
	LLOG("MakeVal cache size before make: " << TheValCache().GetSize());
	Val v = TheValCache().Get(m, [] { ValCacheMutex.Leave(); }, [] { ValCacheMutex.Enter(); });
	LLOG("MakeVal cache size after make: " << TheValCache().GetSize());
	ShrinkValCache();
	LLOG("-------------");
	return v;
}

};