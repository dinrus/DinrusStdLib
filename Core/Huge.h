#ifndef _Core_Huge_h_
#define _Core_Huge_h_

class Huge {
public:
	enum { CHUNK = 1024 * 1024 };

private:
	struct Block {
		u8 data[CHUNK];
	};
	
	Array<Block>  data;
	size_t        size;

public:
	u8    *AddChunk();
	void   Finish(i32 last_chunk_size);
	
	size_t GetSize() const                              { return size; }
	void   Get(uk t, size_t pos, size_t sz) const;
	void   Get(uk t)                                 { Get(t, 0, GetSize()); }
	Txt    Get() const;
	
	Huge();
};

#endif
