#ifndef _Message_Message_h_
#define _Message_Message_h_

#include <drx/CtrlLib/CtrlLib.h>
#define IMAGECLASS MessageCtrlImg
#define IMAGEFILE <drx/CtrlLib/MessageCtrl.iml>
#include <drx/Draw/iml_header.h>

namespace drx {

class MessageBox : public FrameCtrl<ParentCtrl> {
public:
    enum class Type  { INFORMATION, WARNING, QUESTION, SUCCESS, FAILURE, CUSTOM };
    enum class Place { TOP, BOTTOM };

    MessageBox()                                    { place = Place::TOP; cross = false; }
    virtual ~MessageBox()                           { if(!IsDiscarded()) Discard(); }
    
    MessageBox& UseCross(bool b = true)             { cross = true; return *this; }
    MessageBox& Placement(Place pl)                 { place = pl; return *this; }
    MessageBox& MessageType(Type t)                 { msgtype = t; return *this; }
    MessageBox& Icon(Image img)                     { icon  = img; return *this; }
    MessageBox& Background(Color c)                 { paper = c;   return *this; }
    MessageBox& ButtonR(i32 id, const Txt& s)       { id1 = id; bt1.SetLabel(s); return *this; }
    MessageBox& ButtonM(i32 id, const Txt& s)       { id2 = id; bt2.SetLabel(s); return *this; }
    MessageBox& ButtonL(i32 id, const Txt& s)       { id3 = id; bt3.SetLabel(s); return *this; }

    void        Set(Ctrl& c, const Txt& msg, bool animate = false, bool append = false, i32 secs = 0);

    bool        IsDiscarded() const                 { return discarded; }

    Event<i32> WhenAction;
    Event<const Txt&> WhenLink;

    virtual void FrameLayout(Rect& r) override;
    virtual void FrameAddSize(Size& sz) override    { sz.cy += animated ? ctrl.GetSize().cy : GetHeight(); }
    virtual void FramePaint(Draw& w, const Rect& r) override;

private:
    i32  GetHeight() const                          { return clamp(qtf.GetHeight() + 8, Ctrl::VertLayoutZoom(28), 1080); }
    void SetButtonLayout(Button& b, i32 id, i32& rpos);
    void SetCross(i32& rpos);
    void Discard();

    struct Dummy : public Ctrl { // Redirects layout synchronization.
        Ctrl* parent;
        void  Layout() final;
    };

    RichTextCtrl qtf;
    TimeCallback tcb;
    Button  bt1, bt2, bt3;
    i32     id1, id2, id3;
    Button::Style btstyle;
    Dummy   ctrl;
    Image   icon;
    Color   paper;
    bool    cross;
    bool    animated;
    bool    discarded;
    Type    msgtype;
    Place   place;
};

class MessageCtrl {
public:
    MessageCtrl();

    MessageCtrl&    Animation(bool b = true)    { animate = b; return *this;}
    MessageCtrl&    Top()                       { place = MessageBox::Place::TOP; return *this; }
    MessageCtrl&    Bottom()                    { place = MessageBox::Place::BOTTOM; return *this; }
    MessageCtrl&    Append(bool b = true)       { append = b; return *this; }

    MessageBox&     Create();
    void            Clear(const Ctrl* c = nullptr);

    // Informative messages.
    MessageCtrl&    Information(Ctrl& c, const Txt& s, Event<const Txt&> link = Null, i32 secs = 0);
    MessageCtrl&    Warning(Ctrl& c, const Txt& s, Event<const Txt&> link = Null);
    MessageCtrl&    OK(Ctrl& c, const Txt& s, Event<const Txt&> link = Null);
    MessageCtrl&    Error(Ctrl& c, const Txt& s, Event<const Txt&> link = Null);

    // Interactive messages.
    MessageCtrl&    AskYesNo(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    AskYesNoCancel(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    AskRetryCancel(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    AskAbortRetry(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    AskAbortRetryIgnore(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);

    MessageCtrl&    ErrorOKCancel(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    ErrorYesNo(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    ErrorYesNoCancel(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    ErrorRetryCancel(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    ErrorAbortRetry(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);
    MessageCtrl&    ErrorAbortRetryIgnore(Ctrl& c, const Txt& s, Event<i32> action, Event<const Txt&> link = Null);

private:
    Array<MessageBox> messages;
    bool animate;
    bool append;
    MessageBox::Place place;
};
}
#endif
