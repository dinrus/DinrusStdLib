#ifndef _Geom_Ctrl_CoordCtrl_h_
#define _Geom_Ctrl_CoordCtrl_h_

namespace drx {

class EditDegree : public EditVal<double, ConvertDegree>
{
public:
	typedef EditDegree CLASSNAME;
	EditDegree();

	virtual bool  Key(dword key, i32 repcnt);

private:
	void          OnInc();
	void          OnDec();

private:
	SpinButtons   spin;
};

}

#endif
