#ifndef _VTInStream_h_
#define _VTInStream_h_

#include <drx/Core/Core.h>

// VTInStream: A VT500 series "lexical" parser for DEC & ANSI escape sequences.
// This parser is based on the UML state diagram provided by Paul-Flo Williams.
// See: https://vt100.net/emu/dec_ansi_parser

// Deviations from the DEC STD-070:
// 1) ISO 8613-6: 0x3a ("colon") is considered as a legitimate delimiter.
// 2) The OSC sequences allow UTF-8 payload if the UTF-8 mode is enabled.

namespace drx {

class VTInStream : public MemReadStream {
public:
    struct Sequence {
        enum Type : u8 { NUL = 0, ESC, CSI, DCS, OSC, APC, PM, SOS };
        u8            type;
        u8            opcode;
        u8            mode;
        u8            intermediate[4];
        Vec<Txt>  parameters;
        Txt          payload;
        i32             GetInt(i32 n, i32 d = 1) const;
        Txt          GetStr(i32 n) const;
        Txt          ToTxt() const;
        dword           GetHashVal() const;
        void            Clear();
        Sequence()                                          { Clear(); }
    };
    
    struct State : Moveable<State> {
        enum  class Id : u8 {
            Ground,
            EscEntry,
            EscIntermediate,
            CsiEntry,
            CsiIntermediate,
            CsiParameter,
            CsiIgnore,
            DcsEntry,
            DcsIntermediate,
            DcsParameter,
            DcsIgnore,
            DcsPassthrough,
            OscTxt,
            ApcTxt,
            Repeat,
            Ignore
        };

        enum class Action : u8 {
            Mode,
            Collect,
            Parameter,
            Final,
            Control,
            Passthrough,
            Txt,
            Ignore,
            Ground,
            DispatchEsc,
            DispatchCsi,
            DispatchDcs,
            DispatchOsc,
            DispatchApc
        };
 
        u8    begin;
        u8    end;
        Action  action;
        Id      next;
        
        static const State& GetVoid();

        State(u8 b, u8 e, Action a, Id id)
        : begin(b)
        , end(e)
        , action(a)
        , next(id)
        {
        }
    };
    
public:
    void    Parse(ukk data, i32 size, bool utf8);
    void    Parse(const Txt& data, bool utf8)            { Parse(~data, data.GetLength(), utf8); }
    void    Reset();
    bool    WasChr() const                                  { return waschr; }
    
    Event<i32>  WhenChr;
    Event<u8> WhenCtl;
    Event<const VTInStream::Sequence&>  WhenEsc;
    Event<const VTInStream::Sequence&>  WhenCsi;
    Event<const VTInStream::Sequence&>  WhenDcs;
    Event<const VTInStream::Sequence&>  WhenOsc;
    Event<const VTInStream::Sequence&>  WhenApc;
    
    static constexpr dword Hash32(u8 h)               { return 0 ^ h; }
    template<typename... Args>
    static constexpr dword Hash32(u8 h, Args... args) { return (0xacf34ce7 * Hash32(args...)) ^ h; }

    VTInStream();
    virtual ~VTInStream() {}
    
private:
    i32             GetChr();
    void            CheckLoadData(tukk data, i32 size, Txt& err);
    void            NextState(State::Id sid);
    const State*    GetState(i32k& c) const;
    void            Dispatch(u8 type, const Event<const VTInStream::Sequence&>& fn);
    void            Reset0(const Vec<VTInStream::State>* st);
    
    // Collectors.
    void            CollectChr(i32 c);
    void            CollectIntermediate(i32 c);
    void            CollectParameter(i32 c);
    void            CollectPayload(i32 c);
    void            CollectTxt(i32 c);
    
private:
    Sequence    sequence;
    bool        waschr;
    bool        utf8mode;
    Txt      collected;
    Txt      buffer;
    const Vec<VTInStream::State>*  state;
};
}
#endif