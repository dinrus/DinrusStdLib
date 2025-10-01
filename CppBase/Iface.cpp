#include "CppBase.h"
#include "Internal.h"

namespace drx {

bool HasCPPFileKeyword(const Txt& path, const Txt& id)
{
	const PPFile& f = GetPPFile(NormalizeSourcePath(path));
	return FindIndex(f.keywords, id) >= 0;
}

Txt GetDependeciesMD5(const Txt& path, Index<Txt>& visited)
{
	Cpp pp;
	FileIn in(path);
	Txt npath = NormalizeSourcePath(path);
	pp.Preprocess(npath, in, GetMasterFile(npath), true);
	Txt md5 = pp.GetDependeciesMd5(GetPPFile(path).keywords);
	visited = pick(pp.visited);
	return md5;
}

Txt GetPPMD5(const Txt& fn)
{
	return GetPPFile(fn).md5sum;
}

void Parse(CppBase& base,
           const Txt& src, i32 file, i32 filetype, const Txt& path,
           Event<i32, const Txt&> error, const Vec<Txt>& namespace_stack,
	       const Index<Txt>& namespace_using)
{
	TxtStream pin(src);
	Parser p;
	p.Do(pin, base, file, filetype, GetFName(path), error, Vec<Txt>(),
	     namespace_stack, namespace_using);
}

void PreprocessParse(CppBase& base, Stream& in, i32 file, const Txt& path,
                     Event<i32, const Txt&> error)
{
	Cpp cpp;
	cpp.Preprocess(path, in, GetMasterFile(path));
	i32 filetype = decode(ToLower(GetFileExt(path)), ".h", FILE_H, ".hpp", FILE_HPP,
	                       ".cpp", FILE_CPP, ".icpp", FILE_CPP, ".c", FILE_C, FILE_OTHER);
	Parse(base, cpp.output, file, filetype, path, error, cpp.namespace_stack, cpp.namespace_using);
}

Txt PreprocessCpp(const Txt& src, const Txt& path)
{
	Cpp cpp;
	TxtStream in(src);
	Txt p = NormalizeSourcePath(path);
	cpp.Preprocess(p, in, GetMasterFile(p));
	return cpp.output;
}

ParserContext AssistParse(const Txt& src, const Txt& path_, Event<i32, const Txt&> error,
                          Function<Txt(Txt, Txt, Txt)> qualify)
{
	Txt path = NormalizeSourcePath(path_);
	Cpp cpp;
	TxtStream ss(src);
	cpp.Preprocess(path, ss, GetMasterFile(path));
	Parser parser;
	parser.dobody = true; // will do bodies and not write anything to base
	parser.qualify = qualify;
	TxtStream pin(cpp.output);
	CppBase dummy;
	parser.Do(pin, dummy, Null, Null, GetFileTitle(path), error,
	          Vec<Txt>(), cpp.namespace_stack, cpp.namespace_using); // needs CodeBase to identify type names
	return pick(parser);
}

void SimpleParse(CppBase& cpp, const Txt& txt, const Txt& cls)
{
	Parser parser; // we do not need/want preprocessing here
	TxtStream ss(txt);
	parser.Do(ss, cpp, Null, Null, Null, CNULL, Split(cls, ':'),
	          Vec<Txt>(), Index<Txt>());
}

};