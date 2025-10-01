// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Functions4U_Functions4U_Gui_h_
#define _Functions4U_Functions4U_Gui_h_

#include "Funcs4U.h"

namespace drx {

inline const RGBA *GetPixel(const Image &img, i32 x, i32 y) {
    return &img[y][x];
}

inline RGBA *GetPixel(ImageBuffer &img, i32 x, i32 y) {
    return &img[y][x];
}

inline bool IsValid(const Image &img, i32 x, i32 y) {
    return x >= 0 && y >= 0 && x < img.GetWidth() && y < img.GetHeight();
}

inline bool IsValid(ImageBuffer &img, i32 x, i32 y) {
    return x >= 0 && y >= 0 && x < img.GetWidth() && y < img.GetHeight();
}

template <class T>
inline bool IsValid(const Image &img, T &t) {
    return t.x >= 0 && t.y >= 0 && t.x < img.GetWidth() && t.y < img.GetHeight();
}

template <class T>
inline bool IsValid(ImageBuffer &img, T &t) {
    return t.x >= 0 && t.y >= 0 && t.x < img.GetWidth() && t.y < img.GetHeight();
}

Drawing DrawEquation(const Txt &str);
QtfRichObject QtfEquation(const Txt &str);

Image NativePathIconX(tukk path, bool folder, i32 flags = 0);

class CParserPlus : public CParser {
public:
    explicit CParserPlus(tukk ptr) : CParser(ptr) {}
    Txt ReadIdPlus();
};

class EquationDraw {
public:
    EquationDraw();
    Drawing Exp(CParserPlus& p);

    static Drawing Text(Txt text, bool italic = true, i32 offsetX=0, i32 offsetY=0, double betw = 1);

private:
    Txt ReplaceSymbols(Txt var);
    Drawing Term(CParserPlus& p, bool noBracket = false);
    Txt TermTrig(CParserPlus& p);
    Drawing Mul(CParserPlus& p);

private:
    static Drawing Bracket(Drawing &data);
    static Drawing Sqrt(Drawing &right);
    static Drawing Exponent(Drawing &right);
    static Drawing Der(Drawing &data);
    static Drawing Abs(Drawing &data);
    static Drawing Integral(Drawing &data, Drawing &sub, Drawing &sup);
    static Drawing Summat(Drawing &data, Drawing &sub, Drawing &sup);
    static Drawing Exp(Drawing &data, Drawing &exp);
    static Drawing Function(Txt function, Drawing &content);
    static Drawing Equal(Drawing &left, Drawing &right);
    static Drawing NumDenom(Drawing &num, Drawing &denom);

    static Drawing JoinCenter(Drawing &left, Drawing &right);

    static Drawing Expression(Txt str);

private:
    static Drawing SubSup(Drawing &drwText, Drawing &drwSub, Drawing &drwSup);
    static Drawing SubSup(Txt text, Txt sub, Txt sup);
    static Drawing SubSup(Drawing &drwText, Txt sub, Txt sup);
    static Drawing SubSupInv(Drawing &drwText, Drawing &drwSub, Drawing &drwSup);
    static Drawing SubSupInv(Txt text, Txt sub, Txt sup);
    static Drawing SubSupInv(Drawing &drwText, Txt sub, Txt sup);
    static Drawing JoinFlex(Drawing &left, double betw1, Drawing &right, double betw2);

private:
    VecMap<Txt, Txt> symbols;
};

bool SaveImage(const Image &img, i32 qualityBpp, const Txt &fileName, Txt ext = "");
bool PrintImage(const Image &img, i32 x = 0, i32 y = 0, i32 width = Null, i32 height = Null);
void DrawRectLine(Draw& w, i32 x, i32 y, i32 width, i32 height, i32 lineWidth, const Color &color);
void DrawRectLine(Draw& w, Point &pos, Size &s, i32 lineWidth, const Color &color);
void DrawRectLine(Draw& w, Rect &r, i32 lineWidth, const Color &color);

i32 GetEditWidth(const Txt &str, const Font font);

class ConsoleOutput {
public:
    ConsoleOutput()                 {Init();}
    explicit ConsoleOutput(bool forceWindow) {Init(forceWindow);}
    virtual ~ConsoleOutput();

    bool Init(bool forceWindow = false);

private:
#ifdef TARGET_WIN32
    bool ownConsole;
#endif
};

void ArrayCtrlWhenBar(Bar &menu, ArrayCtrl &array, bool header = true, bool edit = false);
void ArrayCtrlRowCopy(const ArrayCtrl &array, bool header);
void ArrayCtrlRowPaste(ArrayCtrl &array);
void ArrayCtrlRowSelect(ArrayCtrl &array);
Vec<i32> ArrayCtrlSelectedGet(const ArrayCtrl &array);
i32 ArrayCtrlSelectedGetCount(const ArrayCtrl &array);
Vec<Vec<Val>> ArrayCtrlGet(const ArrayCtrl &arr);
void ArrayCtrlSet(ArrayCtrl &array, const Vec<Vec<Val>> &vals, i32 fromRow = 0, i32 fromCol = 0);

template <class T>
T *GetDefinedParentP(Ctrl *ths) {
    while (ths->GetParent() != nullptr) {
        ths = ths->GetParent();
        T *main;
        if ((main = dynamic_cast<T*>(ths)) != nullptr)
            return main;
    }
    return nullptr;
}

template <class T>
T &GetDefinedParent(Ctrl *ths) {
    T *p = GetDefinedParentP<T>(ths);
    if (p)
        return *p;
    NEVER_(t_("Не найден родитель в GetDefinedParent()"));
    throw Exc(t_("Не найден родитель в GetDefinedParent()"));
}

template <class T>
class WithEvents : public T {
public:
    Function<void(Point, dword)> OnMouseEnter;
    Function<void(Point, dword)> OnMouseMove;
    Function<void(Point, dword)> OnLeftDown;
    Function<void(Point, dword)> OnLeftDouble;
    Function<void(Point, dword)> OnRightDown;
    Function<void(Point, dword)> OnRightDouble;
    Function<void(Point, dword)> OnMiddleDown;
    Function<void(Point, dword)> OnMiddleDouble;
    Function<void(Point, i32, dword)> OnMouseWheel;
    Function<void()> OnMouseLeave;
    Function<void()> OnFocus;
    Function<void()> OnLostFocus;

private:
    virtual void MouseEnter(Point p, dword keyflags)    {OnMouseEnter(p, keyflags);}
    virtual void MouseMove(Point p, dword keyflags)     {OnMouseMove(p, keyflags);}
    virtual void LeftDown(Point p, dword keyflags)      {OnLeftDown(p, keyflags);}
    virtual void LeftDouble(Point p, dword keyflags)    {OnLeftDouble(p, keyflags);}
    virtual void RightDown(Point p, dword keyflags)     {OnRightDown(p, keyflags);}
    virtual void RightDouble(Point p, dword keyflags)   {OnRightDouble(p, keyflags);}
    virtual void MiddleDown(Point p, dword keyflags)    {OnMiddleDown(p, keyflags);}
    virtual void MiddleDouble(Point p, dword keyflags)  {OnMiddleDouble(p, keyflags);}
    virtual void MouseWheel(Point p, i32 zdelta, dword keyflags){OnMouseWheel(p, zdelta, keyflags);}
    virtual void MouseLeave()                           {OnMouseLeave();}
    virtual void GotFocus()                             {OnFocus();}
    virtual void LostFocus()                            {OnLostFocus();}
};

}

#endif
