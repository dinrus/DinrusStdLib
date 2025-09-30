// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <drx/CtrlLib/CtrlLib.h>

#include "SplitterButton.h"


namespace drx {

SplitterButton::SplitterButton() {
    Add(splitter.SizePos());
    Add(button1.LeftPosZ(80, 10).TopPosZ(30, 40));
    Add(button2.LeftPosZ(80, 10).TopPosZ(30, 40));

    splitter.WhenLayout = OTVET(OnLayout);
    button1.WhenAction = OTVET1(SetButton, 0);
    button2.WhenAction = OTVET1(SetButton, 1);

    movingRight = true;
    buttonWidth = i32(2.5*splitter.GetSplitWidth());
    positionId = 0;
    SetButtonNumber(2);

    splitter.SetPos(5000);
}

SplitterButton& SplitterButton::Horz(Ctrl &left, Ctrl &right) {
    splitter.Horz(left, right);

    SetPositions(0, 5000, 10000);
    SetInitialPositionId(1);
    SetArrows();

    return *this;
}

SplitterButton& SplitterButton::Vert(Ctrl& tp, Ctrl& bottom) {
    splitter.Vert(tp, bottom);

    SetPositions(0, 5000, 10000);
    SetInitialPositionId(1);
    SetArrows();

    return *this;
}

SplitterButton &SplitterButton::SetPositions(const Vec<i32> &_positions) {
    positions = clone(_positions);
    Sort(positions);
    if (positionId >= positions.GetCount() || positionId < 0)
        positionId = 0;

    splitter.SetPos(positions[positionId]);
    return *this;
}

SplitterButton &SplitterButton::SetPositions(i32 pos1) {
    Vec<i32> ps;

    ps << pos1;
    SetPositions(ps);
    return *this;
}

SplitterButton &SplitterButton::SetPositions(i32 pos1, i32 pos2) {
    Vec<i32> ps;

    ps << pos1 << pos2;
    SetPositions(ps);
    return *this;
}

SplitterButton &SplitterButton::SetPositions(i32 pos1, i32 pos2, i32 pos3) {
    Vec<i32> ps;

    ps << pos1 << pos2 << pos3;
    SetPositions(ps);
    return *this;
}

SplitterButton &SplitterButton::SetInitialPositionId(i32 id) {
    positionId = id;
    splitter.SetPos(positions[positionId]);

    SetArrows();

    return *this;
}

void SplitterButton::OnLayout(i32 pos) {
    i32 cwidth, cheight;

    if (splitter.IsVert()) {
        cwidth = GetSize().cy;
        cheight = GetSize().cx;
    } else {
        cwidth = GetSize().cx;
        cheight = GetSize().cy;
    }

    i32 posx = max(0, pos - buttonWidth/2);
    posx = min(cwidth - buttonWidth, posx);
    i32 posy = (2*cheight)/5;
    i32 widthy;
    if (buttonNumber == 1)
        widthy = cheight/5;
    else
        widthy = cheight/8;

    if (splitter.IsVert()) {
        if (buttonNumber == 1)
            button1.SetPos(PosLeft(posy, widthy), PosTop(posx, buttonWidth));
        else {
            button1.SetPos(PosLeft(posy - widthy/2, widthy), PosTop(posx, buttonWidth));
            button2.SetPos(PosLeft(posy + widthy/2, widthy), PosTop(posx, buttonWidth));
        }
    } else {
        if (buttonNumber == 1)
            button1.SetPos(PosLeft(posx, buttonWidth), PosTop(posy, widthy));
        else {
            button1.SetPos(PosLeft(posx, buttonWidth), PosTop(posy - widthy/2, widthy));
            button2.SetPos(PosLeft(posx, buttonWidth), PosTop(posy + widthy/2, widthy));
        }
    }
    WhenAction();
}

void SplitterButton::SetButton(i32 id) {
    ASSERT(positions.GetCount() > 1);

    i32 ps = splitter.GetPos();

    i32 closerPositionId = Null;
    i32 closerPosition = 10000;
    for (i32 i = 0; i < positions.GetCount(); ++i)
        if (abs(positions[i] - ps) < closerPosition) {
            closerPosition = abs(positions[i] - ps);
            closerPositionId = i;
        }

    //bool arrowRight;
    if (buttonNumber == 1) {
        if (movingRight) {
            if (closerPositionId == (positions.GetCount() - 1)) {
                movingRight = false;
                positionId = positions.GetCount() - 2;
            } else
                positionId = closerPositionId + 1;
        } else {
            if (closerPositionId == 0) {
                movingRight = true;
                positionId = 1;
            } else
                positionId = closerPositionId - 1;
        }
    } else {
        if (id == 1) {
            if (positionId < positions.GetCount() - 1)
                positionId++;
        } else {
            if (positionId > 0)
                positionId--;
        }
    }
    splitter.SetPos(positions[positionId]);
    SetArrows();
    WhenAction();
}

void SplitterButton::SetArrows() {
    if (buttonNumber == 1) {
        bool arrowRight = movingRight;
        if (positionId == (positions.GetCount() - 1))
            arrowRight = false;
        if (positionId == 0)
            arrowRight = true;

        if (arrowRight)
            button1.SetImage(splitter.IsVert() ? CtrlImg::smalldown() : CtrlImg::smallright());
        else
            button1.SetImage(splitter.IsVert() ? CtrlImg::smallup() : CtrlImg::smallleft());
    } else {
        button1.SetImage(splitter.IsVert() ? CtrlImg::smallup() : CtrlImg::smallleft());
        button2.SetImage(splitter.IsVert() ? CtrlImg::smalldown() : CtrlImg::smallright());
    }
}
/*
CH_STYLE(BoxOld, Style, StyleDefault) {
    width = Ctrl::HorzLayoutZoom(2);
    vert[0] = horz[0] = SColorFace();
    vert[1] = horz[1] = GUI_GlobalStyle() >= GUISTYLE_XP ? Blend(SColorHighlight, SColorFace)
                                                         : SColorShadow();
    dots = false;
}*/

}