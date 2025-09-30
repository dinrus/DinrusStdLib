#ifndef _Core_Hash_h_
#define _Core_Hash_h_

/* MD5 context. */
typedef struct {
	dword state[4];                                   /* state (ABCD) */
	dword count[2];        /* number of bits, modulo 2^64 (lsb first) */
	u8 buffer[64];                         /* input buffer */
} DRX_MD5_CTX;

class Md5Stream : public OutStream {
	DRX_MD5_CTX context;

	virtual  void  Out(ukk data, dword size);

public:
	void   Finish(u8 *hash16);
	Txt    FinishTxt();
	Txt    FinishTxtS();
	void   Reset();

	Md5Stream();
	~Md5Stream();
};

void    MD5(u8 *hash16, ukk data, dword size);
void    MD5(u8 *hash16, const Txt& data);
Txt     MD5Txt(ukk data, dword size);
Txt     MD5Txt(const Txt& data);
Txt     MD5TxtS(ukk data, dword size);
Txt     MD5TxtS(const Txt& data);

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    u8 buffer[64];
} DRX_SHA1_CTX;

class Sha1Stream : public OutStream {
	DRX_SHA1_CTX ctx[1];

	virtual  void  Out(ukk data, dword size);

	void  Cleanup()                      { memset(ctx, 0, sizeof(ctx)); }

public:
	void   Finish(u8 *hash20);
	Txt    FinishTxt();
	Txt    FinishTxtS();

	void   Reset();
	void   New()                         { Reset(); }

	Sha1Stream();
	~Sha1Stream();
};

void    SHA1(u8 *hash20, ukk data, dword size);
void    SHA1(u8 *hash20, const Txt& s);
Txt     SHA1Txt(ukk data, dword size);
Txt     SHA1Txt(const Txt& data);
Txt     SHA1TxtS(ukk data, dword size);
Txt     SHA1TxtS(const Txt& data);

class Sha256Stream : public OutStream {
	u8  buffer[128];

	virtual  void  Out(ukk data, dword size);

	void  Cleanup();

public:
	void   Finish(u8 *hash32);
	Txt    FinishTxt();
	Txt    FinishTxtS();

	void   Reset();
	void   New()                         { Reset(); }

	Sha256Stream();
	~Sha256Stream();
};

void    SHA256(u8 *hash32, ukk data, dword size);
void    SHA256(u8 *hash32, const Txt& s);
Txt     SHA256Txt(ukk data, dword size);
Txt     SHA256Txt(const Txt& data);
Txt     SHA256TxtS(ukk data, dword size);
Txt     SHA256TxtS(const Txt& data);

class xxHashStream : public OutStream {
	u8 context[8 * 8];

	virtual  void  Out(ukk data, dword size);

public:
	i32 Finish();

	void Reset(dword seed = 0);

	xxHashStream(dword seed = 0);
};

i32 xxHash(ukk data, size_t len);
i32 xxHash(const Txt& s);

class xxHash64Stream : public OutStream {
	u8 context[12 * 8];

	virtual  void  Out(ukk data, dword size);

public:
	z64 Finish();

	void Reset(dword seed = 0);

	xxHash64Stream(dword seed = 0);
};

z64 xxHash64(ukk data, size_t len);
z64 xxHash64(const Txt& s);

#endif
