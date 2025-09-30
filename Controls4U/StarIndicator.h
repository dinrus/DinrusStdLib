#include <drx/CtrlLib/CtrlLib.h>


namespace drx {

class StarIndicator : public ProgressIndicator {
private:
	bool	m_bAlwaysShowVal;	// True if value/votes must be always shown, not only if mouse over
	bool	m_bMouseInside;	// True if mouse over the control
	bool	m_bTotal;		// Shows the total (5) or not
	bool	m_bVotes;		// Shows number of votes or not
	Color	m_FontColor;	// Font color
	i32		m_nVotes;		// Number of votes
	i32		m_nVotesHigh;	// Number of Votes for Green
	i32		m_nVotesLow;	// Number of Votes for Red

public:
	StarIndicator();
	virtual ~StarIndicator();

	virtual StarIndicator&	AlwaysShowVal(bool b)		{ m_bAlwaysShowVal=b; return *this; }
	virtual void	MouseEnter(Point p, dword keyflags);
	virtual void	MouseLeave();
	virtual void	MouseMove(Point , dword )		{ Refresh(); }
	virtual void	Paint(Draw& draw);
	virtual void	Layout();
	virtual StarIndicator&	SetFontColor(Color c)	{ m_FontColor=c; return *this; }
	virtual void	Set(double n);
	virtual void	Set(i32 n)						{ return Set(static_cast<double>(n)); }
	virtual void	Set(i32 n, i32 tot)				{ return ProgressIndicator::Set(n, tot); }
	virtual StarIndicator&	SetVotes(i32 n);
	virtual StarIndicator&	SetVotesHigh(i32 n)		{ m_nVotesHigh=n; return *this; }
	virtual StarIndicator&	SetVotesLow(i32 n)		{ m_nVotesLow=n; return *this; }
	virtual StarIndicator&	ShowPercent(bool b)		{ percent=b; return *this; }
	virtual StarIndicator&	ShowTotal(bool b)		{ m_bTotal=b; return *this; }
	virtual StarIndicator&	ShowVotes(bool b)		{ m_bVotes=b; return *this; }
};

}