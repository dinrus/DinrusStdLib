// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Controls4U_SplitterButton_h_
#define _Controls4U_SplitterButton_h_


namespace drx {

class SplitterButton : public StaticRect {
public:
    typedef SplitterButton CLASSNAME;

    SplitterButton();
    virtual ~SplitterButton() {};

    SplitterButton& Horz(Ctrl &left, Ctrl &right);
    SplitterButton& Vert(Ctrl& top, Ctrl& bottom);

    SplitterButton &SetPositions(const Vec<i32> &_positions);
    SplitterButton &SetPositions(i32 pos1);
    SplitterButton &SetPositions(i32 pos1, i32 pos2);
    SplitterButton &SetPositions(i32 pos1, i32 pos2, i32 pos3);
    const Vec<i32> &GetPositions() const {return positions;}

    void SetPos(i32 p)                      {splitter.SetPos(p);}
    i32 GetPos()                            {return splitter.GetPos();}

    SplitterButton &SetInitialPositionId(i32 id);

    SplitterButton &SetButtonNumber(i32 _buttonNumber) {
        ASSERT(_buttonNumber > 0 && _buttonNumber <= 2);
        buttonNumber = _buttonNumber;
        button2.Show(buttonNumber > 1);
        return *this;
    }

    SplitterButton &SetButtonWidth(i32 width) {buttonWidth = width; return *this;}
    void SetButton(i32 id);

private:
    bool movingRight;
    i32 buttonWidth;
    Vec<i32> positions;
    i32 positionId;
    i32 buttonNumber;

    struct SplitterLay : Splitter {
        Callback1<i32> WhenLayout;

        virtual void Layout() {
            Splitter::Layout();
            WhenLayout(PosToClient(GetPos()));
        }
    };
    SplitterLay splitter;

    Button button1, button2;

    void OnLayout(i32 pos);
    void SetArrows();
};

class Box : public StaticRect {
typedef Box CLASSNAME;

public:
    Box &Add(Ctrl &ctrl, i32 row, i32 col) {
        Ctrl::Add(ctrl);

        if (row >= heights.size()) {
            ctrls.SetCount(row+1);
            heights.SetCount(row+1, 1000);
        }
        if (col >= widths.size())
            widths.SetCount(col+1, 1000);

        for (i32 r = 0; r < heights.size(); ++r)
            ctrls[r].SetCount(widths.size(), nullptr);

        ctrls[row][col] = &ctrl;

        Layout();
        Refresh();

        return *this;
    }

    void Clear() {
        ctrls.Clear();
        widths.Clear();
        heights.Clear();

        Layout();
        Refresh();
    }

    i32 GetRows() {return heights.size();}
    i32 GetCols() {return widths.size();}

    Box &SetWidths(const Vec<i32> &t) {
        widths = clone(t);
        Layout();
        Refresh();
        return *this;
    }

    Box &SetWidth(i32 id, i32 width) {
        widths[id] = width;
        Layout();
        Refresh();
        return *this;
    }

    Box &SetHeights(const Vec<i32> &t) {
        heights = clone(t);
        Layout();
        Refresh();
        return *this;
    }

    Box &SetHeight(i32 id, i32 height) {
        heights[id] = height;
        Layout();
        Refresh();
        return *this;
    }

    Function<void(i32 width, i32 height, Vec<i32> &widths)> WhenWidths;
    Function<void(i32 width, i32 height, Vec<i32> &heights)> WhenHeights;

private:
    Array<Array<Ctrl *>> ctrls;
    Vec<i32> widths;
    Vec<i32> heights;

    virtual void Layout() {
        if (widths.size() == 0 || heights.size() == 0)
            return;

        i32 horiz = GetSize().cx;
        i32 vert = GetSize().cy;

        if (horiz == 0 || vert == 0)
            return;

        if (WhenWidths)
            WhenWidths(horiz, vert, widths);

        i32 width = 0;
        for (i32 c = 0; c < widths.size(); ++c)
            width += widths[c];
        double widthratio = double(horiz)/width;

        if (WhenHeights)
            WhenHeights(horiz, vert, heights);

        i32 height = 0;
        for (i32 r = 0; r < heights.size(); ++r)
            height += heights[r];
        double heightratio = double(vert)/height;

        double heightacc = 0;
        for (i32 r = 0; r < heights.size(); ++r) {
            double hh = heights[r]*heightratio;
            double widthacc = 0;
            for (i32 c = 0; c < widths.size(); ++c) {
                double ww = widths[c]*widthratio;
                if (ctrls[r][c])
                    ctrls[r][c]->TopPos(i32(heightacc), i32(hh)).LeftPos(i32(widthacc), i32(ww));
                widthacc += ww;
            }
            heightacc += hh;
        }
    }
};

}

#endif
