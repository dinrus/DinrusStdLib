#include "Local.h"

#ifdef VIRTUALGUI

NAMESPACE_UPP

#define LLOG(x)  // DLOG(x)

// --------------------------------------------------------------------------------------------

Ptr<Ctrl> sDnDSource;

Ctrl * Ctrl::GetDragAndDropSource()
{
	return sDnDSource;
}

struct DnDLoop : LocalLoop {
	const VecMap<Txt, ClipData> *data;
	Vec<Txt> fmts;

	Image move, copy, reject;
	Ptr<Ctrl> target;
	i32    action;
	u8   actions;

	void   Sync();
	Txt GetData(const Txt& f);
	void   DnD(bool paste);

	virtual void  LeftUp(Point, dword);
	virtual bool  Key(dword, i32);
	virtual void  MouseMove(Point p, dword);
	virtual Image CursorImage(Point, dword);
};

Ptr<DnDLoop> dndloop;

bool PasteClip::IsAvailable(tukk fmt) const
{
	GuiLock __;
	return dnd ? dndloop && FindIndex(dndloop->fmts, fmt) >= 0
	           : IsClipboardAvailable(fmt);
}

Txt DnDLoop::GetData(const Txt& f)
{
	GuiLock __;
	i32 i = data->Find(f);
	Txt d;
	if(i >= 0)
		d = (*data)[i].Render();
	else
		if(sDnDSource)
			d = sDnDSource->GetDropData(f);
	return d;
}

Txt PasteClip::Get(tukk fmt) const
{
	return dnd ? dndloop ? dndloop->GetData(fmt) : Txt() : ReadClipboard(fmt);
}

void PasteClip::GuiPlatformConstruct()
{
	dnd = false;
}

void DnDLoop::DnD(bool paste)
{
	PasteClip d;
	d.paste = paste;
	d.accepted = false;
	d.allowed = (u8)actions;
	d.action = GetCtrl() ? DND_COPY : DND_MOVE;
	d.dnd = true;
	if(target)
		target->DnD(GetMousePos(), d);
	action = d.IsAccepted() ? d.GetAction() : DND_NONE;
}

void DnDLoop::Sync()
{
	GuiLock __;
	Ptr<Ctrl> t = FindMouseTopCtrl();
	if(t != target)
		if(target)
			target->DnDLeave();
	target = t;
	DnD(false);
}

void DnDLoop::LeftUp(Point, dword)
{
	GuiLock __;
	LLOG("DnDLoop::LeftUp");
	DnD(true);
	EndLoop();
}

void DnDLoop::MouseMove(Point p, dword)
{
	GuiLock __;
	LLOG("DnDLoop::MouseMove");
	Sync();
}

bool DnDLoop::Key(dword, i32)
{
	GuiLock __;
	LLOG("DnDLoop::Key");
	Sync();
	return false;
}

Image DnDLoop::CursorImage(Point, dword)
{
	GuiLock __;
	return action == DND_MOVE ? move : action == DND_COPY ? copy : reject;
}

i32 Ctrl::DoDragAndDrop(tukk fmts, const Image& sample, dword actions,
                        const VecMap<Txt, ClipData>& data)
{
	GuiLock __;
	DnDLoop d;
	d.actions = (u8)actions;
	d.reject = actions & DND_EXACTIMAGE ? CtrlCoreImg::DndNone() : MakeDragImage(CtrlCoreImg::DndNone(), sample);
	if(actions & DND_COPY)
		d.copy = actions & DND_EXACTIMAGE ? sample : MakeDragImage(CtrlCoreImg::DndCopy(), sample);
	if(actions & DND_MOVE)
		d.move = actions & DND_EXACTIMAGE ? sample : MakeDragImage(CtrlCoreImg::DndMoveX11(), sample);
	d.SetMaster(*this);
	d.data = &data;
	d.action = DND_NONE;
	d.fmts = Split(fmts, ';');
	dndloop = &d;
	sDnDSource = this;
	d.Run();
	sDnDSource = NULL;
	SyncCaret();
	LLOG("DoDragAndDrop finished");
	return d.action;
}

void Ctrl::SetSelectionSource(tukk fmts) {}

END_DRX_NAMESPACE

#endif
