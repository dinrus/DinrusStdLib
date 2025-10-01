#include "Terminal.h"

#define LLOG(x)     // RLOG("TerminalCtrl (#" << this << "]: " << x)
#define LTIMING(x)	// RTIMING(x)

namespace drx {

void TerminalCtrl::ParseDeviceControlTxts(const VTInStream::Sequence& seq)
{
	LLOG(seq);

	const CbFunction *p = FindFunctionPtr(seq);
	if(p) p->c(*this, seq);
}

void TerminalCtrl::SetUserDefinedKeys(const VTInStream::Sequence& seq)
{
	if(!userdefinedkeys || userdefinedkeyslocked)
		return;

	bool  clear = seq.GetInt(1) == 0;
	bool  lock  = seq.GetInt(2) == 0;
	dword modifiers = seq.GetInt(3, 0);

	Vec<Txt> vudk = Split(seq.payload, ';', false);

	if(clear)
		udk.Clear();

	Txt key, val;
	for(const Txt& pair : vudk)
		if(SplitTo(pair, '/', key, val)) {
			dword k = StrInt(key);
			if(findarg(modifiers, 3, 4) >= 0 && (!pcstylefunctionkeys || k > 34))
				continue;
			k |= decode(modifiers,
					0, K_SHIFT,
					2, K_SHIFT,
					3, K_ALT,
					4, K_SHIFT|K_ALT, 0);
			LLOG("UDKey: " << key << ", modifiers: " << modifiers << ", value (in hex): " << val);
			udk.Put(k, ScanHexTxt(val));
		}

	if(lock)
		LockUDK();
}

bool TerminalCtrl::GetUDKTxt(dword key, Txt& val)
{
	if(!IsLevel2() || udk.IsEmpty())
		return false;

	i32 i = udk.Find(key);
	if(i >= 0)
		val = udk[i];

	return i >= 0 && val.GetCount();
}

void TerminalCtrl::ReportControlFunctionSettings(const VTInStream::Sequence& seq)
{
	// TODO
	Txt reply;// = "0$r";	// Invalid request (unhandled sequence)

	if(seq.payload.IsEqual("r")) {					// DECSTBM
		Rect margins = page->GetMargins();
		reply = Format("%d`$r%d;%d", 1, margins.top, margins.bottom);
	}
	else
	if(IsLevel4() && seq.payload.IsEqual("s")) {	// DECSLRM
		Rect margins = page->GetMargins();
		reply = Format("%d`$r%d;%d", 1, margins.left, margins.right);
	}
	else
	if(seq.payload.IsEqual("m")) {					// SGR
		reply = Format("%d`$r0;%s", 1, GetGraphicsRenditionOpcodes(cellattrs));
	}
	else
	if(seq.payload.IsEqual("\"p")) {				// DECSCL
		i32 level = decode(clevel, LEVEL_4, 64, LEVEL_3, 63, LEVEL_2, 62, 61);
		reply = Format("%d`$r%d;%d", 1, level, Is8BitMode() ? 0 : 1);
	}
	else
	if(IsLevel4() && seq.payload.IsEqual(" q")) {	// DECSCUSR
		i32 style = decode(caret.GetStyle(), Caret::BEAM, 6, Caret::UNDERLINE, 4, 2);
		reply = Format("%d`$r%d", 1, style - (i32) caret.IsBlinking());
	}
	else
	if(seq.payload.IsEqual("\"q")) {				// DECSCA
		reply = Format("%d`$r%d", 1, (i32) cellattrs.HasDECProtection());
	}
	else
	if(IsLevel4() && seq.payload.IsEqual("*x")) {	// DECSACE
		reply = Format("%d`$r%d", 1, streamfill ? 1 : 2);
	}
	else
	if(seq.payload.IsEqual("t")) {					// DECSLPP
		reply = Format("%d`$r%d`t", 1, page->GetSize().cy);
	}
	else
	if(seq.payload.IsEqual("$|")) {					// DECSCPP
		reply = Format("%d`$r%d", 1, page->GetSize().cx);
	}
	else
	if(seq.payload.IsEqual("*|")) {					// DECSNLS
		reply = Format("%d`$r%d", 1, page->GetSize().cy);
	}

	if(!IsNull(reply)) {
		reply << seq.payload;
	}
	else {
		reply = "0$r";	// Invalid request (unhandled sequence)
		LLOG("Unhandled report request. Token: %s " << seq.payload);
	}
	
	PutDCS(reply);
}

void TerminalCtrl::RestorePresentationState(const VTInStream::Sequence& seq)
{
	i32 which = seq.GetInt(1, 0);
	
	if(which == 1) {	// DECRSPS/DECCIR
		Vec<Txt> cr = Split(seq.payload, ';');

		if(cr.IsEmpty())
			return;

		auto GetInt = [&cr](i32 n) -> i32
		{
			Txt s = cr.Get(--n, Null);
			return (s[0] >= 0x40 && s[0] <= 0xFF)
					? (i32) s[0]
					: Nvl(StrInt(s), 0);
		};

		auto GetStr = [&cr](i32 n) -> Txt
		{
			return cr.Get(--n, Null);
		};

		auto GetChrset = [=](i32 i) -> u8
		{
			// TODO: This can be more precise...
			return decode(i,
					'0', CHARSET_DEC_DCS,
					'>', CHARSET_DEC_TCS,
					'<', CHARSET_DEC_MCS,
					'A', CHARSET_ISO8859_1,
					'G', CHARSET_UNICODE, CHARSET_TOASCII);
		};
		
		Point pt;
		pt.y      = GetInt(1);
		pt.x      = GetInt(2);
		i32 sgr   = GetInt(4);
		i32 attrs = GetInt(5);
		i32 flags = GetInt(6);
		i32 gl    = GetInt(7);
		i32 gr    = GetInt(8);
		i32 sz	  = GetInt(9);
		Txt gs = GetStr(10);
		
		cellattrs.Bold(sgr & 0x01);
		cellattrs.Blink(sgr & 0x04);
		cellattrs.Invert(sgr & 0x08);
		cellattrs.Underline(sgr & 0x02);
		cellattrs.ProtectDEC(attrs & 0x01);

		DECom(flags & 0x01);
		
		page->Attributes(cellattrs).MoveTo(pt).SetEol(flags & 0x8);
		
		if(flags & 0x02)
			gsets.SS(0x8E);
		else
		if(flags & 0x04)
			gsets.SS(0x8F);
		
		if(IsNull(gs))
			return;
		
		for(i32 i = 0; i < gs.GetLength(); i++) {
			switch(i) {
			case 0:
				gsets.G0(GetChrset(gs[i]));
				break;
			case 1:
				gsets.G1(GetChrset(gs[i]));
				break;
			case 2:
				gsets.G2(GetChrset(gs[i]));
				break;
			case 3:
				gsets.G3(GetChrset(gs[i]));
				break;
			}
		}
		
		switch(gl) {
		case 0:
			gsets.G0toGL();
			break;
		case 1:
			gsets.G1toGL();
			break;
		case 2:
			gsets.G2toGL();
			break;
		case 3:
			gsets.G3toGL();
			break;
		}

		switch(gr) {
		case 1:
			gsets.G1toGR();
			break;
		case 2:
			gsets.G2toGR();
			break;
		case 3:
			gsets.G3toGR();
			break;
		}
	}
	else
	if(which == 2) {	// DECRSPS/DECTABSR
		Vec<Txt> stab = Split(seq.payload, '/');

		page->ClearTabs();

		for(const auto& s : stab) {
			i32 pos = StrInt(s);
			if(pos > 0)
				page->SetTabAt(pos, true);
		}
	}
}

void TerminalCtrl::ParseSixelGraphics(const VTInStream::Sequence& seq)
{
	if(!sixelimages)
		return;

//	i32  ratio  = decode(seq.GetInt(1, 1), 5, 2, 6, 2, 3, 3, 4, 3, 2, 5, 1);
//	i32  grid   = seq.GetInt(3, 0); // Omitted.
	
	cellattrs.Hyperlink(false);

	ImageTxt imgs;
	imgs.data = seq.payload;
	imgs.encoded = false;
	imgs.transparent = seq.GetInt(2, 0) == 0;

	RenderImage(imgs, !modes[DECSDM]);
}
}