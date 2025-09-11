// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <drx/CtrlLib/CtrlLib.h>
#include "Controls4U.h"

#define TFILE <Controls4U/Controls4U.t>
#include <drx/Core/t.h>

#define IMAGECLASS Controls4UImg
#define IMAGEFILE <Controls4U/Controls4U.iml>
#include <drx/Draw/iml.h>

#define TOPICFILE <Controls4U/src.tpp/all.i>
#include <drx/Core/topic_group.h>


namespace drx {

void PaintCenterText(Painter &w, double x, double y, Txt text, Font fnt, Color color) {
    Size sz = GetTextSize(text, fnt);
    w.Text(x - sz.cx / 2., y - sz.cy / 2., text, fnt).Fill(color);
}

void PaintArc(Painter &w, double cx, double cy, double R, double ang0, double ang1,
                i32 direction, double width, Color color) {
    if (direction == -1) {
        double c = ang0;
        ang0 = ang1;
        ang1 = c;
    }
    ang0 = ToRad(ang0);
    ang1 = ToRad(ang1);
    double delta = ToRad(0.5);
    if (ang0 > ang1)
        ang1 += 2*M_PI;

    double x1, y1;
    x1 = cx + R*cos(ang0 + delta);
    y1 = cy - R*sin(ang0 + delta);
    w.Move(x1, y1).Arc(cx, cy, R, R, -ang0, ang0-ang1).Stroke(width, color);
}

void EditFileFolder::Init() {
    WhenEnter = OTVET1(DoGo, true);
    butBrowseLeft.SetImage(Controls4UImg::Folder());
    butBrowseLeft.Tip(t_("Нажмите, чтобы открыть выборку файлов"));
    butBrowseLeft <<= OTVET(DoBrowse);
    AddFrame(butBrowseRight);
    butBrowseRight.SetImage(Controls4UImg::Folder());
    butBrowseRight <<= OTVET(DoBrowse);
    butBrowseRight.Width(40);
    butLeft.SetImage(CtrlImg::SmallLeft());
    butLeft <<= OTVET(DoLeft);
    butLeft.Tip(t_("К предыдущему"));
    butLeft.Enable(false);
    butRight.SetImage(CtrlImg::SmallRight());
    butRight <<= OTVET(DoRight);
    butRight.Tip(t_("К следующему"));
    butRight.Enable(false);
    butUp.SetImage(CtrlImg::DirUp());//SmallUp());
    butUp <<= OTVET(DoUp);
    butUp.Tip(t_("На директорию выше"));
    butUp.Enable(false);
    butGo.SetImage(CtrlImg::SmallRight());
    butGo <<= OTVET1(DoGo, true);
    butFolder.Width(10).Tip(t_("Открыть директорию файла"));
    InsertFrame(1, butFolder);
    isLoad = true;
    histInd = -1;
//  pfs = 0;
    Dropping(false);
}
/*
void EditFileFolder::InitFs() {
    if (pfs)
        return;
    pfs = new FileSel_();
    pfs->NoExeIcons();
    pfs->Asking(!isLoad);
}*/

EditFileFolder::~EditFileFolder() {
//  if (pfs)
//      delete pfs;
}

EditFileFolder &EditFileFolder::UseOpenFolder(bool use) {
    butFolder.Show(use);
    return *this;
}

EditFileFolder &EditFileFolder::UseHistory(bool use) {
    if (use) {
        if (EditTxt::FindFrame(butLeft) == -1) {
            EditTxt::InsertFrame(0, butRight);
            EditTxt::InsertFrame(0, butLeft);
        }
    } else {
        EditTxt::RemoveFrame(butLeft);
        EditTxt::RemoveFrame(butRight);
    }
    return *this;
}

EditFileFolder &EditFileFolder::UseUp(bool use) {
    if (use) {
        if (EditTxt::FindFrame(butUp) == -1) {
            i32 _pos = EditTxt::FindFrame(butRight);
            EditTxt::InsertFrame(_pos+1, butUp);
        }
    } else
        EditTxt::RemoveFrame(butUp);
    return *this;
}

EditFileFolder &EditFileFolder::UseBrowse(bool use) {
    if (use) {
        if (EditTxt::FindFrame(butBrowseLeft) == -1) {
            i32 _pos = EditTxt::FindFrame(butUp);
            if (_pos == -1)
                _pos = EditTxt::FindFrame(butRight);
            EditTxt::InsertFrame(_pos+1, butBrowseLeft);
        }
    } else
        EditTxt::RemoveFrame(butBrowseLeft);
    return *this;
}

EditFileFolder &EditFileFolder::UseBrowseRight(bool use) {
    if (use) {
        if (EditTxt::FindFrame(butBrowseRight) == -1) {
            i32 _pos = EditTxt::FindFrame(butRight);
            EditTxt::InsertFrame(_pos+1, butBrowseRight);
        }
    } else
        EditTxt::RemoveFrame(butBrowseRight);
    return *this;
}

EditFileFolder &EditFileFolder::UseGo(bool use) {
    if (use) {
        if (EditTxt::FindFrame(butGo) == -1) {
            i32 _pos = EditTxt::FindFrame(butRight);
            EditTxt::InsertFrame(_pos+1, butGo);
        }
    } else
        EditTxt::RemoveFrame(butGo);
    return *this;
}

void EditFileFolder::DoBrowse() {
//  InitFs();
    FileSel_ fs;

    Txt s = GetData();
    fs.Set(s);

    SetFileSel(fs);

    if (!s.IsEmpty()) {
        if (DirExists(AppendFNameX(fs.GetBaseDir(), s)))
            fs.PreSelect(s);
        else {
            Txt folder = GetFileFolder(s);
            if (folder.IsEmpty() || folder.Find("..") >= 0 ||
                !DirExists(AppendFNameX(fs.GetBaseDir(), folder)))
                s = AppendFNameX(NormalizePath(folder, fs.GetActiveDir()), GetFName(s));
            fs.PreSelect(s);
        }
    }
    if (isFile && isLoad) {
        if (fs.ExecuteOpen(title))
            SetData(~fs);
    } else if (isFile && !isLoad)  {
        if (fs.ExecuteSaveAs(title))
            SetData(~fs);
    } else if (!isFile) {
        fs.ClearFiles();
        if (fs.ExecuteSelectDir(title))
            SetData(~fs);
    }
}

void EditFileFolder::SetData(const Val& data) {
    EditTxt::SetData(data);
    DoGo(true);
}

void EditFileFolder::DoGo(bool add) {
//  InitFs();
//  FileSel_ fs;

    Txt path = GetData();
/*  fs.Set(path);
    SetFileSel(fs);*/

    if (!IsRootFolder(path))
        butUp.Enable(true);
    else
        butUp.Enable(false);
    if (add) {
        histInd++;
        history.SetCount(histInd);
        history.Add(path);
        if (histInd > 0)
            butLeft.Enable(true);
        if (histInd >= history.GetCount()-1)
            butRight.Enable(false);
    }
    if (WhenChange || WhenAction) {
        if (isFile && !FileExists(path) && DirExists(path)) {
            DoBrowse();
            return;
        }
    }
    if (WhenChange) {
        if (WhenChange()) {
            AddHistory(path);
            Accept();
        }
    } else
        WhenAction();
}

void EditFileFolder::AddHistory(Txt path, i32 max) {
    if (path.IsEmpty())
        WithDropChoice::AddHistory(max);
    else
        select.AddHistory(path, max);
}

void EditFileFolder::ClearHistory() {
    select.Clear();
}

void EditFileFolder::DoLeft() {
    histInd--;
    if (histInd < 0) {
        histInd = 0;
        return;
    } else if (histInd == 0)
        butLeft.Enable(false);
    butRight.Enable(true);
    EditTxt::SetData(history[histInd]);
    DoGo(false);
}

void EditFileFolder::DoRight() {
    histInd++;
    if (histInd >= history.GetCount()-1)
        butRight.Enable(false);
    butLeft.Enable(true);
    EditTxt::SetData(history[histInd]);
    DoGo(false);
}

void EditFileFolder::DoUp() {
    Txt folder = GetData();
    folder = GetUpperFolder(folder);
    SetData(folder);
}

EditFile::EditFile() {
    isFile = true;
    title = t_("Выбор файла");
    Init();
    butBrowseRight.Tip(t_("Нажмите, чтобы просмотреть файл."));
    butFolder.WhenAction = [&] {
        Txt fileName = GetData();
        Txt folder = GetFileFolder(fileName);
        if (Trim(fileName).IsEmpty())
            Exclamation(t_("Файл не установлен."));
        else if (!DirExists(folder))
            Exclamation(drx::Format(t_("Папки '%s' не существует."), DeQtf(folder)));
        else {
            if (folder.StartsWith("\\")) {
                if (0 == system("explorer " + folder))
                    Exclamation((t_("Не удаётся открыть просмотрщик файлов.")));
            } else
                LaunchWebBrowser(folder);
        }
    };
}

EditFolder::EditFolder() {
    isFile = false;
    title = t_("Выбор папки");
    Init();
    butBrowseRight.Tip(t_("Нажмите, чтобы просмотреть папку"));
    butFolder.WhenAction = [&] {
        Txt folder = GetData();
        if (Trim(folder).IsEmpty())
            Exclamation(t_("Папка не установлена"));
        else if (!DirExists(folder))
            Exclamation(drx::Format(t_("Папки '%s' не существует"), DeQtf(folder)));
        else
            LaunchWebBrowser(folder);};
}

bool SetFirstChild(Ctrl *ctrl) {
    if (Ctrl *p = ctrl->GetParent()) {
        if (p->GetFirstChild() != ctrl) {
            p->RemoveChild(ctrl);
            p->AddChildBefore(ctrl, p->GetFirstChild());
        }
        return true;
    } else
        return false;
}

bool SetLastChild(Ctrl *ctrl) {
    if (Ctrl *p = ctrl->GetParent()) {
        if (p->GetIndexChild(p->GetChildCount()-1) != ctrl) {
            p->RemoveChild(ctrl);
            p->AddChild(ctrl, p->GetIndexChild(p->GetChildCount()-1));
        }
        return true;
    } else
        return false;
}

void ImagePopUp::Paint(Draw &w) {
    Size sz = GetSize();
    Size imagesize = image.GetSize();

    switch (fit) {
    case StaticImage::BestFit: {
        Rect _rect = FitInFrame(sz, imagesize);
        sz = _rect.GetSize();
        w.DrawImage(sz, image);
        break; }
    case StaticImage::FillFrame:
        w.DrawImage(0, 0, sz.cx, sz.cy, image);
        break;
    case StaticImage::NoScale:
        w.DrawImage(0, 0, image);
        break;
    case StaticImage::RepeatToFill:
        for (i32 left = 0; left < sz.cx; left += imagesize.cx)
            for (i32 _top = 0; _top < sz.cy; _top += imagesize.cy)
                w.DrawImage(left, _top, image);
        break;
    }

    DrawBorder(w, sz, BlackBorder);
}

Point ImagePopUp::Offset(Point p) {
    return p + GetScreenView().TopLeft() - ctrl->GetScreenView().TopLeft();
}

void ImagePopUp::LeftDown(Point p, dword flags) {
    ctrl->LeftDown(Offset(p), flags);
}

void ImagePopUp::LeftDrag(Point p, dword flags) {
    Close();
    ctrl->LeftDrag(Offset(p), flags);
}

void ImagePopUp::LeftDouble(Point p, dword flags) {
    ctrl->LeftDouble(Offset(p), flags);
}

void ImagePopUp::RightDown(Point p, dword flags) {
    ctrl->RightDown(Offset(p), flags);
}

void ImagePopUp::LeftUp(Point p, dword flags) {
    ctrl->LeftUp(Offset(p), flags);
}

void ImagePopUp::MouseWheel(Point p, i32 zdelta, dword flags) {
    ctrl->MouseWheel(Offset(p), zdelta, flags);
}

void ImagePopUp::MouseLeave() {
    ctrl->MouseLeave();
    Close();
}

void ImagePopUp::MouseEnter(Point p, dword flags) {
    ctrl->MouseEnter(Offset(p), flags);
}

void ImagePopUp::MouseMove(Point p, dword flags) {
    ctrl->MouseMove(Offset(p), flags);
}

Image ImagePopUp::CursorImage(Point p, dword flags) {
    return ctrl->CursorImage(Offset(p), flags);
}

void ImagePopUp::LostFocus() {
    Close();
}

void ImagePopUp::PopUp(Ctrl *owner, i32 x, i32 y, i32 width, i32 height, Image &_image, i32 _angle, i32 _fit) {
    if (width == 0 || height == 0 || IsNull(_image))
        return;

    Size imagesize = _image.GetSize();
    if (imagesize.cx == 0 || imagesize.cy == 0)
        return;

    Rect r(x, y, x + width, y + height);
    if (fit == StaticImage::BestFit) {
        r = FitInFrame(r.GetSize(), imagesize);
        Size sz = r.GetSize();
        r.left = x;
        r.top = y;
        r.SetSize(sz);
    }
    Size ssz = GetScreenSize();
    if (ssz.cx < r.left + r.GetWidth()) {
        r.left = ssz.cx - r.GetWidth();
        r.right = ssz.cx;
    }
    if (ssz.cy < r.top + r.GetHeight()) {
        r.top = ssz.cy - r.GetHeight();
        r.bottom = ssz.cy;
    }
    if(r != GetRect())
        SetRect(r);

    ctrl = owner;
    image = drx::GetRect(_image, _image.GetSize());
    angle = _angle;
    fit = _fit;
    Ctrl::PopUp(owner, true, false, GUI_DropShadows());
    SetAlpha(230);
}

void ImagePopUp::Close() {
    Ctrl::Close();
}

void StaticImage::MouseEnter(Point, dword) {
    if (isPopUp) {
        Point pt = GetScreenRect().TopLeft();
        popup.PopUp(this, pt.x, pt.y, szPopUp.cx, szPopUp.cy, origImage, angle, fit);
    }
}

void StaticImage::MouseLeave() {
    if (isPopUp)
        popup.Close();
}

Image StaticImage::CursorImage(Point, dword) {
    if (!hyperlink.IsEmpty())
        return Image::Hand();
    return Image::Arrow();
}

void StaticImage::Layout() {
    if (useAsBackground) {
        SetLastChild(static_cast<Ctrl *>(this));
        //SizePos();        // To be fixed
    }
}

StaticImage& StaticImage::SetPopUpSize(Size sz) {
    if (!IsNull(sz))
        szPopUp = sz;
    else
        szPopUp = origImage.GetSize();
    return *this;
}

void StaticImage::Paint(Draw& w) {
    Size sz = GetSize();
    if (sz.cx == 0 || sz.cy == 0)
        return;

    w.DrawRect(sz, background);

    Image image, imageGet, *imageView;

    if (!origImage) {
        if (GetImage) {
            imageGet = GetImage();
            if (!imageGet)
                return;
        }
    }
    imageView = origImage ? &origImage : &imageGet;

    if (angle != StaticImage::Angle_0) {
        switch (angle) {
        case Angle_90:  image = RotateClockwise(*imageView);
                        break;
        case Angle_180: image = Rotate180(*imageView);
                        break;
        case Angle_270: image = RotateAntiClockwise(*imageView);
                        break;
        }
        imageView = &image;
    }
    Size imagesize = imageView->GetSize();
    if (imagesize.cx == 0 || imagesize.cy == 0)
        return;

    switch (fit) {
    case StaticImage::BestFit:
        w.DrawImage(FitInFrame(sz, imagesize), *imageView);
        break;
    case StaticImage::FillFrame:
        w.DrawImage(0, 0, sz.cx, sz.cy, *imageView);
        break;
    case StaticImage::NoScale:
        w.DrawImage(0, 0, *imageView);
        break;
    case StaticImage::RepeatToFill:
        for (i32 left = 0; left < sz.cx; left += imagesize.cx)
            for (i32 _top = 0; _top < sz.cy; _top += imagesize.cy)
                w.DrawImage(left, _top, *imageView);
        break;
    }
}

bool StaticImage::Set(Txt _fileName) {
    fileName = _fileName;
    origImage.Clear();
    origImage = StreamRaster::LoadFileAny(_fileName);

    if (angle != Angle_0)
        SetAngle(angle);
    Refresh();
    return !IsNull(origImage);
}

bool StaticImage::Set(Image _image) {
    origImage.Clear();
    origImage = _image;
    if (angle != Angle_0)
        SetAngle(angle);
    fileName = "";
    Refresh();
    return !IsNull(origImage);
}

StaticImage& StaticImage::SetAngle(i32 _angle) {
    angle = _angle;
    Refresh();
    return *this;
}

void  StaticImage::RightDown(Point , dword ) {
    if(!IsEditable())
        return;

    WhenRightDown();
}

void  StaticImage::LeftDown(Point , dword ) {
    if(!IsEditable())
        return;

    if (!hyperlink.IsEmpty())
        LaunchWebBrowser(hyperlink);

    WhenLeftDown();
}

void  StaticImage::LeftDouble(Point , dword ) {
    if(!IsEditable())
        return;

    WhenLeftDouble();
}


StaticImage::StaticImage() {
    Transparent();
    NoWantFocus();

    origImage = Null;
    background = Null;
    angle = Angle_0;
    fit = BestFit;
    useAsBackground = false;
    isPopUp = false;
    szPopUp = Size(300, 300);
}


StaticImageSet::StaticImageSet() {
    Transparent();
//  NoWantFocus();

    background = Null;
    id = -1;
}

void StaticImageSet::Paint(Draw& w) {
    Size sz = GetSize();

    w.DrawRect(sz, background);
    if (sz.cx == 0 || sz.cy == 0)
        return;
    if (id < 0)
        return;

    Size imagesize = images[id].GetSize();
    if (imagesize.cx == 0 || imagesize.cy == 0)
        return;

    w.DrawImage(FitInFrame(sz, imagesize), images[id]);
}

void StaticImageSet::LeftDown(Point , dword) {
    if(!IsEditable())
        return;
    SetWantFocus();

    Next();

    SetCapture();
    Refresh();
}

void StaticImageSet::LeftRepeat(Point _pos, dword keyflags) {
    if(!HasCapture())
        LeftDown(_pos, keyflags);
}

void StaticImageSet::LeftUp(Point, dword) {
    if (!HasCapture())
        return;

    Refresh();
}

void StaticImageSet::MouseMove(Point , dword ) {
    if(!HasCapture())
        return;
}

bool  StaticImageSet::Add(Txt fileName) {
    return Add(StreamRaster::LoadFileAny(fileName));
}

bool  StaticImageSet::Add(Image image) {
    if (IsNull(image))
        return false;
    images.Add(image);
    return true;
}


void StaticRectangle::Paint(Draw& w) {
    Size sz = GetSize();
    ImageBuffer ib(sz);
    BufferPainter sw(ib);

    sw.Clear(RGBAZero());
    double wid;
    if (background)
        wid = width*2;
    else
        wid = width;
    double sheight;
    if (isSquare)
        sheight = sz.cx-wid;
    else
        sheight = sz.cy-wid;

    sw.Rectangle(wid/2, wid/2, sz.cx-wid, sheight).Stroke(wid, color).Fill(background);

    w.DrawImage(0, 0, ib);
}

void StaticRectangle::Layout() {
    if (isSquare) {
        Rect r = GetRect();
        SetRect(r.left, r.top, r.GetWidth(), r.GetWidth());
    }
}

StaticRectangle::StaticRectangle() {
    Transparent();
    NoWantFocus();

    color = Black();
    background = Null;
    width = 1;
    isSquare = false;
}

void StaticEllipse::Paint(Draw& w) {
    Size sz = GetSize();
    ImageBuffer ib(sz);
    BufferPainter sw(ib);

    sw.Clear(RGBAZero());
    double wid;
    if (background)
        wid = width*2;
    else
        wid = width;
    sw.Ellipse(sz.cx/2, sz.cy/2, sz.cx/2-wid/2, sz.cy/2-wid/2).Stroke(wid, color).Fill(background);

    w.DrawImage(0, 0, ib);
}

StaticEllipse::StaticEllipse() {
    Transparent();
    NoWantFocus();

    color = Black();
    background = Null;
    width = 1;
}

StaticFrame::StaticFrame() {
    Transparent();
    NoWantFocus();

    SetFrame(InsetFrame());
}

void StaticFrame::Paint(Draw& w) {
    Size sz = GetSize();

    if (background)
        w.DrawRect(0, 0, sz.cx, sz.cy, background);
}

void StaticLine::FramePaint(Draw& w, const Rect& rr) {
    i32 off = i32(0.25*max(rr.GetWidth(), rr.GetHeight()));

    ImageBuffer ib(rr.GetWidth()+2*off, rr.GetHeight()+2*off);
    BufferPainter sw(ib);
    Rect r = rr;
    r.Offset(off, off);

    sw.Clear(RGBAZero());
    sw.LineCap(LINECAP_BUTT);

    if (orientation == OrVert)
        sw.DrawLine((r.right+r.left)/2, r.top, (r.right+r.left)/2, r.bottom, width, color);
    else if (orientation == OrHor)
        sw.DrawLine(r.left, (r.bottom+r.top)/2, r.right, (r.bottom+r.top)/2, width, color);
    else if (orientation == OrSW_NE)
        sw.DrawLine(r.left, r.bottom, r.right, r.top, width, color);
    else
        sw.DrawLine(r.left, r.top, r.right, r.bottom, width, color);

    w.DrawImage(-off, -off, ib);
}

StaticLine& StaticLine::SetOrientation(Txt o) {
    if (o == "|")
        return SetOrientation(OrVert);
    else if (o == "-")
        return SetOrientation(OrHor);
    else if (o == "\\")
        return SetOrientation(OrNW_SE);
    else
        return SetOrientation(OrSW_NE);
}

StaticLine::StaticLine() {
    SetFrame(*this);
    Transparent();
    NoWantFocus();

    color = Black();
    width = 1;
    orientation = OrVert;
}

void PaintArrowEnd(Painter &sw, double x0, double y0, double x1, double y1, i32 width, Color &color, bool direction) {
    double alen, awidth;
    double wd = width;

    if (width == 1) {
        alen = 16;
        awidth = 2;
    } else {
        alen = 9*wd;
        awidth = 1.5*wd;
    }
    if (!direction)
        alen = -alen;
    if (x0 == x1) {
        sw.Move(x0, y0).Line(x0 + awidth, y0 + alen).Line(x0 - awidth, y0 + alen).Line(x0, y0);
        sw.Fill(color);
    } else if (y0 == y1) {
        sw.Move(x0, y0).Line(x0 + alen, y0 - awidth).Line(x0 + alen, y0 + awidth).Line(x0, y0);
        sw.Fill(color);
    } else {
        double t = atan((y1-y0)/static_cast<double>(x1-x0));
        double xa = alen*cos(t);
        double ya = alen*sin(t);
        double xb = awidth*sin(t);
        double yb = awidth*cos(t);
        sw.Move(x0, y0).Line(x0+xa+xb, y0+ya-yb).Line(x0+xa-xb, y0+ya+yb).Line(x0, y0);
        sw.Fill(color);
    }
}

void StaticArrow::FramePaint(Draw& w, const Rect& rr) {
    i32 off = i32(0.25*max(rr.GetWidth(), rr.GetHeight()));

    ImageBuffer ib(rr.GetWidth()+2*off, rr.GetHeight()+2*off);
    BufferPainter sw(ib);
    Rect r = rr;
    r.Offset(off, off);

    //Size sz = rr.GetSize();

    sw.Clear(RGBAZero());
    sw.LineCap(LINECAP_BUTT);

    double x0, y0, x1, y1, middle;

    if (orientation == OrVert) {
        x0 = x1 = (r.right+r.left)/2;
        y0 = r.top;
        y1 = r.bottom;
    } else if (orientation == OrHor) {
        x0 = r.left;
        y0 = y1 = (r.bottom+r.top)/2;
        x1 = r.right;
    } else if (orientation == OrSW_NE) {
        x0 = r.left;
        y0 = r.bottom;
        x1 = r.right;
        y1 = r.top;
    } else if (orientation == OrNW_SE_HVH) {
        x0 = r.left;
        y0 = r.top + 2*width;
        x1 = r.right;
        y1 = r.bottom - 2*width;
        middle = (r.left+r.right)/2;
    } else if (orientation == OrSW_NE_HVH) {
        x0 = r.left;
        y0 = r.bottom - 2*width;
        x1 = r.right;
        y1 = r.top + 2*width;
        middle = (r.left + r.right)/2;
    } else if (orientation == OrNW_SE_VHV) {
        x0 = r.left + 2*width;
        y0 = r.top;
        x1 = r.right - 2*width;
        y1 = r.bottom;
        middle = (r.top + r.bottom)/2;
    } else if (orientation == OrSW_NE_VHV) {
        x1 = r.left  + 2*width;
        y1 = r.bottom;
        x0 = r.right - 2*width;
        y0 = r.top;
        middle = (r.top + r.bottom)/2;
    } else {    // OrNW_SE
        x0 = r.left;
        y0 = r.top;
        x1 = r.right;
        y1 = r.bottom;
    }
    // Arrow len is 9*width, but 8 joins better arrow end with line
    if (ends == EndLeft || ends == EndLeftRight) {
        if (x0 == x1 || orientation == OrVert || orientation == OrNW_SE_VHV || orientation == OrSW_NE_VHV) {
            PaintArrowEnd(sw, x0, y0, x0, y1, width, color, true);
            y0 += 8*width;
        } else if (y0 == y1 || orientation == OrHor || orientation == OrNW_SE_HVH || orientation == OrSW_NE_HVH) {
            PaintArrowEnd(sw, x0, y0, x1, y0, width, color, true);
            x0 += 8*width;
        } else {
            PaintArrowEnd(sw, x0, y0, x1, y1, width, color, true);
            double t = atan((y1-y0)/static_cast<double>(x1-x0));
            x0 += 8*width*cos(t);
            y0 += 8*width*sin(t);
        }
    }
    if (ends == EndRight || ends == EndLeftRight) { // Same as other but swapping points
        if (x0 == x1 || orientation == OrVert || orientation == OrNW_SE_VHV || orientation == OrSW_NE_VHV) {
            PaintArrowEnd(sw, x1, y1, x1, y0, width, color, false);
            y1 -= 8*width;
        } else if (y0 == y1 || orientation == OrHor || orientation == OrNW_SE_HVH || orientation == OrSW_NE_HVH) {
            PaintArrowEnd(sw, x1, y1, x0, y1, width, color, false);
            x1 -= 8*width;
        } else {
            PaintArrowEnd(sw, x1, y1, x0, y0, width, color, false);
            double t = atan((y1-y0)/static_cast<double>(x1-x0));
            x1 -= 8*width*cos(t);
            y1 -= 8*width*sin(t);
        }
    }
    if (orientation == OrVert)
        sw.Move(x0, y0).Line(x1, y1).Stroke(width, color);
    else if (orientation == OrHor)
        sw.Move(x0, y0).Line(x1, y1).Stroke(width, color);
    else if (orientation == OrSW_NE)
        sw.Move(x0, y0).Line(x1, y1).Stroke(width, color);
    else if (orientation == OrNW_SE_HVH) {
        middle = (r.left+r.right)/2;
        sw.Move(x0,     y0).Line(middle, y0).Stroke(width, color);
        sw.Move(middle, y0).Line(middle, y1).Stroke(width, color);
        sw.Move(middle, y1).Line(x1,     y1).Stroke(width, color);
    } else if (orientation == OrSW_NE_HVH) {
        middle = (r.left + r.right)/2;
        sw.Move(x0,     y0).Line(middle, y0).Stroke(width, color);
        sw.Move(middle, y0).Line(middle, y1).Stroke(width, color);
        sw.Move(middle, y1).Line(x1,     y1).Stroke(width, color);
    } else if (orientation == OrNW_SE_VHV) {
        middle = (r.top + r.bottom)/2;
        sw.Move(x0,     y0).Line(x0, middle).Stroke(width, color);
        sw.Move(x0, middle).Line(x1, middle).Stroke(width, color);
        sw.Move(x1, middle).Line(x1,     y1).Stroke(width, color);
    } else if (orientation == OrSW_NE_VHV) {
        middle = (r.top + r.bottom)/2;
        sw.Move(x0,     y0).Line(x0, middle).Stroke(width, color);
        sw.Move(x0, middle).Line(x1, middle).Stroke(width, color);
        sw.Move(x1, middle).Line(x1,     y1).Stroke(width, color);
    } else
        sw.Move(x0, y0).Line(x1, y1).Stroke(width, color);

    w.DrawImage(-off, -off, ib);
}

StaticArrow& StaticArrow::SetOrientation(Txt o) {
    if (o == "|")
        return SetOrientation(OrVert);
    else if (o == "-")
        return SetOrientation(OrHor);
    else if (o == "\\")
        return SetOrientation(OrNW_SE);
    else if (o == "/")
        return SetOrientation(OrSW_NE);
    else if (o == "┐_")
        return SetOrientation(OrNW_SE_HVH);
    else if (o == "_┌")
        return SetOrientation(OrSW_NE_HVH);
    else if (o == "└┐")
        return SetOrientation(OrNW_SE_VHV);
    else //if (o == "┌┘") {
        return SetOrientation(OrSW_NE_VHV);
}

StaticArrow& StaticArrow::SetEnds(Txt e) {
    if (e == "<-")
        return SetEnds(EndLeft);
    else if (e == "->")
        return SetEnds(EndRight);
    else if (e == "<->")
        return SetEnds(EndLeftRight);
    else
        return SetEnds(NoEnd);
}

StaticArrow::StaticArrow() {
    SetFrame(*this);
    Transparent();
    NoWantFocus();

    color = Black();
    width = 1;
    orientation = OrVert;
    ends = EndLeft;
}

void StaticClock::PaintPtr(BufferPainter &w, double cmx, double cmy, double _pos, double m, double d, Color color, double cf) {
    double dx = m * sin(_pos * 2 * M_PI);
    double dy = m * cos(_pos * 2 * M_PI);

    double sx = cmx - dx * 35 / 2.0;
    double sy = cmy + dy * 35 / 2.0;
    double ex = cmx + dx * cf;
    double ey = cmy - dy * cf;

    w.Move(sx, sy).Line(ex, ey).Stroke(d, color);
}

void StaticClock::Paint(Draw& ww) {
    Size sz = GetSize();
    ImageBuffer ib(sz);
    BufferPainter w(ib);

    w.Clear(RGBAZero());
    w.LineCap(LINECAP_BUTT);

    Rect r = GetRect();
    //Rect ro = GetRect();

    Color letterColor, background;;
    if (colorType == WhiteType) {
        background = White();
        letterColor = Black();
    } else {
        background = Black();
        letterColor = White();
    }
    //double hs = 20;
    i32 width = min(r.right - r.left, r.bottom - r.top);
    double bigF = width/200.;
    if (image)
        w.DrawImage(0, 0, width, width, image);
    else
        w.Circle(width/2, width/2, width/2).Fill(background);

    double cmx = width / 2;
    double cmy = width / 2;
    double cf = min(cmy, cmx) - 2;

    if (hourType != StaticClock::No) {
        for(i32 i = 1; i <= 60; i++) {
            double x = cmx + (0.95 * sin(i * M_PI / 30.0) * cf);
            double y = cmy - (0.95 * cos(i * M_PI / 30.0) * cf);
            if (hourType == StaticClock::Square) {
                if(i % 5 == 0)
                    w.Rectangle(x, y, 3*bigF, 3*bigF).Fill(letterColor);
                else
                    w.Rectangle(x, y, 2*bigF, 2*bigF).Fill(letterColor);
            } else if (hourType == StaticClock::Rectangle) {
                if(i % 5 == 0) {
                    double x2 = cmx + (0.7 * sin(i * M_PI / 30.0) * cf);
                    double y2 = cmy - (0.7 * cos(i * M_PI / 30.0) * cf);
                    w.Move(x, y).Line(x2, y2).Stroke(4*bigF, Gray());
                    w.Move(x, y).Line(x2, y2).Stroke(2*bigF, letterColor);
                } else {
                    double x2 = cmx + (0.8 * sin(i * M_PI / 30.0) * cf);
                    double y2 = cmy - (0.8 * cos(i * M_PI / 30.0) * cf);
                    w.Move(x, y).Line(x2, y2).Stroke(1*bigF, Gray());
                }
            }
        }
    }
    if (numberType != StaticClock::NoNumber) {
        double numberPos = 0;
        if (hourType == StaticClock::No)
            numberPos = 1;
        else if (hourType == StaticClock::Square)
            numberPos = 0.96;
        else if (hourType == StaticClock::Rectangle)
            numberPos = 0.75;
        double numberd = numberPos - 0.2;
        Font fnt, fnt4;
        if (numberType == StaticClock::Small)  {
            fnt4 = Arial(static_cast<i32>(14*bigF));
            fnt = Arial(static_cast<i32>(14*bigF));
        } else if (numberType == StaticClock::Big)  {
            fnt4 = Arial(static_cast<i32>(20*bigF));
            fnt = Arial(static_cast<i32>(20*bigF));
        } else if (numberType == StaticClock::BigSmall)  {
            fnt4 = Arial(static_cast<i32>(20*bigF));
            fnt = Arial(static_cast<i32>(14*bigF));
        } else if (numberType == StaticClock::Big4)  {
            fnt4 = Arial(static_cast<i32>(20*bigF));
        }
        for(i32 i = 1; i <= 12; i++) {
            double x = cmx + (numberd * sin(i * M_PI / 6.0) * cf);
            double y = cmy - (numberd * cos(i * M_PI / 6.0) * cf);
            if (i % 3 == 0)
                PaintCenterText(w, x, y, FormatInt(i), fnt4, letterColor);
            else if (numberType != StaticClock::Big4)
                PaintCenterText(w, x, y, FormatInt(i), fnt, letterColor);
        }
    }
    double handleF = bigF;
    double tm = t.hour * 3600 + t.minute * 60 + t.second;
    PaintPtr(w, cmx, cmy, tm / 3600 / 12, 0.5, 4*handleF, Color(200, 200, 200), cf);
    PaintPtr(w, cmx, cmy, tm / 3600 / 12, 0.5, 3*handleF, letterColor, cf);
    PaintPtr(w, cmx, cmy, tm / 3600, 0.6, 3*handleF, Color(200, 200, 200), cf);
    PaintPtr(w, cmx, cmy, tm / 3600, 0.6, 2*handleF, letterColor, cf);
    if (seconds)
        PaintPtr(w, cmx, cmy, tm / 60, 0.75, 1*handleF, Color(200, 200, 200), cf);

    i32 wm = width;
    if (colorType == WhiteType) {
        w.Begin();
            w.BeginMask();
                w.Move(0, 0).Line(wm, 0)
                  .Line(wm, wm).Line(0, wm)
                  .Fill(wm/4, wm/4, Black(), 2*wm, Color(220, 220, 220));
            w.End();
            w.Circle(wm/2, wm/2, wm/2).Fill(Black());
        w.End();
    } else {
        w.Begin();
            w.BeginMask();
                w.Ellipse(wm/2, wm/4, wm/3, wm/4).Fill(Color(60, 60, 60));
            w.End();
            w.Ellipse(wm/2, wm/4, wm/3, wm/4).Fill(White());
        w.End();
    }
    ww.DrawImage(0, 0, ib);
}

StaticClock& StaticClock::SetImage(Txt _fileName) {
    image = StreamRaster::LoadFileAny(_fileName);
    Refresh();
    return *this;
}

void StaticClock::SetData(const Val& v) {
    SetTime(static_cast<Time>(v));
}

void StaticClock::SetTime(const Time& tm) {
    if (!IsNull(tm) && tm.IsValid()) {
        t = tm;
        Refresh();
    }
}

void StaticClock::SetTime(i32 h, i32 n, i32 s) {
    t.hour = h;
    t.minute = n;
    t.second = s;
    Refresh();
}

struct StaticClocks;
void StaticClockThread(StaticClocks *gui);

struct StaticClocks {
    Array <StaticClock *> clocks;
    volatile Atomic running, kill;

