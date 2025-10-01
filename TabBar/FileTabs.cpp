#include "TabBar.h"

namespace drx {

Txt FileTabs::GetFileGroup(const Txt &file)
{
    return GetFileDir(file);
}

Txt FileTabs::GetStackId(const Tab &a)
{
    Txt s = a.value;
    return GetFileTitle(s);
}

hash_t FileTabs::GetStackSortOrder(const Tab &a)
{
    Txt s = a.value;
    return GetFileExt(s).GetHashVal();
}

void FileTabs::ComposeTab(Tab& tab, const Font &font, Color ink, i32 style)
{
    if(PaintIcons() && tab.HasIcon())
    {
        tab.AddImage(tab.img);
        tab.AddSpace(TB_SPACEICON);
    }

    WTxt txt = IsTxt(tab.value) ? tab.value : StdConvert().Format(tab.value);
    i32 extpos = txt.ReverseFind('.');
    tab.AddText(extpos >= 0 ? txt.Left(extpos) : txt, font, filecolor);

    if (extpos >= 0) {
        tab.AddText(txt.Right(txt.GetLength() - extpos), font, extcolor);
    }
}

void FileTabs::ComposeStackedTab(Tab& tab, const Tab& stacked_tab, const Font &font, Color ink, i32 style)
{
    tab.AddImage(TabBarImg::STSEP);

    if (stackedicons && tab.HasIcon()) {
        tab.AddImage(style == CTRL_HOT ? stacked_tab.img : (greyedicons ? DisabledImage(stacked_tab.img) : stacked_tab.img))
            .Clickable();
    }
    else {
        WTxt txt = IsTxt(stacked_tab.value) ? stacked_tab.value : StdConvert().Format(stacked_tab.value);
        i32 extpos = txt.ReverseFind('.');

        Color c = (style == CTRL_HOT) ? extcolor : SColorDisabled();
        if (extpos >= 0) {
            tab.AddText(
                txt.Mid(extpos + 1),
                font,
                c
            ).Clickable();
        }
        else {
            tab.AddText("-", font, c).Clickable();
        }
    }
}

Size FileTabs::GetStackedSize(const Tab &t)
{
    if (stackedicons && t.HasIcon())
        return min(t.img.GetSize(), Size(TB_ICON, TB_ICON)) + Size(TB_SPACEICON, 0) + 5;

    WTxt txt = IsTxt(t.value) ? t.value : StdConvert().Format(t.value);
    i32 extpos = txt.ReverseFind('.');
    txt = extpos >= 0 ? txt.Mid(extpos + 1) : "-";
    return GetTextSize(txt, GetStyle().font) + Size(TabBarImg::STSEP().GetSize().cx, 0);
}

void FileTabs::Serialize(Stream& s)
{
    TabBar::Serialize(s);
    if(s.IsLoading() && icons)
    {
        for(i32 i = 0; i < tabs.GetCount(); i++)
            tabs[i].img = NativePathIcon(Txt(tabs[i].value));
    }
}

void FileTabs::AddFile(const WTxt &file, bool make_active)
{
    AddFile(file, NativePathIcon(file.ToTxt()), make_active);
}

void FileTabs::AddFile(const WTxt &file, Image img, bool make_active)
{
    InsertFile(GetCount(), file, img, make_active);
}

void FileTabs::InsertFile(i32 ix, const WTxt &file, bool make_active)
{
    InsertFile(ix, file, NativePathIcon(file.ToTxt()), make_active);
}

void FileTabs::InsertFile(i32 ix, const WTxt &file, Image img, bool make_active)
{
    Txt s = file.ToTxt();
    TabBar::InsertKey(ix, file, GetFName(s), img, GetFileGroup(s), make_active);
}

void FileTabs::AddFiles(const Vec<Txt> &files, bool make_active)
{
    AddFiles(files, Vec<Image>(), make_active);
}

void FileTabs::AddFiles(const Vec<Txt> &files, const Vec<Image> &img, bool make_active)
{
    InsertFiles(GetCount(), files, img, make_active);
}

void FileTabs::InsertFiles(i32 ix, const Vec<Txt> &files, bool make_active)
{
    InsertFiles(ix, files, Vec<Image>(), make_active);
}

void FileTabs::InsertFiles(i32 ix, const Vec<Txt> &files, const Vec<Image> &img, bool make_active)
{
    if (!files.GetCount()) return;
    bool useimg = img.GetCount() == files.GetCount();
    for (i32 i = files.GetCount() - 1; i > 0; i--) {
        TabBar::InsertKey0(ix, files[i].ToWTxt(), GetFName(files[i]),
            useimg ? img[i] : NativePathIcon(files[i]), GetFileGroup(files[i]));
    }
    InsertFile(ix, files[0].ToWTxt(), useimg ? img[0] : NativePathIcon(files[0]), make_active);
}

void FileTabs::RenameFile(const WTxt &from, const WTxt &to, Image icon)
{
    i32 n = FindKey(from);
    if (n >= 0)
        Set(n, to, GetFName(to.ToTxt()), IsNull(icon) ? NativePathIcon(to.ToTxt()) : icon);
}

FileTabs& FileTabs::FileIcons(bool normal, bool stacked, bool stacked_greyedout)
{
    stackedicons = stacked;
    greyedicons = stacked_greyedout;
    TabBar::Icons(normal);
    return *this;
}

Vec<Txt> FileTabs::GetFiles() const
{
    Vec<Txt> files;
    files.SetCount(tabs.GetCount());
    for (i32 i = 0; i < tabs.GetCount(); i++)
        files[i] = tabs[i].key;
    return files;
}

FileTabs& FileTabs::operator<<(const FileTabs &src)
{
    Clear();
    Stacking(false);
    SortGroups(false);
    Grouping(false);
    AddFiles(src.GetFiles(), src.GetIcons(), false);
    CopySettings(src);
    stackedicons = src.stackedicons;
    greyedicons = src.greyedicons;

    if (src.HasCursor())
        SetData(~src);
    sc.SetPos(src.GetScrollPos());
    Refresh();
    return *this;
}

FileTabs::FileTabs() :
stackedicons(false),
greyedicons(true),
filecolor(SColorLabel),
extcolor(IsDark(SColorFace()) ? Blend(White, LtBlue) : LtBlue)
{

}

}
