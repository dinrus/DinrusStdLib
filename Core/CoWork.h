class CoWork : NoCopy {
	struct MJob : Moveable<MJob>, Link<MJob, 2> {
		Function<void ()> fn;
		CoWork           *work = NULL;
		bool              looper = false;
	};

	enum { SCHEDULED_MAX = 2048 };

public:
	struct Pool {
		MJob             *free;
		Link<MJob, 2>     jobs;
		MJob              slot[SCHEDULED_MAX];
		i32               waiting_threads;
		Array<Thread>     threads;
		bool              quit;

		Mutex             lock;
		ConditionVariable waitforjob;

		void              Free(MJob& m);
		void              DoJob(MJob& m);
		void              PushJob(Function<void ()>&& fn, CoWork *work, bool looper = false);

		void              InitThreads(i32 nthreads);
		void              ExitThreads();

		Pool();
		~Pool();

		static thread_local bool    finlock;

		bool DoJob();
		static void ThreadRun(i32 tno);
	};

	friend struct Pool;

	static Pool& GetPool();

	static thread_local i32 worker_index;
	static thread_local CoWork *current;

	ConditionVariable  waitforfinish;
	Link<MJob, 2>      jobs; // global stack and CoWork stack as double-linked lists
	i32                todo;
	bool               canceled;
	
	#ifndef COSMO
	std::exception_ptr exc = nullptr; // workaround for sanitizer bug(?)
	#endif
	
	Function<void ()>  looper_fn;
	i32                looper_count;

	void Do0(Function<void ()>&& fn, bool looper);

	void Cancel0();
	void Finish0();

	Atomic             index;

// experimental pipe support
	Mutex stepmutex;
	Array<BiVec<Function<void ()>>> step;
	Vec<bool> steprunning;

public:
	static bool TrySchedule(Function<void ()>&& fn);
	static bool TrySchedule(const Function<void ()>& fn)      { return TrySchedule(clone(fn)); }
	static void Schedule(Function<void ()>&& fn);
	static void Schedule(const Function<void ()>& fn)         { return Schedule(clone(fn)); }

	void     Do(Function<void ()>&& fn)                       { Do0(pick(fn), false); }
	void     Do(const Function<void ()>& fn)                  { Do(clone(fn)); }

	CoWork&  operator&(const Function<void ()>& fn)           { Do(fn); return *this; }
	CoWork&  operator&(Function<void ()>&& fn)                { Do(pick(fn)); return *this; }

	void     Loop(Function<void ()>&& fn);
	void     Loop(const Function<void ()>& fn)                { Loop(clone(fn)); }

	CoWork&  operator*(const Function<void ()>& fn)           { Loop(fn); return *this; }
	CoWork&  operator*(Function<void ()>&& fn)                { Loop(pick(fn)); return *this; }

	i32      Next()                                           { return ++index - 1; }

	i32  GetScheduledCount() const                            { return todo; }
	void Pipe(i32 stepi, Function<void ()>&& lambda); // experimental

	static void FinLock();

	void Cancel();
	static bool IsCanceled();

	void Finish();

	bool IsFinished();

	void Reset();

	static bool IsWorker()                                    { return GetWorkerIndex() >= 0; }
	static i32  GetWorkerIndex();
	static i32  GetPoolSize();
	static void SetPoolSize(i32 n);

	CoWork();
	~CoWork() noexcept(false);
};

struct CoWorkNX : CoWork {
	~CoWorkNX() noexcept(true) {}
};

inline
void CoDo(Function<void ()>&& fn)
{
	CoWork co;
	co * fn;
}

inline
void CoDo_ST(Function<void ()>&& fn)
{
	fn();
}

inline
void CoDo(bool co, Function<void ()>&& fn)
{
	if(co)
		CoDo(pick(fn));
	else
		CoDo_ST(pick(fn));
}

template <typename Fn>
void CoFor(i32 n, Fn iterator)
{
	std::atomic<i32> ii(0);
	CoDo([&] {
		for(i32 i = ii++; i < n; i = ii++)
			iterator(i);
	});
}

template <typename Fn>
void CoFor_ST(i32 n, Fn iterator)
{
	for(i32 i = 0; i < n; i++)
		iterator(i);
}

template <typename Fn>
void CoFor(bool co, i32 n, Fn iterator)
{
	if(co)
		CoFor(n, iterator);
	else
		CoFor_ST(n, iterator);
}

template <class T>
class CoWorkerResources {
	i32          workercount;
	Buffer<T>    res;

public:
	i32 GetCount() const  { return workercount + 1; }
	T& operator[](i32 i)  { return res[i]; }

	T& Get()              { i32 i = CoWork::GetWorkerIndex(); return res[i < 0 ? workercount : i]; }
	T& operator~()        { return Get(); }

	T *begin()            { return ~res; }
	T *end()              { return ~res + GetCount(); }

	CoWorkerResources()   { workercount = CoWork::GetPoolSize(); res.Alloc(GetCount()); }

	CoWorkerResources(Event<T&> initializer) : CoWorkerResources() {
		for(i32 i = 0; i < GetCount(); i++)
			initializer(res[i]);
	}
};

template <class Ret>
class AsyncWork {
	template <class Ret2>
	struct Imp {
		CoWork co;
		Ret2   ret;

		template<class Function, class... Args>
		void        Do(Function&& f, Args&&... args) { co.Do([=, this]() { ret = f(args...); }); }
		const Ret2& Get()                            { return ret; }
		Ret2        Pick()                           { return pick(ret); }
	};

	struct ImpVoid {
		CoWork co;

		template<class Function, class... Args>
		void        Do(Function&& f, Args&&... args) { co.Do([=, this]() { f(args...); }); }
		void        Get()                            {}
		void        Pick()                           {}
	};

	using ImpType = typename std::conditional<std::is_void<Ret>::value, ImpVoid, Imp<Ret>>::type;

	One<ImpType> imp;

public:
	template< class Function, class... Args>
	void  Do(Function&& f, Args&&... args)          { imp.Create().Do(f, args...); }

	void        Cancel()                            { if(imp) imp->co.Cancel(); }
	static bool IsCanceled()                        { return CoWork::IsCanceled(); }
	bool        IsFinished()                        { return imp && imp->co.IsFinished(); }
	Ret         Get()                               { ASSERT(imp); imp->co.Finish(); return imp->Get(); }
	Ret         operator~()                         { return Get(); }
	Ret         Pick()                              { ASSERT(imp); imp->co.Finish(); return imp->Pick(); }

	AsyncWork& operator=(AsyncWork&&) = default;
	AsyncWork(AsyncWork&&) = default;

	AsyncWork()                                     {}
	~AsyncWork()                                    { if(imp) imp->co.Cancel(); }
};

template< class Function, class... Args>
AsyncWork<
#ifdef __clang__//CPP_17
	std::invoke_result_t<Function, Args...>
#else
	typename std::result_of<
		typename std::decay<Function>::type
			(typename std::decay<Args>::type...)
	>::type
#endif
>
Async(Function&& f, Args&&... args)
{
	AsyncWork<
#if defined(CPP_20) && defined(__clang__)
		std::invoke_result_t<Function, Args...>
#else
		typename std::result_of<
			typename std::decay<Function>::type
				(typename std::decay<Args>::type...)
		>::type
#endif
	> h;
	h.Do(f, args...);
	return pick(h);
}