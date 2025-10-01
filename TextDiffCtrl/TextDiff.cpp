#include "TextDiffCtrl.h"

namespace drx {

template <class I>
static i32 CompareGetCount(I a, I b, i32 max_count)
{
	if(max_count <= 0 || *a != *b)
		return 0;
	i32 left;
	for(left = max_count; --left > 0;)
		if(*++a != *++b)
			return max_count - left;
	return max_count;
}

Vec<Txt> GetLineMap(Stream& stream)
{
	Vec<Txt> out;
	i32 emp = 0;
	if(stream.IsOpen())
		while(!stream.IsEof()) {
			Txt s = stream.GetLine();
			tukk p = s, e = s.End(), f = e;
			while(e > p && /*(u8)e[-1] != 9 && */(u8)e[-1] < ' ')
				e--;
			if(e == p)
				emp++;
			else
			{
				while(emp-- > 0)
					out.Add(Null);
				if(e != f)
					s.Trim(i32(e - p));
				out.Add(s);
				emp = 0;
			}
		}
	return out;
}

Vec<Txt> GetFileLineMap(const Txt& path)
{
	FileIn fi(path);
	return GetLineMap(fi);
}

Vec<Txt> GetTxtLineMap(const Txt& s)
{
	TxtStream ss(s);
	return GetLineMap(ss);
}

class TextComparator
{
public:
	TextComparator(const Vec<Txt>& f1, const Vec<Txt>& f2);

	Array<TextSection>    GetSections() const;

private:
	bool                  Find(i32 start1, i32 end1, i32 start2, i32 end2, i32& best_match, i32& best_count) const;
	void                  Split(Array<TextSection>& dest, i32 start1, i32 end1, i32 start2, i32 end2) const;

private:
	Vec<Index<dword>>  hash1;
	Vec<Index<dword>>  hash2;
	const Vec<Txt>& file1;
	const Vec<Txt>& file2;
};

Array<TextSection> CompareLineMaps(const Vec<Txt>& s1, const Vec<Txt>& s2)
{
	return TextComparator(s1, s2).GetSections();
}

static void CalcHash(Vec<Index<dword>>& hash, const Vec<Txt>& file, i32 limit)
{
	{ // 1st row
		Index<dword>& first = hash.Add();
		for(i32 i = 0; i < file.GetCount(); i++)
			first.Add(FoldHash(GetHashVal(file[i])));
	}
	static i32k prime[] =
	{
		3,  5,  7,   11,  13,  17,  19,  21,
		23, 29, 31,  37,  41,  43,  47,  51,
		53, 61, 67,  71,  73,  79,  83,  87,
		89, 97, 101, 103, 107, 109, 113, 117,
	};
	i32k *pp = prime;
	for(i32 l = 1; l < limit; l <<= 1) {
		Index<dword>& nhash = hash.Add();
		const Index<dword>& ohash = hash[hash.GetCount() - 2];
		i32 pri = *pp++;
		i32 t;
		for(t = l; t < ohash.GetCount(); t++)
			nhash.Add(ohash[t - l] + pri * ohash[t]);
		for(t -= l; t < ohash.GetCount(); t++)
			nhash.Add(ohash[t]);
	}
}

TextComparator::TextComparator(const Vec<Txt>& f1, const Vec<Txt>& f2)
: file1(f1), file2(f2)
{
	i32 limit = min(f1.GetCount(), f2.GetCount());
	CalcHash(hash1, f1, limit);
	CalcHash(hash2, f2, limit);
}

static bool CompareSection(const TextSection& ta, const TextSection& tb)
{
	return ta.start1 < tb.start1 || ta.start1 == tb.start1 && ta.start2 < tb.start2;
}

Array<TextSection> TextComparator::GetSections() const
{
	Array<TextSection> output;
	Split(output, 0, file1.GetCount(), 0, file2.GetCount());
	Sort(output, &CompareSection);
	return output;
}

static i32 GetHashLevel(i32 min_count, i32 hash_count)
{
	i32 l = 0;
	hash_count--;
	while(min_count > 1 && l < hash_count)
	{
		min_count >>= 1;
		l++;
	}
	return l;
}

bool TextComparator::Find(i32 start1, i32 end1, i32 start2, i32 end2, i32& best_match, i32& best_count) const
{
	ASSERT(end1 > start1 && end2 > start2);
	bool done = false;
	const Txt *f1 = file1.Begin() + start1;
	i32 len1 = end1 - start1;
	i32 lvl = GetHashLevel(best_count + 1, hash1.GetCount());
	i32 chunk = 1 << lvl;
	i32 last = max(best_count - chunk + 1, 0);
	const Index<dword> *hp1 = &hash1[lvl];
	const Index<dword> *hp2 = &hash2[lvl];
	const dword *h1 = hp1->begin() + start1;

	i32 i = hp2->Find(*h1);
	while(i >= 0)
		if(i + best_count >= end2)
			return done;
		else {
			if(i >= start2 && h1[last] == (*hp2)[i + last]) {
				i32 top = min(len1, end2 - i);
				i32 hc = CompareGetCount(h1, hp2->begin() + i, top) + chunk - 1;
				i32 cnt = CompareGetCount(f1, file2.begin() + i, min(hc, top));
				if(cnt > best_count) {
					best_count = cnt;
					best_match = i;
					done = true;
					last = best_count - chunk + 1;
					if(best_count + 1 >= 2 * chunk)
					{
						lvl = GetHashLevel(best_count + 1, hash1.GetCount());
						chunk = 1 << lvl;
						last = best_count - chunk + 1;
						hp1 = &hash1[lvl];
						hp2 = &hash2[lvl];
						h1 = hp1->begin() + start1;
						i32 oi = i;
						for(i = hp2->Find(*h1); i >= 0 && i <= oi; i = hp2->FindNext(i))
							;
						continue;
					}
				}
			}
			i = hp2->FindNext(i);
		}
	return done;
}

void TextComparator::Split(Array<TextSection>& dest, i32 start1, i32 end1, i32 start2, i32 end2) const
{
	ASSERT(start1 <= end1 && start2 <= end2);
	while(start1 < end1 && start2 < end2) {
		i32 new1 = -1, new2 = -1, count = 0;
		for(i32 i = start1; i + count < end1; i++)
			if(Find(i, end1, start2, end2, new2, count))
				new1 = i;
		if(count == 0)
			break; // no match at all
		ASSERT(new1 >= start1 && new1 + count <= end1);
		ASSERT(new2 >= start2 && new2 + count <= end2);
		dest.Add(TextSection(new1, count, new2, count, true));
		if(new1 - start1 >= end1 - new1 - count) { // head is longer - recurse for tail
			Split(dest, new1 + count, end1, new2 + count, end2);
			end1 = new1;
			end2 = new2;
		}
		else { // tail is longer - recurse for head
			Split(dest, start1, new1, start2, new2);
			start1 = new1 + count;
			start2 = new2 + count;
		}
		ASSERT(start1 <= end1 && start2 <= end2);
	}
	if(start1 < end1 || start2 < end2)
		dest.Add(TextSection(start1, end1 - start1, start2, end2 - start2, false));
}

};
