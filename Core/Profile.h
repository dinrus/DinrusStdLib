class Txt;

#if defined(TARGET_POSIX) || defined(TARGET_WINCE)
inline i32 tmGetTime() {
	return GetTickCount();
}
#else
inline i32 tmGetTime() {
	return timeGetTime();
}
#endif

class TimingInspector {
protected:
	static bool active;

	tukk name;
	i32         call_count;
	dword       total_time;
	dword       min_time;
	dword       max_time;
	i32         max_nesting;
	i32         all_count;
	StaticMutex mutex;

public:
	TimingInspector(tukk name = NULL); // Not Txt !!!
	~TimingInspector();

	void   Add(dword time, i32 nesting);

	Txt Dump();

	class Routine {
	public:
		Routine(TimingInspector& stat, i32& nesting)
		: nesting(nesting), stat(stat) {
			start_time = tmGetTime();
			nesting++;
		}

		~Routine() {
			nesting--;
			stat.Add(start_time, nesting);
		}

	protected:
		dword start_time;
		i32& nesting;
		TimingInspector& stat;
	};

	static void Activate(bool b)                    { active = b; }
};

class HitCountInspector
{
public:
	HitCountInspector(tukk name, i32 hitcount = 0) : name(name), hitcount(hitcount) {}
	~HitCountInspector();

	void Step()              { hitcount++; }
	void Add(i32 i)          { hitcount += i; }
	void operator ++ ()      { Step(); }
	void operator += (i32 i) { Add(i); }

private:
	tukk name;
	i32         hitcount;
};

#define RTIMING(x) \
	static DRX::TimingInspector COMBINE(sTmStat, __LINE__)(x); \
	static thread_local i32 COMBINE(sTmStatNesting, __LINE__); \
	DRX::TimingInspector::Routine COMBINE(sTmStatR, __LINE__)(COMBINE(sTmStat, __LINE__), COMBINE(sTmStatNesting, __LINE__))

#define RACTIVATE_TIMING()    TimingInspector::Activate(true);
#define RDEACTIVATE_TIMING()  TimingInspector::Activate(false);

#define RHITCOUNT(n) \
	{ static HitCountInspector hitcount(n); hitcount.Step(); }
