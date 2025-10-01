#include "RichText.h"

#define LLOG(x) DLOG(x)

namespace drx {

RichObjectType::RichObjectType() {}
RichObjectType::~RichObjectType() {}

Txt RichObjectType::GetCreateName() const
{
	return Null;
}

Size RichObjectType::StdDefaultSize(const Val& data, Size maxsize, uk  context) const
{
	if(IsNull(data)) return Size(0, 0);
	Size psz = GetPhysicalSize(data, context);
	if((psz.cx | psz.cy) == 0)
		psz = 625 * GetPixelSize(data, context) / 100;
	Size sz;
	for(i32 i = 1; i < 10000; i++) {
		sz = psz / i;
		if(sz.cx <= maxsize.cx && sz.cy <= maxsize.cy)
			break;
	}
	return sz;
}

Size RichObjectType::GetDefaultSize(const Val& data, Size maxsize, uk context) const
{
	return GetDefaultSize(data, maxsize);
}

Size RichObjectType::GetDefaultSize(const Val& data, Size maxsize) const
{
	return StdDefaultSize(data, maxsize, NULL);
}

Val RichObjectType::Read(const Txt& s) const
{
	return s;
}

Txt RichObjectType::Write(const Val& v) const
{
	return v;
}

bool RichObjectType::IsText() const
{
	return false;
}

bool RichObjectType::Accept(PasteClip& clip)
{
	return false;
}

Val RichObjectType::Read(PasteClip& clip)
{
	return Null;
}

Txt RichObjectType::GetClipFmts() const
{
	return Null;
}

Txt RichObjectType::GetClip(const Val& data, const Txt& fmt) const
{
	return Null;
}

void RichObjectType::Menu(Bar& bar, RichObj& data, uk context) const
{
	Menu(bar, data);
}

void RichObjectType::Menu(Bar& bar, RichObj& data) const {}

void RichObjectType::DefaultAction(RichObj& data, uk context) const
{
	DefaultAction(data);
}

void RichObjectType::DefaultAction(RichObj& data) const {}

Size RichObjectType::GetPhysicalSize(const Val& data, uk context) const
{
	return GetPhysicalSize(data);
}

Size RichObjectType::GetPhysicalSize(const Val& data) const { return Size(0, 0); }

Size RichObjectType::GetPixelSize(const Val& data, uk context) const
{ 
	return GetPixelSize(data);
}

Size RichObjectType::GetPixelSize(const Val& data) const { return Size(1, 1); }

void RichObjectType::Paint(const Val& data, Draw& w, Size sz, uk context) const
{
	Paint(data, w, sz);
}

void RichObjectType::Paint(const Val& data, Draw& w, Size sz) const {}

Image RichObjectType::ToImage(z64 serial_id, const Val& data, Size sz) const
{
	return ToImage(serial_id, data, sz, NULL);
}

Image RichObjectType::ToImage(z64, const Val& data, Size sz, uk context) const
{
	ImageAnyDraw iw(sz);
	iw.DrawRect(sz, SColorPaper());
	Paint(data, iw, sz, context);
	return iw;
}

Txt RichObjectType::GetLink(const Val& data, Point pt, Size sz) const
{
	return GetLink(data, pt, sz, NULL);
}

Txt RichObjectType::GetLink(const Val& data, Point pt, Size sz, uk context) const
{
	return Null;
}

void RichObj::InitSize(i32 cx, i32 cy, uk context)
{
	Size sz;
	Size phsz = 600 * GetPixelSize() / 96; // 100% size...
	if(cx || cy)
		sz = GetRatioSize(phsz, cx, cy);
	else
		sz = phsz;
	if(sz.cx > 2000 || sz.cy > 2000)
		sz = sz.cx > sz.cy ? GetRatioSize(phsz, 2000, 0)
		                   : GetRatioSize(phsz, 0, 2000);
	SetSize(sz);
}

VecMap<Txt, RichObjectType *>& RichObj::Map()
{
	static VecMap<Txt, RichObjectType *> h;
	return h;
}

void RichObj::NewSerial()
{
	INTERLOCKED {
		static z64 s;
		serial = ++s;
	}
}

void RichObj::Register(tukk name, RichObjectType *type)
{
	AssertST();
	Map().FindAdd(name, type);
}

void RichObj::Paint(Draw& w, Size sz, uk context) const
{
	if(type)
		type->Paint(data, w, sz, context);
	else {
		w.DrawRect(sz, SColorFace());
		DrawFrame(w, sz, SColorText());
		w.DrawText(2, 2, type_name);
	}
}

Image RichObj::ToImage(Size sz, uk context) const
{
	if(type)
		return type->ToImage(serial, data, sz, context);
	else {
		ImageAnyDraw w(sz);
		Paint(w, sz, context);
		return w;
	}
}

struct UnknownRichObject : RichObjectType {
	virtual Txt GetTypeName(const Val&) const;
};

Txt UnknownRichObject::GetTypeName(const Val&) const
{
	return Null;
}

const RichObjectType& RichObj::GetType() const
{
	if(type)
		return *type;
	return Single<UnknownRichObject>();
}

void RichObj::SetData(const Val& v)
{
	data = v;
	NewSerial();
}

void RichObj::AdjustPhysicalSize()
{
	if(physical_size.cx == 0 || physical_size.cy == 0)
		physical_size = 600 * pixel_size / 96;
}

void   RichObj::Set(RichObjectType *_type, const Val& _data, Size maxsize, uk context)
{
	Clear();
	type = _type;
	if(type) {
		data = _data;
		physical_size = type->GetPhysicalSize(data, context);
		pixel_size = type->GetPixelSize(data, context);
		size = type->GetDefaultSize(data, maxsize, context);
		AdjustPhysicalSize();
	}
	NewSerial();
}

bool   RichObj::Set(const Txt& _type_name, const Val& _data, Size maxsize, uk context)
{
	NewSerial();
	type_name = _type_name;
	RichObjectType *t = Map().Get(type_name, NULL);
	if(t) {
		Set(t, _data, maxsize, context);
		return true;
	}
	else {
		Clear();
		data = _data;
		physical_size = pixel_size = size = Size(64, 64);
	}
	return false;
}

bool   RichObj::Read(const Txt& _type_name, const Txt& _data, Size sz, uk context)
{
	type_name = _type_name;
	RichObjectType *t = Map().Get(type_name, NULL);
	if(t) {
		Clear();
		type = t;
		data = type->Read(_data);
		physical_size = type->GetPhysicalSize(data, context);
		pixel_size = type->GetPixelSize(data, context);
		AdjustPhysicalSize();
		size = sz;
		NewSerial();
		return true;
	}
	data = _data;
	physical_size = pixel_size = size = sz;
	NewSerial();
	return false;
}

Txt RichObj::GetTypeName() const
{
	return type ? type->GetTypeName(data) : type_name;
}

void   RichObj::Clear()
{
	serial = 0;
	keepratio = true;
	type = NULL;
	data = Val();
	size = physical_size = pixel_size = Size(0, 0);
	ydelta = 0;
	type_name.Clear();
}

RichObj::RichObj()
{
	Clear();
}

RichObj::RichObj(RichObjectType *type, const Val& data, Size maxsize)
{
	Set(type, data, maxsize);
}

RichObj::RichObj(const Txt& type, const Val& data, Size maxsize)
{
	Set(type, data, maxsize);
}

struct RichObjectTypeDrawingCls : public RichObjectType
{
	virtual Txt GetTypeName(const Val&) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz) const;
	virtual Val  Read(const Txt& s) const;
	virtual Txt Write(const Val& v) const;

