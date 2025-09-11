#include "IconDes.h"

namespace drx {

class AlphaImageInfo
{
public:
	enum ENCODING { COLOR_RLE, MONO_RLE, MONO_PACKED };

	AlphaImageInfo(Size size = Size(0, 0), i32 encoding = COLOR_RLE, Point hotspot = Point(0, 0))
		: encoding(encoding), size(size), hotspot(hotspot) {}

	void               Serialize(Stream& stream);

public:
	i32                encoding;
	Size               size;
	Point              hotspot;
};

Txt AlphaToRLE(const Image& aa)
{
	Txt result;
	for(i32 y = 0; y < aa.GetHeight(); y++) {
		result.Cat(PackRLE(aa[y], aa.GetWidth()));
		result.Cat(0x80);
	}
	return result;
}

Image RLEToAlpha(const Txt& rle, Size sz)
{
	ImageBuffer ib(sz);
	u8k *s = rle;
	for(i32 y = 0; y < sz.cy; y++)
		if((tukk )s < rle.End())
			s = UnpackRLE(ib[y], s, sz.cx) + 1;
		else
			memset(ib[y], 0, sz.cx * sizeof(RGBA));
	return static_cast<Image>(ib);
}

void AlphaImageInfo::Serialize(Stream& stream)
{
	i32 version = 1;
	stream / version;
	if(version >= 1)
		stream % size % hotspot % encoding;
}

void ScanIML(CParser& parser, Array<ImlImage>& out_images,
             VecMap<Txt, Txt>& out_settings)
{
	Txt name, bid;
	bool exp = false;
	while(!parser.IsEof())
	{
		if((bid = parser.ReadId()) == "IMAGE_META")
		{
			parser.Char('(');
			if(parser.IsTxt())
				name = parser.ReadTxt();
			else
				name = parser.ReadId();
			parser.PassChar(',');
			Txt value = parser.ReadTxt();
			parser.PassChar(')');
			out_settings.Add(name, value);
			if(value == "exp")
				exp = true;
		}
		else if(bid == "IMAGE_BEGIN" && parser.Char('(') && !IsNull(name = parser.ReadId()) && parser.Char(')'))
		{
			Txt encoded_data;
			out_settings.GetAdd("wince_16bit", "0");
			Txt id;
			bool first = true;
			while((id = parser.ReadId()) == "IMAGE_SCAN" && parser.Char('('))
			{
				bool first_in_row = true;
				while(parser.IsChar('\"'))
				{
					Txt scan = parser.ReadOneTxt();
					if(!first && first_in_row)
						encoded_data.Cat('\x80');
					first_in_row = first = false;
					encoded_data.Cat(scan);
				}
				if(!parser.Char(')'))
					break;
			}
			AlphaImageInfo image;
			bool accepted = false;
			if(parser.Char('(') && parser.ReadId() == name && parser.Char(',')) {
				if(id == "IMAGE_END"
				&& (image.size.cx = parser.ReadInt()) > 0 && parser.Char(',')
				&& (image.size.cy = parser.ReadInt()) > 0 && parser.Char(')')) {
					accepted = true;
				}
				else if(id == "IMAGE_PACKED" && parser.IsChar('\"')) {
					Txt d = parser.ReadOneTxt();
					if(parser.Char(')'))
					{
						TxtStream ss(d);
						ss % image;
						if(!ss.IsError())
							accepted = true;
					}
				}
			}
			if(name.GetLength() >= 6 && !memcmp(name, "_java_", 6))
				accepted = false;

			if(accepted) {
				if(name.GetLength() >= 4 && !memcmp(name, "im__", 4))
					name = Null;

				Image m = RLEToAlpha(encoded_data, image.size);
				ImageBuffer ib(m);
				ib.SetHotSpot(image.hotspot);
				m = ib;
				ImlImage& c = out_images.Add();
				c.name = name;
				c.image = m;
				c.exp = exp;
				exp = false;
			}
		}
		else if(bid == "IMAGE_BEGIN16" && parser.Char('(') && !IsNull(name = parser.ReadId()) && parser.Char(')'))
		{ //TODO: FIX THESE!!!
			out_settings.GetAdd("wince_16bit", "1");
			Txt encoded_data;
			Txt id;
			bool first = true;
			while((id = parser.ReadId()) == "IMAGE_SCAN16" && parser.Char('(') && parser.Char('L'))
			{
				bool first_in_row = true;
				while(parser.Char('\"'))
				{
					CParser::Pos pos = parser.GetPos();
					tukk end;
					end = pos.ptr; // TODO - remove
					Txt scan; // TODO = GetUnicodeScan(pos.ptr, &end);
					pos.ptr = end;
					parser.SetPos(pos);
					if(!parser.Char('\"'))
						break;
					if(!first && first_in_row)
						encoded_data.Cat('\x80');
					first_in_row = first = false;
					encoded_data.Cat(scan);
				}
				if(!parser.Char(')'))
					break;
			}
			AlphaImageInfo idata;
			bool accepted = false;
			if(id == "IMAGE_END16" && parser.Char('(') && parser.ReadId() == name && parser.Char(',')
			&& (idata.size.cx = parser.ReadInt()) > 0 && parser.Char(',')
			&& (idata.size.cy = parser.ReadInt()) > 0 && parser.Char(',')
			&& !IsNull(idata.hotspot.x = parser.ReadInt()) && parser.Char(',')
			&& !IsNull(idata.hotspot.y = parser.ReadInt()) && parser.Char(')'))
			{
				accepted = true;
			}

			if(accepted)
			{
				if(name.GetLength() >= 4 && !memcmp(name, "im__", 4))
					name = Null;

				Image m = RLEToAlpha(encoded_data, idata.size);
				ImageBuffer ib(m);
				ib.SetHotSpot(idata.hotspot);
				m = ib;
				ImlImage& c = out_images.Add();
				c.name = name;
				c.image = m;
				c.exp = exp;
				exp = false;
			}
		}
		else
			break;
	}
}

bool LoadIml(const Txt& data, Array<ImlImage>& img, i32& format)
{
	CParser p(data);
	format = 0;
	try {
		bool premultiply = !p.Id("PREMULTIPLIED");
		Vec<Txt> name;
		Vec<bool> exp;
		while(p.Id("IMAGE_ID")) {
			p.PassChar('(');
			Txt n;
			if(p.IsId()) {
				n = p.ReadId();
				if(n.StartsWith("im__", 4))
					n = Null;
				p.PassChar(')');
			}
			else
				while(!p.IsEof()) {
					if(p.Char(')'))
						break;
					p.SkipTerm();
				}
			name.Add(n);
			bool e = false;
			if(p.Id("IMAGE_META")) {
				p.PassChar('(');
				e = p.ReadTxt() == "exp";
				if(p.Char(',') && p.IsTxt())
					p.ReadTxt();
				p.PassChar(')');
			}
			exp.Add(e);
		}
		i32 ii = 0;
		while(p.Id("IMAGE_BEGIN_DATA")) {
			Txt data;
			while(p.Id("IMAGE_DATA")) {
				p.PassChar('(');
				for(i32 j = 0; j < 32; j++) {
					if(j) p.PassChar(',');
					data.Cat(p.ReadInt());
				}
				p.PassChar(')');
			}
			p.PassId("IMAGE_END_DATA");
			p.PassChar('(');
			i32 zlen = p.ReadInt();
			p.PassChar(',');
			i32 count = p.ReadInt();
			p.PassChar(')');

			data.Trim(zlen);
			Vec<ImageIml> m = UnpackImlData(data, data.GetCount());
			if(m.GetCount() != count || ii + count > name.GetCount())
				p.ThrowError("");
			for(i32 i = 0; i < count; i++) {
				ImlImage& c = img.Add();
				(ImageIml &)c = m[i];
				c.name = name[ii];
				c.exp = exp[ii++];
				c.name.TrimEnd("__DARK");
				c.name.TrimEnd("__UHD");
				if(premultiply)
					c.image = Premultiply(c.image);
			}
		}
		if(!p.IsEof())
			p.ThrowError("");
	}
	catch(CParser::Error) {
		try {
			CParser p(data);
			Array<ImlImage> m;
			VecMap<Txt, Txt> s;
			ScanIML(p, img, s);
			if(img.GetCount())
				format = 1;
		}
		catch(...) {
			return false;
		}
	}
	return true;
}

static void PutOctalTxt(Stream& out, tukk b, tukk e, const Txt& eol, bool split = false)
{
	out.Put('\"');
	z64 start = out.GetPos();
	while(b < e) {
		if(split && out.GetPos() >= start + 200u) {
			out << "\"" << eol << "\t\"";
			start = out.GetPos();
		}
		if((u8)*b >= ' ' && *b != '\x7F' && *b != '\xFF') {
			if(*b == '\\' || *b == '\"' || *b == '\'')
				out.Put('\\');
			out.Put(*b++);
		}
		else if(IsDigit(b[1]))
			out.Put(Sprintf("\\%03o", (u8)*b++));
		else
			out.Put(Sprintf("\\%o", (u8)*b++));
	}
	out.Put('\"');
}

Txt SaveIml(const Array<ImlImage>& iml, i32 format, const Txt& eol) {
	TxtStream out;
	if(format == 1) {
		for(i32 i = 0; i < iml.GetCount(); i++) {
			const ImlImage& c = iml[i];
			if(c.exp)
				out << "IMAGE_META(\"exp\", \"\")" << eol;
			Txt name = c.name;
			Image buffer = c.image;
			if(IsNull(name))
				name = "im__" + IntStr(i);
			out.PutLine(Format("IMAGE_BEGIN(%s)", name));
			i32 last = 0;
			for(i32 i = 0; i < buffer.GetHeight(); i++) {
				Txt scan = PackRLE(buffer[i], buffer.GetWidth());
				if(!scan.IsEmpty() || i == 0) // force at least 1 scan
				{
					for(; last < i; last++)
						out.PutLine("\tIMAGE_SCAN(\"\")");
					out.Put("\tIMAGE_SCAN(");
					PutOctalTxt(out, scan.Begin(), scan.End(), eol, true);
					out << ")" << eol;
					last = i + 1;
				}
			}
			out.Put("IMAGE_PACKED(");
			out.Put(name);
			out.Put(", ");
			TxtStream datastrm;
			Size size = buffer.GetSize();
			Point hotspot = buffer.GetHotSpot();
			i32 encoding = AlphaImageInfo::COLOR_RLE;
			i32 version = 1;
			datastrm / version;
			datastrm % size % hotspot % encoding;
			ASSERT(!datastrm.IsError());
			Txt s = datastrm.GetResult();
			PutOctalTxt(out, s.Begin(), s.End(), eol);
			out << ")" << eol;
		}
	}
	else {
		out << "PREMULTIPLIED" << eol;
		Index<Txt> std_name;
		for(i32 i = 0; i < iml.GetCount(); i++) {
			const ImlImage& c = iml[i];
			if(c.image.GetResolution() == IMAGE_RESOLUTION_STANDARD)
				std_name.Add(c.name);
		}
		for(i32 i = 0; i < iml.GetCount(); i++) {
			const ImlImage& c = iml[i];
			out << "IMAGE_ID(" << c.name;
			if(c.flags & IML_IMAGE_FLAG_UHD)
				out << "__UHD";
			if(c.flags & IML_IMAGE_FLAG_DARK)
				out << "__DARK";
			out << ")";
			if(c.exp)
				out << " IMAGE_META(\"exp\", \"\")" << eol;
			out << eol;
		}
		i32 ii = 0;
		while(ii < iml.GetCount()) {
			i32 bl = 0;
			i32 bn = 0;
			Vec<ImageIml> bimg;
			while(bl < 4096 && ii < iml.GetCount()) {
				const ImlImage& c = iml[ii++];
				ImageIml& m = bimg.Add();
				m.image = c.image;
				m.flags = c.flags;
				if(c.flags & IML_IMAGE_FLAG_UHD)
					SetResolution(m.image, IMAGE_RESOLUTION_UHD);
				if(c.flags & (IML_IMAGE_FLAG_FIXED|IML_IMAGE_FLAG_FIXED_SIZE))
					SetResolution(m.image, IMAGE_RESOLUTION_NONE);
				bl += (i32)c.image.GetLength();
				bn++;
			}
			Txt bs = PackImlData(bimg);
			out << eol << "IMAGE_BEGIN_DATA" << eol;
			bs.Cat(0, ((bs.GetCount() + 31) & ~31) - bs.GetCount());
			u8k *s = bs;
			for(i32 n = bs.GetCount() / 32; n--;) {
				out << "IMAGE_DATA(";
				for(i32 j = 0; j < 32; j++) {
					if(j) out << ',';
					out << (i32)*s++;
				}
				out << ")" << eol;
			}
			out << "IMAGE_END_DATA(" << bs.GetCount() << ", " << bn << ")" << eol;
		}
	}
	return out.GetResult();
}

}
