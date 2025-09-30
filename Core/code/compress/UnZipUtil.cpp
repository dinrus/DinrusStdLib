//#include "../Compress.h"

namespace drx {

bool FileUnZip::Create(tukk name)
{
	if(!zip.Open(name))
		return false;
	UnZip::Create(zip);
	return true;
}

void MemUnZip::Create(ukk ptr, i32 count)
{
	zip.Create(ptr, count);
	UnZip::Create(zip);
}

void TxtUnZip::Create(const Txt& s)
{
	zip.Open(s);
	UnZip::Create(zip);
}

}