	struct Data
	{
		void    Serialize(Stream& stream);

		Drawing drawing;
		Size    dot_size;
	};
};

RichObjectType *RichObjectTypeDrawing() { return &Single<RichObjectTypeDrawingCls>(); }

void RichObjectTypeDrawingCls::Data::Serialize(Stream& stream)
{
	i32 version = 1;
	stream % version % dot_size % drawing;
}

Txt RichObjectTypeDrawingCls::GetTypeName(const Val&) const
{
	return "Drawing";
}

RichObj CreateDrawingObject(const Drawing& dwg, Size dot_size, Size out_size)
{
	RichObjectTypeDrawingCls::Data data;
	data.drawing = dwg;
	data.dot_size = dot_size;
	RichObj obj(RichObjectTypeDrawing(), RawToVal(data));
	if(!IsNull(out_size))
		obj.SetSize(out_size);
	return obj;
}

RichObj CreateDrawingObject(const Drawing& dwg, i32 cx, i32 cy)
{
	Size dsz = dwg.GetSize();
	return CreateDrawingObject(dwg, dsz, cx || cy ? GetRatioSize(dsz, cx, cy) : dsz);
}

Size RichObjectTypeDrawingCls::GetPixelSize(const Val& data) const
{
	if(IsTypeRaw<Data>(data))
		return ValTo<Data>(data).drawing.GetSize(); // dot_size; TRC 08/04/04
	return Size(0, 0);
}

