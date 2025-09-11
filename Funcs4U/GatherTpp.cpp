// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifdef flagGUI
#include <drx/CtrlLib/CtrlLib.h>
#include <drx/PdfDraw/PdfDraw.h>
//#include <ide/Browser/Browser.h>

#include <drx/Funcs4U/Funcs4U.h>
#include "GatherTpp.h"
#include <drx/Funcs4U/Html/htmld.h>


namespace drx {

static Topic ReadTopic(tukk text)
{
    Topic topic;
    CParser p(text);
    try {
        if(p.Id("topic")) {
            topic.title = p.ReadTxt();
            p.Char(';');
            topic.text = p.GetPtr();
            return topic;
        }
        while(!p.IsEof()) {
            if(p.Id("TITLE")) {
                p.PassChar('(');
                topic.title = p.ReadTxt();
                p.PassChar(')');
            } else if(p.Id("REF")) {
                p.PassChar('(');
                p.ReadTxt();
                p.PassChar(')');
            } else if(p.Id("TOPIC_TEXT")) {
                p.PassChar('(');
                topic.text << p.ReadTxt();
                p.PassChar(')');
            } else if(p.Id("COMPRESSED")) {
                TxtBuf b;
                b.Reserve(1024);
                while(p.IsInt()) {
                    b.Cat(p.ReadInt());
                    p.PassChar(',');
                }
                topic.text = ZDecompress(~b, b.GetLength());
            } else {
                topic.text << p.GetPtr();
                break;
            }
        }
    }
    catch(CParser::Error &e) {
        topic.text = Txt::GetVoid();
        topic.title = e;
    }
    return topic;
}

struct ScanTopicIterator : RichText::Iterator {
    VecMap<Txt, Txt> *reflink;
    Txt link;
    StaticCriticalSection reflink_lock;

    ScanTopicIterator(VecMap<Txt, Txt> *_reflink) : reflink(_reflink) {};
    virtual bool operator()(i32 , const RichPara& para)
    {
        if(!IsNull(para.format.label))
            reflink->Add(para.format.label, link);
        return false;
    }
};

void GatherTpp::GatherRefLinks(tukk upp)
{
    for(FindFile pff(AppendFNameX(upp, "*.*")); pff; pff.Next()) {
        if(pff.IsFolder()) {
            Txt package = pff.GetName();
            Txt pdir = AppendFNameX(upp, package);
            TopicLink tl;
            tl.package = package;
            for(FindFile ff(AppendFNameX(pdir, "*.tpp")); ff; ff.Next()) {
                if(ff.IsFolder()) {
                    Txt group = GetFileTitle(ff.GetName()    );
                    tl.group = group;
                    Txt _dir = AppendFNameX(pdir, ff.GetName());
                    for(FindFile ft(AppendFNameX(dir, "*.tpp")); ft; ft.Next()) {
                        if(ft.IsFile()) {
                            Txt path = AppendFNameX(_dir, ft.GetName());
                            tl.topic = GetFileTitle(ft.GetName());
                            Txt link = TopicLinkTxt(tl);
                            ScanTopicIterator sti(&reflink);
                            sti.link = link;
                            ParseQTF(ReadTopic(LoadFile(path))).Iterate(sti);
                        }
                    }
                }
            }
        }
    }
}

struct GatherLinkIterator : RichText::Iterator {
    VecMap<Txt, Txt> *reflink;
    Index<Txt> link;

