typedef std::atomic<i32> Atomic;

inline i32  AtomicInc(volatile Atomic& t)             { return ++t; }
inline i32  AtomicDec(volatile Atomic& t)             { return --t; }