Size RichObjectTypeDrawingCls::GetPhysicalSize(const Val& data) const
{
	if(IsTypeRaw<Data>(data))
		return ValTo<Data>(data).dot_size;
	return Size(0, 0);
}

Val RichObjectTypeDrawingCls::Read(const Txt& s) const
{
	Data data;
	TxtStream ss(s);
	ss % data;
	if(ss.IsError())
		return Val();
	return RawToVal(data);
}

Txt RichObjectTypeDrawingCls::Write(const Val& v) const
{
	if(IsTypeRaw<Data>(v))
	{
		TxtStream ss;
		ss % const_cast<Data&>(ValTo<Data>(v));
		return ss;
	}
	return Null;
}

void RichObjectTypeDrawingCls::Paint(const Val& data, Draw& w, Size sz) const
{
	w.DrawRect(sz, White);
	if(IsTypeRaw<Data>(data))
		w.DrawDrawing(Rect(sz), ValTo<Data>(data).drawing);
}

RichObj CreatePaintingObject(const Painting& pw, Size dot_size, Size out_size)
{
	return CreateDrawingObject(AsDrawing(pw), dot_size, out_size);
}

RichObj CreatePaintingObject(const Painting& pw, i32 cx, i32 cy)
{
	return CreateDrawingObject(AsDrawing(pw), cx, cy);
}

INITBLOCK {
	RichObj::Register("Drawing", &Single<RichObjectTypeDrawingCls>());
};

struct RichObjectTypePNGCls : public RichObjectType
{
	virtual Txt GetTypeName(const Val&) const;
	virtual Size   GetPhysicalSize(const Val& data) const;
	virtual Size   GetPixelSize(const Val& data) const;
	virtual void   Paint(const Val& data, Draw& w, Size sz) const;
	virtual Val  Read(const Txt& s) const;
	virtual Txt Write(const Val& v) const;
};

RichObjectType *RichObjectTypePNG() { return &Single<RichObjectTypePNGCls>(); }

Txt RichObjectTypePNGCls::GetTypeName(const Val&) const
{
	return "PING";
}

RichObj CreatePNGObject(const Image& img, Size dot_size, Size out_size)
{
	RichObj obj(RichObjectTypePNG(), PNGEncoder().SaveTxt(img));
	if(!IsNull(out_size))
		obj.SetSize(out_size);
	return obj;
}

RichObj CreatePNGObject(const Image& img, i32 cx, i32 cy)
{
	Size dsz = img.GetSize();
	return CreatePNGObject(img, dsz, cx || cy ? GetRatioSize(dsz, cx, cy) : dsz);
}

Size RichObjectTypePNGCls::GetPixelSize(const Val& data) const
{
	if(IsTxt(data)) {
		TxtStream strm(data);
		One<StreamRaster> ras = StreamRaster::OpenAny(strm);
		if(!!ras)
			return ras->GetSize();
	}
	return Size(0, 0);
}

Size RichObjectTypePNGCls::GetPhysicalSize(const Val& data) const
{
	if(IsTxt(data)) {
		TxtStream strm(data);
		One<StreamRaster> ras = StreamRaster::OpenAny(strm);
		if(!!ras)
			return ras->GetInfo().dots;
	}
	return Size(0, 0);
}

Val RichObjectTypePNGCls::Read(const Txt& s) const
{
	return s;
}

Txt RichObjectTypePNGCls::Write(const Val& v) const
{
	return v;
}

void RichObjectTypePNGCls::Paint(const Val& data, Draw& w, Size sz) const
{
	if(IsTxt(data)) {
		TxtStream strm(data);
		One<StreamRaster> ras = StreamRaster::OpenAny(strm);
		if(!!ras) {
			w.DrawRect(sz, White);
			w.DrawImage(Rect(sz), ras->GetImage());
			return;
		}
	}
	w.DrawRect(sz, LtRed());
}

INITBLOCK {
	RichObj::Register("PING", &Single<RichObjectTypePNGCls>());
};

}
