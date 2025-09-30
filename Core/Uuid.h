struct Uuid : AssignValTypeNo<Uuid, 50, Moveable<Uuid> > {
	zu64 v[2];

	void Serialize(Stream& s);
	void Xmlize(XmlIO& xio);
	void Jsonize(JsonIO& jio);
	bool IsNullInstance() const   { return (v[0] | v[1]) == 0; }
	void SetNull()                { v[0] = v[1] = 0; }

	operator   Val() const        { return RichToVal(*this); }
	Uuid(const Val& q)          { *this = q.Get<Uuid>(); }
	Uuid(const Nuller&)           { SetNull(); }
	Uuid()                        {}

	hash_t   GetHashVal() const { return CombineHash(v[0], v[1]); }
	Txt      ToTxt() const;
	Txt      ToTxtWithDashes() const;
	
	void     New();

	static Uuid Create()          { Uuid uuid; uuid.New(); return uuid; }
};

Txt    Format(const Uuid& id);
Txt    FormatWithDashes(const Uuid& id);
Uuid   ScanUuid(tukk s);

inline bool  operator==(const Uuid& u, const Uuid& w) {
	return ((u.v[0] ^ w.v[0]) | (u.v[1] ^ w.v[1])) == 0;
}

inline bool  operator!=(const Uuid& u, const Uuid& w) {
	return !(u == w);
}

template<>
inline Txt AsTxt(const Uuid& id) { return Format(id); }

ValGen& UuidValGen();