    explicit GatherLinkIterator(VecMap<Txt, Txt> *_reflink) : reflink(_reflink) {};
    virtual bool operator()(i32 , const RichPara& para)
    {
        for(i32 i = 0; i < para.GetCount(); i++) {
            Txt l = para[i].format.link;
            if(!IsNull(l)) {
                if(l[0] == ':') {
                    i32 q = reflink->Find(l);
                    if(q < 0)
                        q = reflink->Find(l + "::class");
                    if(q < 0)
                        q = reflink->Find(l + "::struct");
                    if(q < 0)
                        q = reflink->Find(l + "::union");
                    if(q >= 0)
                        l = (*reflink)[q];
                }
                link.FindAdd(Nvl(reflink->Get(l, Null), l));
            }
        }
        return false;
    }
};

Txt GetIndexTopic(Txt file)
{
    Txt topic = GetFileTitle(file);
    Txt folder = GetFileFolder(file);
    Txt topicLocation = GetFileTitle(folder);
    folder = GetUpperFolder(folder);
    topicLocation = GetFileTitle(folder) + "/" + topicLocation;

    return "topic://" + topicLocation + "/" + topic;
}

i32 CharFilterLbl(i32 c)
{
    return IsAlNum(c) ? c : '.';
}

void QtfAsPdf(PdfDraw &pdf, tukk qtf)
{
    RichText txt = ParseQTF(qtf);
    Size page = Size(3968, 6074);
    DRX::Print(pdf, txt, page);
}

HtmlsD RoundFrame(HtmlsD data, Txt border, Color bg)
{
    return HtmlPackedTableD()
                .BgColor(bg).Width(-100)
                .Attr("style", "border-style: solid; border-width: 1px; border-color: #" + border + ";")
            / HtmlLineD() / data;
}

bool ContainsAt(const Txt &source, const Txt &pattern, i32 pos)
{
    return    pos >= 0
           && pos + pattern.GetLength() <= source.GetLength()
           && 0 == memcmp(source.Begin() + pos, pattern.Begin(), pattern.GetLength());
}

bool StartsWith(const Txt &source, const Txt &pattern)
{
    return ContainsAt(source, pattern, 0);
}

bool EndsWith(const Txt &source, const Txt &pattern)
{
    return ContainsAt(source, pattern, source.GetLength() - pattern.GetLength());
}

Txt GatherTpp::QtfAsHtml(tukk qtf, Index<Txt>& css,
                 const VecMap<Txt, Txt>& _links,
                 const VecMap<Txt, Txt>& _labels,
                 const Txt& outdir, const Txt& fn)
{
    return EncodeHtml(ParseQTF(qtf), css, _links, _labels, outdir, fn, Zoom(8, 40), escape, 40);
}

Txt GetText(tukk s)
{
    return GetTopic(s).text;
}

void GatherTpp::ExportPage(i32 i, Txt htmlFolder, Txt _keywords)
{
    Index<Txt> css;
    Txt path = links.GetKey(i);

    Txt qtflangs;
    Txt strlang;

    Txt page = tt[i];
    page = QtfAsHtml(page, css, links, labels, htmlFolder, links[i]);

    Color bg = Color(210, 217, 210);

    HtmlsD html;
    html <<
        HtmlPackedTableD().Width(-100) /
            HtmlLineD().ColSpan(3)  +
        HtmlPackedTableD().Width(-100) / (
            HtmlLineD().ColSpan(3).BgColor(bg).Height(6) / "" +
            HtmlRowD() / (
                HtmlTCellD().Width(-100).BgColor(bg) / (
                    RoundFrame(page , "6E89AE;padding: 10px;", White)
                )
            )
        );

    //Txt topicTitle = tt.GetKey(i);
    Txt pageTitle = tt[i].title;
    if(IsNull(pageTitle))
        pageTitle = title;
/*
    if(StartsWith(topicTitle, "examples$"))
        pageTitle = "Demos / " + pageTitle;
    else if(StartsWith(topicTitle, "reference$"))
        pageTitle = "Examples / " + pageTitle;
*/
    if(pageTitle != title)
        pageTitle << " :: " << title;

    HtmlsD content =
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n" +
        HtmlHeaderD(pageTitle, AsCss(css) +
            "a.l1         { text-decoration:none; font-size: 8pt; font-family: sans-serif; "
                          "font-weight: normal; }\n"
            "a.l1:link    { color:#000000; }\n"
            "a.l1:visited { color:#000080; }\n"
            "a.l1:hover   { color:#9933CC; }\n"
            "a.l1:active  { color:#000000; }\n"
            "a.l2         { text-decoration:none; font-size: 12pt; font-family: sans-serif; "
                          "font-variant: small-caps; }\n"
            "a.l2:link    { color:#0066FF; }\n"
            "a.l2:visited { color:#FF6600; }\n"
            "a.l2:hover   { color:#BC0624; }\n"
            "a.l2:active  { color:#BC0024; }\n",
            "<META NAME=\"keywords\" "
            "CONTENT=\"" + _keywords + "\">"
            "<META name=\"robots\" content=\"index,follow\">"
        )
        .BgColor(bg)
        .Alink(Red).Link(Black).Vlink(Blue)
        / html;
    SaveFile(AppendFNameX(htmlFolder, links[i]), ~content);
}

Txt GatherTpp::TopicFName(tukk topic)
{
    TopicLink tl = ParseTopicLink(topic);
    Txt file = AppendFNameX(dir, tl.group + ".tpp", tl.topic + ".tpp");
    if (FileExists(file))
        return file;

    for (i32 i = 0; i < rootFolders.GetCount(); ++i) {
        if (rootFolders[i] != dir) {
            file = AppendFNameX(rootFolders[i], tl.package, tl.group + ".tpp", tl.topic + ".tpp");
            if (FileExists(file))
                return file;
        }
    }
    return "";
}

Txt TopicFNameHtml(tukk topic)
{
    TopicLink tl = ParseTopicLink(topic);
    return tl.group + "$" + tl.package+ "$" + tl.topic + ".html";
}

Txt ChangeTopicLanguage(const Txt &topic, i32 lang) {
    i32 pos = topic.ReverseFind('$');
    if (pos < 0)
        return "";
    Txt langtxt = ToLower(LNGAsText(lang));
    return topic.Left(pos+1) + langtxt + topic.Mid(pos+1+langtxt.GetCount());
}

Txt GetTopicLanguage(const Txt &topic) {
    i32 pos = topic.ReverseFind('$');
    if (pos < 0)
        return "";
    return topic.Mid(pos+1, 5);
}

Txt GatherTpp::GatherTopics(tukk topic, Txt& _title)
{
    static StaticCriticalSection mapl;
    i32 q;
    INTERLOCKED_(mapl)
        q = tt.Find(topic);
    if(q < 0) {
        Topic p = ReadTopic(LoadFile(TopicFName(topic)));
        _title = p.title;
        Txt t = p;
        if(IsNull(t)) {
            Txt topicEng = ChangeTopicLanguage(topic, LNG_('E','N','U','S'));
            p = ReadTopic(LoadFile(TopicFName(topicEng)));
            Txt _tt = p;
            if(IsNull(_tt))
                return "index.html";
            _title = p.title;
            p.title += " (translated)";
            Txt help = "topic://uppweb/www/contribweb$" + GetTopicLanguage(topic);
            p.text = Txt("{{1f1t0/50b0/50@(240.240.240) [<A2 ") + t_("Эта страница ещё не переведена") +
                    "]. " + "[^" + help + "^ [<A2 " + t_("Хотите перевести её?") + "]]}}&&" + p.text;
        }
        INTERLOCKED_(mapl)
            tt.Add(topic) = p;
        GatherLinkIterator ti(&reflink);
        ParseQTF(t).Iterate(ti);
#ifdef MTC
        CoWork work;
        for(i32 i = 0; i < ti.link.GetCount(); i++)
            work & callback2(sGatherTopics, &tt, ti.link[i]);
#else
        for(i32 i = 0; i < ti.link.GetCount(); i++)
            GatherTopics(ti.link[i]);
#endif
    } else {
        INTERLOCKED_(mapl)
            _title = tt[q].title;
    }
    return TopicFNameHtml(topic);
}


Txt GatherTpp::GatherTopics(tukk topic)
{
    Txt dummy;
    return GatherTopics(topic, dummy);
}

bool GatherTpp::Load(Txt indexFile, Gate2<i32, i32> progress) {
    indexTopic = GetIndexTopic(indexFile);
    for (i32 i = 0; i < rootFolders.GetCount(); ++i) {
        if (progress(i+1, rootFolders.GetCount()))
            return false;
        dir = rootFolders[i];

        if (!DirExists(dir))
            return false;

        GatherRefLinks(dir);

        if (i == 0)
            GatherTopics(indexTopic);
    }
    return true;
}

bool GatherTpp::MakeHtml(tukk folder, Gate2<i32, i32> progress) {
    DeleteFolderDeep(folder);   Sleep(100);
    DirCreate(folder);

    for(i32 i = 0; i < tt.GetCount(); i++) {
        Txt topic = tt.GetKey(i);
        links.Add(topic, topic == indexTopic ? "index.html" :
                         memcmp(topic, "topic://", 8) ? topic : TopicFNameHtml(topic));
    }
    for(i32 i = 0; i < reflink.GetCount(); i++) {
        Txt l = reflink.GetKey(i);
        Txt lbl = Filter(l, CharFilterLbl);
        Txt f = links.Get(reflink[i], Null) + '#' + lbl;
        links.Add(l, f);
        static tukk x[] = { "::struct", "::class", "::union" };
        for(i32 ii = 0; ii < 3; ii++) {
            Txt e = x[ii];
            if(EndsWith(l, e)) {
                links.Add(l.Mid(0, l.GetLength() - e.GetLength()), f);
            }
        }
        labels.Add(l, lbl);
    }

    for(i32 i = 0; i < tt.GetCount(); i++) {
        if (progress(i+1, tt.GetCount()))
            return false;
        ExportPage(i, folder);
    }
    return true;
}

bool GatherTpp::MakePdf(tukk filename, Gate2<i32, i32> progress) {
    PdfDraw pdf;
    for(i32 i = 0; i < tt.GetCount(); i++) {
        if (progress(i32(0.6*(i+1)), tt.GetCount()))
            return false;
        bool dopdf = true;
        for (i32 j = 0; j < i; ++j) {
            if (tt[j].text == tt[i].text) {
                dopdf = false;
                break;
            }
        }
        if (dopdf)
            QtfAsPdf(pdf, tt[i]);
    }
    Txt rawPdf = pdf.Finish();       progress(9, 10);
    SaveFile(filename, rawPdf);         progress(10, 10);
    return true;
}

i32 GatherTpp::FindTopic(const Txt name) {
    return tt.Find(name);
}

Topic &GatherTpp::GetTopic(i32 id) {
    return tt[id];
}

Topic &GatherTpp::AddTopic(const Txt name) {
    return tt.Add(name);
}

Txt GatherTpp::Www(tukk topic, i32 lang, Txt topicLocation) {
    Txt strLang = ToLower(LNGAsText(lang));
    Txt www = GatherTopics(Txt().Cat() << topicLocation << topic << "$" << strLang);
    if (www != "index.html")
        return www;
    return GatherTopics(Txt().Cat() << topicLocation << topic << "$" << "ru-ru");
}

}

#endif