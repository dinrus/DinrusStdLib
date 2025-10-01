template <class DataType, class Base, class IncType>
WithSpin<DataType, Base, IncType>::WithSpin()
:	inc(WithSpin_DefaultIncVal<IncType>())
{
	Init();
}

template <class DataType, class Base, class IncType>
WithSpin<DataType, Base, IncType>::WithSpin(IncType inc)
:	inc(inc)
{
	Init();
}

template <class DataType, class Base, class IncType>
WithSpin<DataType, Base, IncType>::WithSpin(DataType min, DataType max, IncType inc)
:	inc(WithSpin_DefaultIncVal<IncType>())
{
	Base::MinMax(min, max);
	Init();
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::Init()
{
	style = &SpinButtons::StyleDefault();
}

template <class DataType, class Base, class IncType>
WithSpin<DataType, Base, IncType>& WithSpin<DataType, Base, IncType>::OnSides(bool b)
{
	SetSpinStyle(b ? SpinButtons::StyleOnSides() : SpinButtons::StyleDefault());
	Base::RefreshLayout();
	return *this;
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::Inc()
{
	if(Ctrl::IsReadOnly()) {
		BeepExclamation();
		return;
	}
	DataType d = Base::GetData();
	if(!IsNull(d)) {
		WithSpin_Add(d, inc, Base::GetMin(), roundfrommin);
		if(IsNull(Base::GetMax()) || d <= Base::GetMax()) {
			Base::SetData(d);
			Ctrl::Action();
		}
	}
	else {
		DataType min = Base::GetMin();
		if(IsNull(min) || min <= Base::GetDefaultMin())
			Base::SetData(WithSpin_DefaultStartVal<DataType>());
		else
			Base::SetData(min);
	}
	Ctrl::SetFocus();
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::Dec()
{
	if(Ctrl::IsReadOnly()) {
		BeepExclamation();
		return;
	}
	DataType d = Base::GetData();
	if(!IsNull(d)) {
		WithSpin_Add(d, -inc, Base::GetMin(), roundfrommin);
		if(IsNull(Base::GetMin()) || d >= Base::GetMin()) {
			Base::SetData(d);
			Ctrl::Action();
		}
	}
	else {
		DataType max = Base::GetMax();
		if(IsNull(max) || max >= Base::GetDefaultMax())
			Base::SetData(WithSpin_DefaultStartVal<DataType>());
		else
			Base::SetData(max);
	}
	Ctrl::SetFocus();
}

template <class DataType, class Base, class IncType>
bool WithSpin<DataType, Base, IncType>::Key(dword key, i32 repcnt)
{
	if(keys) {
		if(key == K_UP) {
			Inc();
			return true;
		}
		if(key == K_DOWN) {
			Dec();
			return true;
		}
	}
	return Base::Key(key, repcnt);
}

template <class DataType, class Base, class IncType>
Image WithSpin<DataType, Base, IncType>::MouseEvent(i32 event, Point p, i32 zdelta, dword keyflags)
{
	if(ButtonsMouseEvent(this, event, p))
		return Image::Arrow();
	return Base::MouseEvent(event, p, zdelta, keyflags);
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::MouseWheel(Point, i32 zdelta, dword)
{
	if(mousewheel) {
		if(zdelta < 0)
			Dec();
		else
			Inc();
	}
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::CancelMode()
{
	Base::CancelMode();
	ButtonsCancelMode();
}

template <class DataType, class Base, class IncType>
i32 WithSpin<DataType, Base, IncType>::GetSpaceLeft() const
{
	if(!visible)
		return 0;
	return style->onsides ? min(Base::GetSize().cx / 2, style->width) - style->over : 0;
}

template <class DataType, class Base, class IncType>
i32 WithSpin<DataType, Base, IncType>::GetSpaceRight() const
{
	if(!visible)
		return 0;
	return min(Base::GetSize().cx / 2, style->width) - style->over;
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::EditCapture()
{
	buttons_capture = false;
}

template <class DataType, class Base, class IncType>
bool WithSpin<DataType, Base, IncType>::HasEditCapture()
{
	return Base::HasCapture() && !buttons_capture;
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::ButtonPush(i32 i)
{
	if(i)
		Inc();
	else
		Dec();
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::ButtonRepeat(i32 i)
{
	ButtonPush(i);
}

template <class DataType, class Base, class IncType>
void WithSpin<DataType, Base, IncType>::PaintSpace(Draw& w)
{
	PaintButtons(w, this);
}

template <class DataType, class Base, class IncType>
i32 WithSpin<DataType, Base, IncType>::ButtonCount() const
{
	return visible ? 2 : 0;
}

template <class DataType, class Base, class IncType>
Rect WithSpin<DataType, Base, IncType>::ButtonRect(i32 i) const
{
	Rect rect = Base::GetSize();
	i32 h = rect.GetHeight();
	i32 h7 = min(rect.GetWidth() / 2, style->width);
	i32 h7o = h7 - style->over;
	if(style->onsides)
		return i ? RectC(rect.left - style->over, rect.top, h7, h) : RectC(rect.right - h7o, rect.top, h7, h);
	i32 h2 = h / 2;
	return i ? RectC(rect.right - h7o, rect.top, h7, h2) : RectC(rect.right - h7o, rect.top + h2, h7, h - h2);
}

template <class DataType, class Base, class IncType>
const Button::Style& WithSpin<DataType, Base, IncType>::ButtonStyle(i32 i) const
{
	return i ? style->inc : style->dec;
}