    friend void StaticClockThread(StaticClocks *gui);

    StaticClocks() {
        running = kill = 0;
    }
    ~StaticClocks() {
        AtomicInc(kill);
        while (running)
            Sleep(10);
    }
    void Add(StaticClock *clock) {
        clocks.Add(clock);

        if (!running) {
            AtomicInc(running);
            Thread().Run(callback1(StaticClockThread, this));
        }

    }
    void Remove(StaticClock *clock) {
        for (i32 i = 0; i < clocks.GetCount(); ++i) {
            if (clocks[i] == clock) {
                clocks.Remove(i);
                break;
            }
        }
        if (clocks.GetCount() == 0 && running) {
            AtomicInc(kill);
            while (running)
                Sleep(10);
            AtomicDec(kill);
        }
    }
    void SetTimeRefresh(i32 i) {
        clocks[i]->SetTimeRefresh();
    }
    i32 GetCount() {return clocks.GetCount();}
};


StaticClocks clocks;

void StaticClockThread(StaticClocks *gui) {
    while (true) {
        if (gui->kill) {
            AtomicDec(gui->running);
            return;
        }
        for (i32 i = 0; i < gui->GetCount(); ++i)
            PostCallback(callback(gui->clocks[i], &StaticClock::SetTimeRefresh));
        Sleep(200);
    }
}

StaticClock& StaticClock::SetAuto(bool mode) {
    autoMode = mode;
    if (!mode)
        clocks.Remove(this);
    else
        clocks.Add(this);
    return *this;
}

StaticClock::StaticClock() {
    Transparent();
    NoWantFocus();

    hourType = StaticClock::Square;
    numberType = StaticClock::Small;
    image = Null;
    colorType = WhiteType;
    seconds = true;

    t = GetSysTime();
    autoMode = false;
}

StaticClock::~StaticClock() {
    clocks.Remove(this);
}

void Meter::PaintMarks(BufferPainter &w, double cx, double cy, double R, double ang0,
            double ang1, i32 direction, double _step, double bigF, Color color) {
    if (direction == -1)
        Swap(ang0, ang1);
    ang0 = ToRad(ang0);
    ang1 = ToRad(ang1);
    _step = ToRad(_step);
    if (ang0 > ang1)
        ang1 += 2*M_PI;
    double width = 6*bigF;
    for (double i = ang0; i <= ang1+0.1; i += _step) {
        double x0 = cx + R*cos(i);
        double y0 = cy - R*sin(i);
        double x1 = cx + 0.93*R*cos(i);
        double y1 = cy - 0.93*R*sin(i);
        w.Move(x0, y0).Line(x1, y1).Stroke(width, color);
    }
}

void Meter::PaintNumbers(BufferPainter &w, double cx, double cy, double R, double a0,
    double _step, i32 direction, double minv, double maxv, double stepv, double bigF, Color color)
{
    a0 = ToRad(a0);
    _step = ToRad(_step);
    Font fnt = Arial(static_cast<i32>(13*bigF));
    while (minv <= maxv) {
        double x = cx + 0.8*R*cos(a0);
        double y = cy - 0.8*R*sin(a0);
        PaintCenterText(w, x, y, FormatDouble(minv), fnt, color);
        a0 += _step*direction;
        minv += stepv;
    }
}

void Meter::PaintHand(BufferPainter &w, double cx, double cy, double R, double val,
                      double bigF, i32 _colorType)
{
    Color letterColor;
    if (_colorType == WhiteType) {
        letterColor = Black();
    } else {
        letterColor = White();
    }
    val = ToRad(val);
    double x0 = cx + 0.90*R*cos(val);
    double y0 = cy - 0.90*R*sin(val);
    double x1 = cx;
    double y1 = cy;
    w.Move(x0, y0).Line(x1, y1).Stroke(5*bigF, LtGray());
    double x2 = cx + 0.4*R*cos(val);
    double y2 = cy - 0.4*R*sin(val);
    w.Move(x0, y0).Line(x2, y2).Stroke(4*bigF, letterColor);
    double x3 = cx - 0.3*R*cos(val);
    double y3 = cy + 0.3*R*sin(val);
    w.Move(x1, y1).Line(x3, y3).Stroke(5*bigF, letterColor);

    w.Circle(cx, cy, bigF*18).Fill(LtGray());
    w.Circle(cx, cy, bigF*15).Fill(Black());
}

void Meter::Paint(Draw& ww) {
    Size sz = GetSize();
    ImageBuffer ib(sz);
    BufferPainter w(ib);

    w.Clear(RGBAZero());
    w.LineCap(LINECAP_BUTT);

    Rect r(0, 0, sz.cx, sz.cy);

    Color backColor, letterColor;
    if (colorType == WhiteType) {
        backColor = White();
        letterColor = Black();
    } else {
        backColor = Black();
        letterColor = White();
    }
    w.DrawRect(r, backColor);
    i32 width = r.right - r.left;
    i32 height = r.bottom - r.top;
    double bigF = width/200.;
    double bigH = height/100.;

    i32 direction;
    if (clockWise)
        direction = -1;
    else
        direction = 1;
    double a = angleBegin;
    double b = angleEnd;
    double cosa = cos(ToRad(a));
    double cosb = cos(ToRad(b));
    double sina = sin(ToRad(a));
    double sinb = sin(ToRad(b));
    double minx = cosa;
    double maxx = cosa;
    double miny = sina;
    double maxy = sina;
    double angminx = a;
    //double angminy = a;
    if (cosb < minx) {
        minx = cosb;
        angminx = b;
    }
    if (cosb > maxx)
        maxx = cosb;
    if (sinb > miny) {
        miny = sinb;
        //angminy = b;
    }
    if (sinb < maxy)
        maxy = sinb;
    i32 maxgrad = 0;
    //double angmaxx = 0;
    for (double ang = a; ang != b; ang = AngleAdd360(ang, double(direction))) {
        maxgrad++;
        if (ang == 180) {
            minx = -1;
            angminx = 180;
        } else if (ang == 0) {
            maxx = 1;
            //angmaxx = 0;
        } else if (ang == 90) {
            miny = 1;
            //angminy = 90;
        } else if (ang == 270)
            maxy = -1;
    }
    double rate = width/(maxx - minx);
    double cx = -minx*rate;
    double R = 0.92*fabs(rate*minx/cos(ToRad(angminx)));
    double cy;
    if (fabs(miny) >= fabs(maxy))
        cy = 1.08*R*miny;
    else
        cy = r.bottom + 1.08*R*maxy;

    PaintArc(w, cx, cy, R, a, b, direction, fceil(0.8*bigF), letterColor);

    if (peak < max && peak > min) {
        double valpk = peak*maxgrad/(max-min);
        for (double i = 0.93; i < 0.98; i+= 0.004)
            PaintArc(w, cx, cy, i*R, a + valpk*direction, b, direction, static_cast<i32>(2*bigF), LtRed());
        double fsize = 7*drx::min(bigF, bigH);
        Font fnt = Arial(static_cast<i32>(fsize));
        double txtx = cx + R*cos(ToRad(b))/2;
        double txty = cy - R*sin(ToRad(b))/2;
        PaintCenterText(w, txtx, txty, "  PEAK", fnt, Gray());
        Image light;
        if (value < peak)
            light = Controls4UImg::LightOff();
        else
            light = Controls4UImg::LightOn();
        w.DrawImage(static_cast<i32>(txtx - fsize*2/3.), static_cast<i32>(txty + fsize),
                    static_cast<i32>(1.8*fsize),         static_cast<i32>(1.8*fsize), light);
    }
    double stepa = step*maxgrad/(max-min);
    PaintMarks(w, cx, cy, R, a, b, direction, stepa, bigF, letterColor);
    if (number)
        PaintNumbers(w, cx, cy, R, a, stepa, direction, min, max, step, bigF, letterColor);

    Font fnt = Arial(static_cast<i32>(20*drx::min(bigF, bigH)));
    double angtxt = ToRad(a + maxgrad*direction/2);
    double txtx = cx + R*cos(angtxt)/2;
    double txty = cy - R*sin(angtxt)/2;
    PaintCenterText(w, txtx, txty, text, fnt, letterColor);

    double vala = (value-min)*maxgrad/(max-min);
    PaintHand(w, cx, cy, R, a + vala*direction, bigF, colorType);

    i32 wm = width;
    if (colorType == WhiteType) {
        w.Begin();
            w.BeginMask();
                w.Rectangle(0, 0, wm, wm)
                  .Fill(wm/4, wm/4, Black(), wm, Color(100, 100, 100));
            w.End();
            w.Rectangle(0, 0, wm, wm).Fill(Black());
        w.End();
    } else {
        w.Begin();
            w.BeginMask();
                w.Rectangle(0, 0, wm, height).Fill(Color(70, 70, 70));
            w.End();
            w.Rectangle(0, 0, wm, height/2).Fill(White());
        w.End();
    }
    ww.DrawImage(0, 0, ib);
}

void MeterThread(Meter *gui, double newVal) {
    double delta = Sign(newVal-gui->value)*(gui->max - gui->min)/gui->sensibility;
    long deltaT = labs(long(1000.*delta*gui->speed/(gui->max - gui->min)));
    i32 maxi = static_cast<i32>(fabs((newVal-gui->value)/delta));

    //long t0 = GetTickCount();
    for (i32 i = 0; i < maxi; ++i, gui->value += delta) {
        if (gui->kill) {
            AtomicDec(gui->running);
            return;
        }
        PostCallback(callback(gui, &Meter::RefreshVal));
        Sleep(deltaT);
    }
    gui->value = newVal;
    PostCallback(callback(gui, &Meter::RefreshVal));
    AtomicDec(gui->running);
}

void Meter::SetData(const Val& v) {
    double val = minmax(double(v), min, max) ;

    if (running) {  // Stop movement before changing value
        AtomicInc(kill);
        while (running)
            Sleep(10);
        AtomicDec(kill);
    }
    AtomicInc(running);
    Thread().Run(callback2(MeterThread, this, val));
}

Meter::~Meter() {
    AtomicInc(kill);
    while (running)
        Sleep(10);
}

Meter::Meter() {
    Transparent();
    NoWantFocus();

    min = value = 0;
    max = 100;
    peak = 80;
    step = 20;
    angleBegin = 120;
    angleEnd = 40;
    clockWise = true;
    number = true;
    colorType = WhiteType;
    sensibility = 30;
    speed = 1;
    running = 0;
    kill = 0;
}

Knob::Knob() : value(0), minv(0), maxv(100), minorstep(10), majorstep(20), keyStep(1),
               angleBegin(225), angleEnd(315),
               colorType(SimpleWhiteType), clockWise(true), number(true), mark(Line),
               interlocking (false), style(Simple)  {
    Transparent();
}

void Knob::PaintMarks(BufferPainter &w, double cx, double cy, double R, double begin, double end,
        double ang0, double ang1, i32 direction, double _minorstep, double bigF, Color color) {
    if (direction == -1)
        Swap(ang0, ang1);
    ang0 = ToRad(ang0);
    double minorstepRad = ToRad(_minorstep);
    double width = bigF;
    i32 inum = 0;
    i32 imin = nminor;
    for (double iang = ang0; inum < nmajor+2; iang += minorstepRad) {
        if (imin >= nminor) {
            imin = 0;
            double x0 = cx + end*R*cos(iang);
            double y0 = cy - end*R*sin(iang);
            double x1 = cx + begin*R*cos(iang);
            double y1 = cy - begin*R*sin(iang);
            w.Move(x0, y0).Line(x1, y1).Stroke(width, color);
            inum++;
        } else {
            double x0 = cx + (begin+end)*R*cos(iang)/2;
            double y0 = cy - (begin+end)*R*sin(iang)/2;
            w.Circle(x0, y0, 0.2*(begin-end)*R).Fill(color);
            imin++;
        }
    }
}

void Knob::PaintNumbers(BufferPainter &w, double cx, double cy, double R, double a0,
    double step, i32 direction, double _minv, double _maxv, double stepv, double bigF, Color color) {
    double range = _maxv - _minv;
    a0 = ToRad(a0);
    step = ToRad(step);
    Font fnt = Arial(3+static_cast<i32>(4*bigF));
    Txt strminv;
    Txt strmaxv = FormatDoubleAutosize(_maxv, range);
    while (strminv != strmaxv) {
        double x = cx + 0.8*R*cos(a0);
        double y = cy - 0.8*R*sin(a0);
        strminv = FormatDoubleAutosize(_minv, range);
        PaintCenterText(w, x, y, strminv, fnt, color);
        a0 += step*direction;
        _minv += stepv;
    }
}

void Knob::PaintRugged(BufferPainter &sw, double cx, double cy, double angle, double r, double rugg, i32 numRug, Color &color) {
    double deltaAngle = 360./numRug;

    double iang = angle;
    for (i32 i = 0; i <= numRug; iang += deltaAngle, i++) {
        if (deltaAngle > 360)
            deltaAngle -= 360;

        PaintArc(sw, cx, cy, r-rugg/2-1, iang, iang+0.6*deltaAngle, 1, rugg, color);
        PaintArc(sw, cx, cy, r,          iang, iang+0.6*deltaAngle, 1, 1,    Gray());
        double riang = ToRad(iang);
        sw.Move(cx+r*cos(riang), cx-r*sin(riang)).Line(cx+(r-rugg)*cos(riang), cx-(r-rugg)*sin(riang)).Stroke(1, Gray());
    }
}

void Knob::Layout() {
    double R = min(GetSize().cx/2., GetSize().cy/2.);
    double dx = R;
    double maxgrad = fabs(angleEnd - angleBegin);
    if (clockWise)
        maxgrad = 360 - maxgrad;
    double dangle = (value - minv)*maxgrad/(maxv - minv);
    if (clockWise)
        dangle = angleBegin - dangle;
    else
        dangle = angleBegin + dangle;

    ImageBuffer ib(i32(2*R), i32(2*R));
    BufferPainter sw(ib);
    sw.Clear(RGBAZero());
    i32 direction;
    if (clockWise)
        direction = -1;
    else
        direction = 1;

    if (majorstep == 0)
        majorstep = maxv - minv;
    majorstep = (maxv-minv)/i32((maxv-minv)/majorstep);

    if (minorstep > majorstep || minorstep == 0)
        minorstep = majorstep;

    if (minorstep == 0)
        nminor = 0;
    else {
        nminor = i32(majorstep/minorstep) - 1;
        minorstep = majorstep/i32(majorstep/minorstep);
    }
    nmajor = i32((maxv-minv)/majorstep) - 1;

    double minorstepa = minorstep*maxgrad/(maxv - minv);    // Step in angle
    double majorstepa = majorstep*maxgrad/(maxv - minv);
    if (number) {
        PaintNumbers(sw, R, R, R, angleBegin, majorstepa, direction, minv, maxv, majorstep, R/40., Black());
        PaintMarks(sw, R, R, R, 0.65, 0.73, angleBegin, angleEnd, direction, minorstepa, R/40., Black());
        R *= 0.65;
    } else {
        PaintMarks(sw, R, R, R, 0.9, 1, angleBegin, angleEnd, direction, minorstepa, R/40., Black());
        R *= 0.9;
    }
    dx = dx - R;

    double rugg = 0;
    if (style == Rugged)
        rugg = 0.5+0.054*R;

    double r = 0.15*R;
    R = R-rugg;

    if (colorType == WhiteType || colorType == BlackType) {
        Color fill = (colorType == SimpleWhiteType || colorType == WhiteType) ? White() : Color(20, 20, 20);
        {
            if (colorType == WhiteType) {
                sw.Circle(dx+rugg+R, dx+rugg+R, R).Fill(dx+R/2, dx+R/2, White(), dx+R, dx+R, R, LtGray());
            } else if (colorType == BlackType) {
                sw.Circle(dx+rugg+R, dx+rugg+R, R).Fill(fill);
                sw.Begin();
                    sw.BeginMask();
                        sw.Circle(dx+rugg+R, dx+rugg+R, R).Fill(White());
                    sw.End();
                    sw.Ellipse(dx+rugg+R, dx+rugg+0.5*R, 0.89*R, 0.5*R).Fill(dx, dx, White(), dx, dx+0.9*R, Color(20, 20, 20));
                    sw.Ellipse(dx+rugg+R, dx+rugg+1.7*R, 0.8*R, 0.5*R).Fill(dx, dx+2*R, White(), dx, dx+1.4*R, Color(20, 20, 20));
                sw.End();
            }
        }
        {
            ImageBuffer _ib(i32(2*r)+1, i32(2*r)+1);
            BufferPainter _sw(_ib);
            _sw.Clear(RGBAZero());

            if (colorType == WhiteType) {
                if (mark == Circle)
                    _sw.Circle(r, r, r).Fill(1.5*r, 1.5*r, White(), r, r, r, LtGray());
            } else if (colorType == BlackType) {
                if (mark == Circle) {
                    Color lineColor = (colorType == WhiteType) ? Black() : White();
                    Color almostColor = (colorType == WhiteType) ? Color(220, 220, 220) : Color(60, 60, 60);
                    _sw.Circle(r, r, r).Fill(fill);
                    _sw.Begin();
                        _sw.BeginMask();
                            _sw.Circle(r, r, r).Fill(almostColor);
                        _sw.End();
                        _sw.Ellipse(r, r + r/2,   r, r/2).Fill(lineColor);
                        _sw.Ellipse(r, r - r*0.9, r, r/2).Fill(lineColor);
                    _sw.End();
                }
            }
            imgMark = _ib;
        }
    }
    img = ib;
    Ctrl::Layout();
}

void Knob::Paint(Draw& w) {
    Size sz = GetSize();
    double maxgrad = fabs(angleEnd - angleBegin);
    if (clockWise)
        maxgrad = 360 - maxgrad;
    double dangle = (value - minv)*maxgrad/(maxv - minv);
    if (clockWise)
        dangle = angleBegin - dangle;
    else
        dangle = angleBegin + dangle;
    double angle = ToRad(dangle);
    double r = min(GetSize().cx/2., GetSize().cy/2.);
    double cx = r;
    double cy = r;

    ImageBuffer ib(sz);
    BufferPainter sw(ib);
    sw.Clear(RGBAZero());

    //if (majorstep == 0)
    //  majorstep = maxv - minv;
    //majorstep = (maxv-minv)/i32((maxv-minv)/majorstep);

    //if (minorstep > majorstep || minorstep == 0)
    //  minorstep = majorstep;

    //if (minorstep == 0) {
    //  nminor = 0;
    //} else {
    //  nminor = i32(majorstep/minorstep) - 1;
    //  minorstep = majorstep/i32(majorstep/minorstep);
    //}
    //nmajor = i32((maxv-minv)/majorstep) - 1;

    if (HasFocus()) {
        if (number)
            sw.Circle(cx, cy, 0.65*r+2.5).Stroke(5, GrayColor(240));
        else
            sw.Circle(cx, cy, 0.9*r+2.5).Stroke(5, GrayColor(240));
    }

    if (number)
        r *= 0.65;
    else
        r *= 0.9;

    double capt = 0;
    if (HasCapture())
        capt = 1;

    Color fill = (colorType == SimpleWhiteType || colorType == WhiteType) ? White() : Color(20, 20, 20);
    double rugg = 0;
    if (style == Rugged)
        rugg = 0.5+0.05*r;
    sw.Circle(cx, cy, r-capt-rugg).Stroke(1, Gray());
    Color ruggColor = fill;
    if (colorType == WhiteType)
        ruggColor = LtGray();
    if (style == Rugged)
        PaintRugged(sw, cx, cy, dangle, r-capt, rugg, 4+i32(r/6), ruggColor);

    if (IsNull(img))
        Layout();
    double realR = r-capt;
    sw.DrawImage(i32(capt), i32(capt), i32(2*(cx-capt)), i32(2*(cx-capt)), img);
    if (colorType == SimpleWhiteType || colorType == SimpleBlackType) {
        sw.Circle(cx, cy, realR-rugg).Fill(fill);

        Color lineColor = (colorType == SimpleWhiteType) ? Color(20, 20, 20) : White();
        if (mark == Line)
            sw.Move(cx+(r-capt)*cos(angle), cy-(r-capt)*sin(angle))
              .Line(cx+0.5*r*cos(angle), cy-0.5*r*sin(angle)).Stroke(r/25., lineColor);
        else if (mark == Circle)
            sw.Circle(cx+0.7*r*cos(angle), cy-0.7*r*sin(angle), 0.15*r).Stroke(1, lineColor).Fill(fill);
    } else if (colorType == WhiteType || colorType == BlackType) {
        Color lineColor = (colorType == WhiteType) ? Black() : White();
        if (mark == Line)
            sw.Move(cx+realR*cos(angle), cy-realR*sin(angle))
              .Line(cx+0.5*r*cos(angle), cy-0.5*r*sin(angle)).Stroke(r/25., lineColor);
        else if (mark == Circle) {
            double ccx = cx+0.7*r*cos(angle);
            double ccy = cy-0.7*r*sin(angle);
            if (IsNull(imgMark))
                Layout();
            sw.DrawImage(i32(ccx-0.15*r), i32(ccy-0.15*r), imgMark);
        }
    }
    w.DrawImage(0, 0, ib);
}

bool Knob::Key(dword key, i32 repcnt) {
    double fs = keyStep;
    if (interlocking)
        fs = minorstep;

    if(IsEditable())
        switch(key) {
        case K_RIGHT:
        case K_UP:
            Inc(fs);
            return true;
        case K_PAGEUP:
            Inc(minorstep);
            return true;
        case K_LEFT:
        case K_DOWN:
            Dec(fs);
            return true;
        case K_PAGEDOWN:
            Dec(minorstep);
            return true;
        }
    return Ctrl::Key(key, repcnt);
}

double Knob::SliderToClient(Point _pos) {
    double r = min(GetSize().cx/2., GetSize().cy/2.);
    double rx = r-_pos.x;
    double ry = r-_pos.y;
    if (rx*rx + ry*ry > r*r)
        return Null;
    double angle = atan(fabs(ry/rx))*180/M_PI;
    if (_pos.x > r) {
        if (_pos.y > r)
            angle = 360 - angle;
    } else {
        if (_pos.y > r)
            angle += 180;
        else
            angle = 180 - angle;
    }
    return angle;
}

void Knob::LeftDown(Point _pos, dword ) {
    if(!IsEditable())
        return;
    SetWantFocus();
    angleClick = SliderToClient(_pos);
    if (IsNull(angleClick))
        return;
    SetCapture();
    Refresh();
}

void Knob::LeftRepeat(Point p, dword f) {
    if(!HasCapture())
        LeftDown(p, f);
}

void Knob::LeftUp(Point , dword ) {
    if (HasCapture()) {
        if (interlocking)
            value = minorstep*fround((value-minv)/minorstep);
        WhenSlideFinish();
    }
    Refresh();
}

void Knob::MouseMove(Point _pos, dword ) {
    if(!HasCapture())
        return;
    double angle = SliderToClient(_pos);
    if (IsNull(angle)) {
        angleClick = Null;
        return;
    }
    if (IsNull(angleClick)) {
        angleClick = angle;
        return;
    }
    double deltaAngle = angleClick - angle;
    deltaAngle *= clockWise ? 1 : -1;
    if (fabs(deltaAngle) > 180)
        deltaAngle = -Sign(deltaAngle)*(360 - fabs(deltaAngle));
    double maxgrad = fabs(angleEnd - angleBegin);
    if (clockWise)
        maxgrad = 360 - maxgrad;
    double deltaVal = deltaAngle*(maxv - minv)/maxgrad;
    double n = minmax(value + deltaVal, minv, maxv);
    angleClick = angle;
    if (n != value) {
        value = n;
        WhenSlideFinish();
        UpdateActionRefresh();
    }
}

void Knob::SetData(const Val& v) {
    double n = minmax(double(v), minv, maxv);
    if(n != value) {
        value = n;
        UpdateRefresh();
    }
}

Val Knob::GetData() const {
    return value;
}

void Knob::Dec(double st) {
    double n = max(value - st, minv);
    if (interlocking)
        n = minorstep*fround((n-minv)/minorstep);
    if(n != value) {
        value = n;
        WhenSlideFinish();
        Refresh();
    }
}

void Knob::Inc(double st) {
    double n = min(value + st, maxv);
    if (interlocking)
        n = minorstep*fround((n-minv)/minorstep);
    if(n != value) {
        value = n;
        WhenSlideFinish();
        Refresh();
    }
}

void Knob::GotFocus() {
    Refresh();
}

void Knob::LostFocus() {
    Refresh();
}


void FileBrowser::AddFolder(Txt _folder, Txt &myFolders, i32 id) {
    if (!_folder.IsEmpty() && DirExists(_folder))
        if (myFolders.Find(_folder + ";") < 0) {
            folders.Add(id, NativePathIconX(_folder, true, flags), _folder, GetFName(_folder), true);
            myFolders << _folder << ";";
        }
}

struct DisplayNameIcon : public Display {
    Font fnt;

