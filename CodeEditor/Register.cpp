#include "CodeEditor.h"

namespace drx {

ArrayMap<Txt, EditorSyntax::SyntaxDef>& EditorSyntax::defs()
{
	static ArrayMap<Txt, SyntaxDef> d;
	return d;
}

void EditorSyntax::Register(tukk id, Event<One<EditorSyntax>&> factory,
                            tukk exts, tukk description)
{
	SyntaxDef& f = defs().GetAdd(id);
	f.factory = factory;
	f.patterns = exts;
	f.description = description;
}

One<EditorSyntax> EditorSyntax::Create(tukk id)
{
	One<EditorSyntax> s;
	SyntaxDef *f = defs().FindPtr(id);
	if(f)
		f->factory(s);
	if(!s)
		s.Create();
	return s;
}

Txt EditorSyntax::GetSyntaxForFilename(tukk fn)
{
	ArrayMap<Txt, SyntaxDef>& d = defs();
	for(i32 i = 0; i < d.GetCount(); i++)
		if(PatternMatchMulti(d[i].patterns, fn))
			return d.GetKey(i);
	return Null;
}

Txt EditorSyntax::GetSyntaxDescription(i32 i)
{
	Txt d = defs()[i].description;
	Txt x = defs()[i].patterns;
	if(x.GetCount()) {
		x.Replace("*", "");
		d << " (" << TrimBoth(x) << ")";
	}
	return d;
}

}
