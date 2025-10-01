#ifndef _GridCtrl_GridDisplay_h_
#define _GridCtrl_GridDisplay_h_

#define  IMAGEFILE <drx/GridCtrl/GridCtrl.iml>
#define  IMAGECLASS GridImg
#include <drx/Draw/iml_header.h>

#define BIT(x) (1 << x)
#define BIT_ALL (0xFFFFFFFF)

class GridCtrl;

namespace GD
{
    enum
    {
        CURSOR    = BIT(0),
        SELECT    = BIT(1),
        LIVE      = BIT(2),
        FOCUS     = BIT(3),
        READONLY  = BIT(4),
        FOUND     = BIT(5),
        MARKED    = CURSOR | SELECT | LIVE | READONLY | FOUND,
        HIGHLIGHT = BIT(6),
        EVEN      = BIT(7),
        ODD       = BIT(8),
        LEFT      = BIT(9),
        RIGHT     = BIT(10),
        TOP       = BIT(11),
        BOTTOM    = BIT(12),
        HCENTER   = BIT(13),
        VCENTER   = BIT(14),
        HPOS      = BIT(15),
        VPOS      = BIT(16),
        WRAP      = BIT(17),
        CHAMELEON = BIT(18),
        NOTEXT    = BIT(19),
        BOLDKEY   = BIT(20),
        BOLDVALUE = BIT(21),
        HALIGN    = LEFT | RIGHT | HCENTER,
        VALIGN    = TOP | BOTTOM | VCENTER,
        ALIGN     = HALIGN | VALIGN
    };
}

class GridDisplay
{
    public:

        Image   leftImg;
        Image   rightImg;
        Image   centerImg;
        Image   bgImg;
        Image   hdrhigh;

        bool    reverse_sort_icon;

        i32     align;
        i32     lm, rm, tm, bm;
        i32     theme;

        i32     col, row;

        GridCtrl *parent;

        GridDisplay()
        {
            SetDefault();
        }

        Size real_size;

        void SetDefault();

        virtual ~GridDisplay() {};

        void SetLeftImage(const Image &img)             { leftImg = img;               }
        void SetRightImage(const Image &img)            { rightImg = img;              }
        void SetCenterImage(const Image &img)           { centerImg = img;             }
        void SetBgImage(Image &img)                     { bgImg = img;                 }
        void SetTextAlign(i32 al = GD::TOP | GD::LEFT)  { align = al;                  }
        void SetHorzMargin(i32 left = 4, i32 right = 4) { lm = left; rm = right;       }
        void SetVertMargin(i32 top = 0, i32 bottom = 0) { tm = top;  bm = bottom;      }
        void ReverseSortIcon(bool b = true)             { reverse_sort_icon = b;       }
        void SetTheme(i32 th = 6);
        i32  GetThemeCount()                            { return 7;                    }
        bool IsFixedRow()                               { return row < 0;              }
        bool IsFixedCol()                               { return col < 0;              }

        WTxt GetStdConvertedVal(const Val &v) const;
        i32 GetLinesCount(i32 cx, const wchar* s, const Font& font, bool wrap);

        void DrawText(Draw &w, i32 mx, i32 x, i32 y, i32 cx, i32 cy, i32 align,
                      wtukk s, const Font &font, const Color &fg, const Color &bg,
                      bool found = false, i32 fs = 0, i32 fe = 0, bool wrap = false);

        virtual void Paint(Draw &w, i32 x, i32 y, i32 cx, i32 cy, const Val &val, dword style,
                           Color &fg, Color &bg, Font &fnt, bool found = false, i32 fs = 0, i32 fe = 0);

        virtual void PaintFixed(Draw &w, bool firstx, bool firsty, i32 x, i32 y, i32 cx, i32 cy, const Val &val, dword style, Font &fnt,
                                bool indicator = false, bool moved = false,
                                i32 sortmode = 0, i32 sortcol = -1, i32 sortcnt = 0, bool horizontal = true);

};

struct GridDisplayCtrl : GridDisplay, StaticRect
{};

GridDisplay& StdGridDisplay();

#endif