    virtual Size GetStdSize(const Val& q) const {
        ValArray va = q;
        Txt fileName = GetFName(Txt(va[0]));
        Size sz = GetTextSize(fileName, fnt);
        sz.cx += 20;
        sz.cy = max(sz.cy, 16);
        return sz;
    }
    virtual void Paint(Draw& w, const Rect& r, const Val& q, Color ink, Color paper, dword ) const {
        ValArray va = q;
        Txt fileName = GetFName(Txt(va[0]));
        Image icon = va[1];
        w.DrawRect(r, paper);
        w.DrawImage(r.left, r.top + (r.Height() - 16) / 2, IsNull(icon) ? Null : icon);
        w.DrawText(r.left + 20, r.top + (r.Height() - Draw::GetStdFontCy()) / 2, fileName, fnt, ink);
        if (GetTextSize(fileName, fnt).cx + 5 > (r.GetWidth() - static_cast<i32>(0.8*EditTxt::GetStdHeight()))) {
            Image rightIcon = CtrlImg::right_arrow();
            w.DrawRect(r.right - 6, r.top, r.right, r.bottom, paper);
            w.DrawImage(r.right - 10, r.top + rightIcon.GetHeight()/2, rightIcon);
        }
    }
    DisplayNameIcon() { fnt = StdFont(); }
};

struct FNameConvert : public Convert {
    static ValArray values;
    Val Format(const Val& v) const {
        values = v;
        ValArray va = v;
        return GetFName(Txt(va[0]));
    }
    Val Scan(const Val& text) const {
        if (values.IsEmpty())
            values.SetCount(1, Txt(""));
        values.Set(0, AppendFNameX(GetFileDir(Txt(values[0])), Txt(text)));
        return values;
    }
};

ValArray FNameConvert::values;

struct FileLenConvert : public Convert {
    Val Format(const Val& v) const {
        if (v == -1)
            return "";
        else
            return BytesToTxt(z64(v));
    }
};

FileBrowser::FileBrowser() {
    flags = EXT_FILE_FLAGS(USE_TRASH_BIN | BROWSE_LINKS);   // | ASK_BEFORE_DELETE;
    readOnly = false;
    acceptDragAndDrop = true;

    pack.Horz(foldersRect, files);
    pack.SetPos(3000);
    i32 height = folder.GetStdHeight();
    foldersRect.Add(foldersLabel.HSizePos().TopPos(0, height));
    foldersRect.Add(folders.HSizePos().VSizePos(height + 1));

    Add(folder.HSizePos().TopPos(0, height));
    Add(pack.HSizePos().VSizePos(height + 1));

    Transparent();
    Background(Null);
    pack.Transparent();
    foldersRect.Transparent();
    foldersRect.Background(Null);

    folder.UseBrowse(false).UseUp(true).UseHistory(true);
    folder <<= OTVET(FolderWhenChange);
    foldersLabel.SetText(t_("Папки"));

    folders.NoRoot();
    folders.WhenOpen       = OTVET(FoldersWhenOpen);
    folders.WhenSel        = OTVET(FoldersWhenSel);
    folders.WhenLeftDouble = OTVET(FoldersWhenLeftDouble);
    folders.WhenClose      = OTVET(FoldersWhenClose);

    Vec<Txt> ds = GetDriveList();
    Txt desktopFolder = GetDesktopFolder();
    if (!desktopFolder.IsEmpty())
        folders.Add(0, NativePathIconX(desktopFolder, true, flags), desktopFolder,
                                                            GetFName(desktopFolder), true);
    i32 id = folders.Add(0, NativePathIconX(desktopFolder, true, flags),
                                                    t_("Мои Папки"), t_("Мои Папки"), true);

    Txt myFolders = desktopFolder + ";";

    AddFolder(GetPersonalFolder(), myFolders, id);
    AddFolder(GetMusicFolder(), myFolders, id);
    AddFolder(GetPicturesFolder(), myFolders, id);
    AddFolder(GetVideoFolder(), myFolders, id);
    AddFolder(GetDownloadFolder(), myFolders, id);
    AddFolder(GetAppDataFolder(), myFolders, id);
    AddFolder(GetTemplatesFolder(), myFolders, id);

    for (i32 i = 0; i < ds.GetCount(); ++i)
        folders.Add(0, NativePathIconX(ds[i], true, flags), ds[i], ds[i], true);
    noDoOpen = true;
    folders.Open(0);

    files.Reset();
    files.AddColumn(t_("Имя")).SetDisplay(Single<DisplayNameIcon>()).Add(3).Margin(2)
                .Edit(textFName).HeaderTab().Min(50).WhenAction = OTVET1(SortByColumn, 0);
    files.AddColumn(t_("Размер")).SetConvert(Single<FileLenConvert>()).HeaderTab()
                .Min(50).WhenAction = OTVET1(SortByColumn, 1);
    files.AddColumn(t_("Дата")).HeaderTab().Min(50).WhenAction = OTVET1(SortByColumn, 2);
    files.AddColumn("Папка ли");

    files.MultiSelect().HeaderObject().Absolute().Clipboard();
    files.BackPaintHint();
    files.AddIndex();
    files.VertGrid(false).HorzGrid(false);
    files.SetLineCy(static_cast<i32>(0.85*EditTxt::GetStdHeight()));
    files.HeaderTab(0).SetRatio(10);
    files.ColumnWidths("200 60 120");
    files.EvenRowColor(Blend(SColorMark, SColorPaper, 240));
    files.textFName = &textFName;

    files.WhenSel        = OTVET(FilesWhenSel);
    files.WhenLeftDouble = OTVET(FilesWhenLeftDouble);
    files.WhenEnterKey   = OTVET(FilesWhenLeftDouble);

    textFName.SetConvert(Single<FNameConvert>());
    textFName.WhenChange = OTVET(FNameWhenChanged);
}

void FileBrowser::FilesWhenSel() {
    if (files.GetCursor() < 0)
        return;
    ValArray va = files.GetColumn(files.GetCursor(), 0);
    if (t_("Нет файлов или доступ закрыт") == va[0])
        fileNameSelected = "";
    else
        fileNameSelected = va[0];
    if (WhenSelected)
        WhenSelected();
}

void FileBrowser::FNameWhenChanged() {
//  fileNameSelected;       // Old name full path
//  textFName;           // New name just name
}

void FileBrowser::FoldersWhenOpen(i32 id) {
    if (noDoOpen) {
        noDoOpen = false;
        return;
    }
    folder.SetData(folders[id]);
    forceOpenTree = true;
    FolderWhenChange();
    forceOpenTree = false;
}

void FileBrowser::FoldersWhenClose(i32 id) {
    if (folders[id] != t_("Файловая Система") && folders[id] != t_("Мои Папки"))
        folders.RemoveChildren(id);
}

void FileBrowser::FoldersWhenSel()  {
    folder.SetData(~folders);
    FolderWhenChange();
    if (WhenTreeSelected)
        WhenTreeSelected();
}

void FileBrowser::FoldersWhenLeftDouble() {
    if (WhenTreeDblClick)
        WhenTreeDblClick();
}

void FileBrowser::FilesWhenLeftDouble() {
    FilesWhenSel();

    if (IsSymLink(fileNameSelected)) {
        Txt newPath = GetSymLinkPath(fileNameSelected);
        if (!newPath.IsEmpty())
            fileNameSelected = newPath;
    }
    if (FileExists(fileNameSelected)) {
        if(WhenAction)
            WhenAction();
    } else if (DirExistsX(fileNameSelected, flags)) {
        folder.SetData(fileNameSelected);
        FolderWhenChange();
    }
}

void FileBrowser::FilesList(Txt folderName, bool &thereIsAFolder) {
    files.Clear();
    FileDataArray fileData(false, flags);
    fileData.Search(folderName, "*.*", false);
    if (fileData.GetCount() == 0) {
        i32 cy = files.GetLineCy();
        Image img = Rescale(CtrlImg::information(), cy, cy);
        files.Add(t_("Нет файлов или доступ закрыт"), -1, "", img, false, 0);
        return;
    }
    fileData.SortByName();

    thereIsAFolder = false;
    for(i32 i = 0; i < fileData.GetCount(); i++) {
        Txt fullFilename = fileData.FullFName(i);
        Txt fileName     = fileData[i].fileName;
        bool isFolder       = fileData[i].isFolder;

        if (isFolder)
            thereIsAFolder = true;
        files.Add(fullFilename,
                  isFolder ? -1 : fileData[i].length,
                  fileData[i].t,
                  NativePathIconX(fullFilename, DirExistsX(fullFilename, flags), flags),
                  isFolder ? fileName : "ZZZZZZZ" + fileName);
    }
}

bool HasSubfolders(Txt folder, i32 ) {
    FindFile ff(AppendFNameX(folder, "*.*"));
    while(ff) {
        if (DirExistsX(AppendFNameX(folder, ff.GetName())))
            return true;
        ff.Next();
    }
    return false;
}

void FileBrowser::FolderWhenChange() {
    i32 id;

    if (~folder == "" || ~folder == t_("Мои Папки")) {
        id = folders.Find(~folder);
        folders.Open(id, true);
        files.Clear();
        return;
    }
    Txt folderName = FileRealName(~folder);
    fileNameSelected = folderName;
    if (folderName.IsEmpty()) {
        Exclamation(Format(t_("Папки %s не существует или она недоступна"), ~folder));
        return;
    }
    if (folderName != ~folder)
        folder.SetData(folderName);

    Txt folderNameAux = folderName;

    while ((id = folders.Find(folderNameAux)) == -1) {
        Txt upper = GetUpperFolder(folderNameAux);
        if (upper == folderNameAux)
            return;
        folderNameAux = upper;
    }
    bool thereIsAFolder;
    FilesList(folderName, thereIsAFolder);

    if (folderNameAux != folderName || !folders.IsOpen(id) || forceOpenTree) {
        while (folderNameAux != folderName) {
            Txt newFolderNameAux = GetNextFolder(folderNameAux, folderName);
            noDoOpen = true;
            folders.Open(id, true);

            FileDataArray fileData;
            fileData.Search(folderNameAux, "*.*", false);
            fileData.SortByName();
            i32 newid = -1;
            folders.RemoveChildren(id);
            for (i32 i = 0; i < fileData.GetCount(); ++i) {
                Txt fullName = fileData.FullFName(i);
                i32 auxid = folders.Add(id, NativePathIconX(fullName, true, flags),
                                    fullName, fileData[i].fileName, HasSubfolders(fullName, flags));
                if (fullName == newFolderNameAux)
                    newid = auxid;
            }
            if (newid == -1) {
                Exclamation(Format(t_("Папка %s не найдена"), DeQtf(folderNameAux)));
                return;
            }
            id = newid;
            folderNameAux = newFolderNameAux;
        }
        FileDataArray fileData;
        fileData.Search(folderName, "*.*", false);
        fileData.SortByName();
        folders.RemoveChildren(id);
        for (i32 i = 0; i < fileData.GetCount(); ++i) {
            Txt fullName = fileData.FullFName(i);
            if (DirExistsX(fullName, flags))
                folders.Add(id, NativePathIconX(fullName, true, flags), fullName,
                                            fileData[i].fileName, HasSubfolders(fullName, flags));
        }
        noDoOpen = true;
        folders.Open(id, true);
    }
    if (WhenSelected)
        WhenSelected();
}

void FileBrowser::FilesEnterRow() {
    Exclamation("DELETE");
    //i32 i = 1;
}

void FileBrowser::SortByColumn(i32 col) {
    static bool order[] = {true, true, true};

    if (col < 0 || col > 2)
        return;

    i32 ordercol = col;
    if (col == 0)
        ordercol = 4;
    if (order[col])
        files.Sort(ordercol, StdValCompare);
    else
        files.Sort(ordercol, StdValCompareDesc);
    order[col] = !order[col];
}

Txt FileBrowser::GetFile() {
    return fileNameSelected;
}

Txt FileBrowser::GetFolder() {
    return ~folders;
}


Aboutdrx::AboutUpp() {
    about.SetQTF(GetTopic("topic://Controls4U/src/About$en-us"), Zoom(130, 1024));
    about.SetZoom(Zoom(1, 1));
    about.RightPos(0, 0).VSizePos();
    about.HMargins(4);
    about.SetFrame(NullFrame());
    about.NoLazy();
    Add(about.SizePos());
}

}