struct KeyInfo {
	tukk name;
	dword key[4];
};

NTL_MOVEABLE(KeyInfo)

void RegisterKeyBinding(tukk group, tukk id, KeyInfo& (*info)());

KeyInfo& AK_NULL();

void   EditKeys();

Txt StoreKeys();
void   RestoreKeys(const Txt& data);

dword ParseKeyDesc(CParser& p);

Txt GetDesc(const KeyInfo& f, bool parenthesis = true);

bool   Match(const KeyInfo& k, dword key);

inline
bool   Match(KeyInfo& (*k)(), dword key) { return Match((*k)(), key); }
