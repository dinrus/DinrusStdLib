#include "CtrlCore.h"

namespace drx {

// #define LOG_QUEUE
#define LLOG(x) // LOG(x)
// #define DELAY_WATCH  1000 _DBG_

i32 MemoryProbeInt;

struct TimeEvent : public Link<TimeEvent> {
	dword      time;
	i32        delay;
	Event<>    cb;
	void      *id;
	bool       rep;
};

static dword sTClick;

static StaticCriticalSection sTimerLock;

struct CtrlTimerOwner__ : public LinkOwner<TimeEvent> {
	CtrlTimerOwner__();
	~CtrlTimerOwner__();
};

static TimeEvent *tevents() {
	static LinkOwner<TimeEvent> t;
	return t.GetPtr();
}

static void sTimeCallback(dword time, i32 delay, Event<>  cb, uk id) {
	TimeEvent *ne = tevents()->InsertPrev();
	ne->time = time;
	ne->cb = cb;
	ne->delay = delay;
	ne->id = id;
	ne->rep = false;
	LLOG("sTimeCalllback " << ne->time << " " << ne->delay << " " << ne->id);
}

void SetTimeCallback(i32 delay_ms, Function<void ()> cb, uk id) {
	Mutex::Lock __(sTimerLock);
	ASSERT(abs(delay_ms) < 0x40000000);
	LLOG("SetTimeCallback " << delay_ms << " " << id);
	sTimeCallback(msecs() + abs(delay_ms), delay_ms, Event<> () << cb, id);
}

void KillTimeCallbacks(uk id, uk idlim) {
	Mutex::Lock __(sTimerLock);
	TimeEvent *list = tevents();
	for(TimeEvent *e = list->GetNext(); e != list;)
		if(e->id >= id && e->id < idlim) {
			e = e->GetNext();
			delete e->GetPrev();
		}
		else
			e = e->GetNext();
}

EXITBLOCK
{
	Mutex::Lock __(sTimerLock);
	while(tevents()->GetNext() != tevents())
		delete tevents()->GetNext();
}

bool ExistsTimeCallback(uk id) {
	Mutex::Lock __(sTimerLock);
	TimeEvent *list = tevents();
	for(TimeEvent *e = list->GetNext(); e != list; e = e->GetNext())
		if(e->id == id)
			return true;
	return false;
}

void KillTimeCallback(uk id) {
	KillTimeCallbacks(id, (u8 *)id + 1);
}

void Ctrl::TimerProc(dword time)
{
	if(IsPanicMode())
		return;
	sTimerLock.Enter();
	TimeEvent *list = tevents();
	if(time == sTClick) {
		sTimerLock.Leave();
		return;
	}
	sTClick = time;
	sTimerLock.Leave();
	Ctrl::CheckMouseCtrl();
	Ctrl::SyncCaret();
	sTimerLock.Enter();

	#ifdef LOG_QUEUE
		LLOG("--- Timer queue at " << time);
		for(TimeEvent *e = list->GetNext(); e != list; e = e->GetNext())
			LLOG("TP " << e->time << " " << e->delay << " " << e->id << " " << e->rep);
		LLOG("----");
	#endif

	for(;;) {
		TimeEvent *todo = NULL;
		i32 maxtm = -1;
		for(TimeEvent *e = list->GetNext(); e != list; e = e->GetNext()) {
			i32 tm = (i32)(time - e->time);
			if(!e->rep && tm >= 0 && tm > maxtm) {
				maxtm = tm;
				todo = e;
			}
		}
		if(!todo)
			break;
		LLOG("Performing " << todo->time << " " << todo->delay << " " << todo->id);
		Event<>  cb = todo->cb;
		if(todo->delay < 0)
			todo->rep = true;
		else
			delete todo;
		sTimerLock.Leave();
	#if DELAY_WATCH
		i32 tm = msecs();
	#endif
		cb();
	#if DELAY_WATCH
		if(msecs() - tm > DELAY_WATCH)
			Panic("Long timer procedure detected!");
	#endif
		sTimerLock.Enter();
	}
	time = msecs();
	LLOG("--- Rescheduling at " << time);
	TimeEvent *e = list->GetNext();
	while(e != list) {
		TimeEvent *w = e;
		e = e->GetNext();
		if(w->rep) {
			LLOG("Rescheduling " << e->id);
			sTimeCallback(time - w->delay, w->delay, w->cb, w->id);
			delete w;
		}
	}
	LLOG("----");
	sTimerLock.Leave();
}

void  Ctrl::InitTimer()
{
	Mutex::Lock __(sTimerLock);
	tevents();
}

void  Ctrl::SetTimeCallback(i32 delay_ms, Function<void ()> cb, i32 id) {
	ASSERT(id >= 0 && (size_t)id < (i32)sizeof(Ctrl));
	DRX::SetTimeCallback(delay_ms, cb, (u8 *)this + id);
}

void  Ctrl::KillTimeCallback(i32 id) {
	ASSERT(id >= 0 && (size_t)id < sizeof(Ctrl));
	DRX::KillTimeCallback((u8 *)this + id);
}

void  Ctrl::KillSetTimeCallback(i32 delay_ms, Function<void ()> cb, i32 id)
{
	KillTimeCallback(id);
	SetTimeCallback(delay_ms, cb, id);
}

void  Ctrl::PostCallback(Function<void ()> cb, i32 id)
{
	SetTimeCallback(0, cb, id);
}

void  Ctrl::KillPostCallback(Function<void ()> cb, i32 id)
{
	KillSetTimeCallback(0, cb, id);
}

bool  Ctrl::ExistsTimeCallback(i32 id) const {
	ASSERT(id >= 0 && (size_t)id < sizeof(Ctrl));
	return DRX::ExistsTimeCallback((u8 *)this + id);
}

dword GetTimeClick()
{
	return sTClick;
}

}
