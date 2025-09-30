////#include "../Compress.h"

namespace drx {

void UnZip::ReadDir()
{
	error = true;
	
	file.Clear();
	current = 0;

	i32 entries = -1;
	i32 offset;

	i64 zipsize = zip->GetSize();
	i64 pos = zipsize - 1; //22;
	zip->Seek(std::max((z64)0, zip->GetSize() - 4000)); // Precache end of zip
	zip->Get();
	while(pos >= std::max((z64)0, zip->GetSize() - 65536)) {
		zip->ClearError();
		zip->Seek(pos);
		if(zip->Get32le() == 0x06054b50) {
			zip->Get16le();  // number of this disk
			zip->Get16le();  // number of the disk with the start of the central directory
			i32 h = zip->Get16le(); // total number of entries in the central directory on this disk
			entries = zip->Get16le(); // total number of entries in the central directory
			if(h != entries) // Multiple disks not supported
				return;
			zip->Get32le(); // size of the central directory
			offset = zip->Get32le(); //offset of start of central directory with respect to the starting disk number
			i32 commentlen = zip->Get16le();
			if(zip->GetPos() + commentlen == zipsize)
				break;
		}
		pos--;
	}
	if(entries < 0)
		return;

	zip->Seek(offset);
	for(i32 i = 0; i < entries; i++) {
		if(zip->Get32le() != 0x02014b50 && zip->IsEof())
			return;
		File& f = file.Add();
		zip->Get16le();
		zip->Get16le();
		f.bit = zip->Get16le();  // general purpose bit flag
		f.method = zip->Get16le();
		f.time = zip->Get32le();
		f.crc = zip->Get32le();
		f.csize = zip->Get32le();
		f.usize = zip->Get32le();
		i32 fnlen = zip->Get16le();
		i32 extralen = zip->Get16le(); // extra field length              2 bytes
		i32 commentlen = zip->Get16le(); // file comment length             2 bytes
		if(zip->Get16le() != 0) // disk number start               2 bytes
			return; // Multiple disks not supported
		zip->Get16le(); // internal file attributes
		zip->Get32le(); // external file attributes
		f.offset = zip->Get32le();
		f.path = zip->Get(fnlen);
		zip->SeekCur(extralen + commentlen);
		if(zip->IsEof() || zip->IsError())
			return;
	}
	
	error = false;
}

Time UnZip::GetZipTime(u32 dt)
{
	Time time;
	time.year = i16(((dt >> 25) & 0x7f) + 1980);
	time.month = u8((dt >> 21) & 0x0f);
	time.day = u8((dt >> 16) & 0x1f);
	time.hour = u8((dt >> 11) & 0x1f);
	time.minute = u8((dt >> 5) & 0x3f);
	time.second = u8((dt << 1) & 0x3e);
	return time;
}

i64 zPress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress, bool gzip,
             bool compress, u32 *crc, bool hdr);

bool UnZip::ReadFile(Stream& out, Gate<i32, i32> progress)
{
	if(error)
		return false;
	if(IsFolder()) {
		current++;
		return true;
	}
	error = true;
	if(current >= file.GetCount())
		return false;
	const File& f = file[current];
	zip->Seek(f.offset);
	if(zip->Get32le() != 0x04034b50)
		return false;
	zip->Get16le();
	zip->Get16le(); // Skip header, use info from centrall dir
	zip->Get16le();
	zip->Get32le();
	zip->Get32le();
	zip->Get32le();
	zip->Get32le();
	u32 filelen = zip->Get16le();
	u32 extralen = zip->Get16le();
	zip->SeekCur(filelen + extralen);
	u32 crc;
	u32 l;
	if(f.method == 0) {
		Buffer<u8> temp(65536);
		i32 loaded;
		i32 count = f.csize;
		Crc32Stream crc32;
		while(count > 0 && (loaded = zip->Get(temp, (i32)min<i64>(count, 65536))) > 0) {
			out.Put(temp, loaded);
			crc32.Put(temp, loaded);
			count -= loaded;
		}
		if(count > 0)
			return false;
		l = f.csize;
		crc = crc32;
	}
	else
	if(f.method == 8)
		l = (i32)zPress(out, *zip, f.csize, AsGate64(progress), false, false, &crc, false);
	else
		return false;
	if(crc != f.crc || l != f.usize)
		return false;
	current++;
	error = false;
	return true;
}

Txt UnZip::ReadFile(Gate<i32, i32> progress)
{
	TxtStream ss;
	return ReadFile(ss, progress) ? ss.GetResult() : Txt::GetVoid();
}

Txt UnZip::ReadFile(tukk path, Gate<i32, i32> progress)
{
    for(i32 i = 0; i < file.GetCount(); i++)
        if(file[i].path == path) {
            Seek(i);
            return ReadFile(progress);
        }

    return Txt::GetVoid();
}

void UnZip::Create(Stream& _zip)
{
	zip = &_zip;
	ReadDir();
}

UnZip::UnZip(Stream& in)
{
	Create(in);
}

UnZip::UnZip()
{
	error = true;
	zip = NULL;
}

UnZip::~UnZip() {}

}
