class InFilterStream : public Stream {
public:
	virtual   bool  IsOpen() const;

protected:
	virtual   i32   _Term();
	virtual   i32   _Get();
	virtual   dword _Get(uk data, dword size);

	Vec<u8> buffer;
	bool         eof;
	i32          buffersize = 4096;
	Buffer<i32>  inbuffer;
	u8        *t; // target pointer for block _Get
	dword        todo; // target count

	void   Init();
	void   Fetch();
	void   SetRd();

private:
	void   SetSize(z64 size)  { NEVER(); } // removed
	z64    GetSize() const      { NEVER(); return 0; }

public:
	Stream                      *in;
	Event<ukk , i32>     Filter;
	Gate<>                       FilterEof;
	Event<>                      End;
	Gate<>                       More;
	void                         Out(ukk ptr, i32 size);
	
	Event<>                      WhenOut;
	
	template <class F>
	void Set(Stream& in_, F& filter) {
		Init();
		in = &in_;
		filter.WhenOut = [=, this](ukk ptr, i32 size) { Out(ptr, size); };
		Filter = [&filter](ukk ptr, i32 size) { filter.Put(ptr, size); };
		End = [&filter] { filter.End(); };
	}
	
	void SetBufferSize(i32 size) { buffersize = size; inbuffer.Clear(); }
	
	InFilterStream();
	template <class F> InFilterStream(Stream& in, F& filter) { Set(in, filter); }
};

class OutFilterStream : public Stream {
public:
	virtual   void  Close();
	virtual   bool  IsOpen() const;

protected:
	virtual   void  _Put(i32 w);
	virtual   void  _Put(ukk data, dword size);

	Buffer<u8> buffer;
	z64        count;

	void   FlushOut();
	dword  Avail()               { return dword(4096 - (ptr - ~buffer)); }
	void   Init();

public:
	Stream                      *out;
	Event<ukk , i32>     Filter;
	Event<>                      End;
	void                         Out(ukk ptr, i32 size);
	
	z64                        GetCount() const             { return count; }
	
	Event<z64>                 WhenPos;

	template <class F>
	void Set(Stream& out_, F& filter) {
		Init();
		out = &out_;
		filter.WhenOut = callback(this, &OutFilterStream::Out);
		Filter = callback<F, F, ukk , i32>(&filter, &F::Put);
		End = callback(&filter, &F::End);
		count = 0;
	}
	
	OutFilterStream();
	template <class F> OutFilterStream(Stream& in, F& filter) { Set(in, filter); }
	~OutFilterStream();
};
