/* inflate.h -- internal inflate state definition
 * Copyright (C) 1995-2019 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef INFLATE_H
#define INFLATE_H

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* define NO_GZIP when compiling if you want to disable gzip header and
   trailer decoding by inflate().  NO_GZIP would be used to avoid linking in
   the crc code when it is not needed.  For shared libraries, gzip decoding
   should be left enabled. */
#ifndef NO_GZIP
#  define GUNZIP
#endif

/* Possible inflate modes between inflate() calls */
typedef enum {
    HEAD = 16180,   /* i: waiting for magic header */
    FLAGS,      /* i: waiting for method and flags (gzip) */
    TIME,       /* i: waiting for modification time (gzip) */
    OS,         /* i: waiting for extra flags and operating system (gzip) */
    EXLEN,      /* i: waiting for extra length (gzip) */
    EXTRA,      /* i: waiting for extra bytes (gzip) */
    NAME,       /* i: waiting for end of file name (gzip) */
    COMMENT,    /* i: waiting for end of comment (gzip) */
    HCRC,       /* i: waiting for header crc (gzip) */
    DICTID,     /* i: waiting for dictionary check value */
    DICT,       /* waiting for inflateSetDictionary() call */
        TYPE,       /* i: waiting for type bits, including last-flag bit */
        TYPEDO,     /* i: same, but skip check to exit inflate on new block */
        STORED,     /* i: waiting for stored size (length and complement) */
        COPY_,      /* i/o: same as COPY below, but only first time in */
        COPY,       /* i/o: waiting for input or output to copy stored block */
        TABLE,      /* i: waiting for dynamic block table lengths */
        LENLENS,    /* i: waiting for code length code lengths */
        CODELENS,   /* i: waiting for length/lit and distance code lengths */
            LEN_,       /* i: same as LEN below, but only first time in */
            LEN,        /* i: waiting for length/lit/eob code */
            LENEXT,     /* i: waiting for length extra bits */
            DIST,       /* i: waiting for distance code */
            DISTEXT,    /* i: waiting for distance extra bits */
            MATCH,      /* o: waiting for output space to copy string */
            LIT,        /* o: waiting for output space to write literal */
    CHECK,      /* i: waiting for 32-bit check value */
    LENGTH,     /* i: waiting for 32-bit length (gzip) */
    DONE,       /* finished check, done -- remain here until reset */
    BAD,        /* got a data error -- remain here until reset */
    MEM,        /* got an inflate() memory error -- remain here until reset */
    SYNC        /* looking for synchronization bytes to restart inflate() */
} inflate_mode;

/*
    State transitions between above modes -

    (most modes can go to BAD or MEM on error -- not shown for clarity)

    Process header:
        HEAD -> (gzip) or (zlib) or (raw)
        (gzip) -> FLAGS -> TIME -> OS -> EXLEN -> EXTRA -> NAME -> COMMENT ->
                  HCRC -> TYPE
        (zlib) -> DICTID or TYPE
        DICTID -> DICT -> TYPE
        (raw) -> TYPEDO
    Read deflate blocks:
            TYPE -> TYPEDO -> STORED or TABLE or LEN_ or CHECK
            STORED -> COPY_ -> COPY -> TYPE
            TABLE -> LENLENS -> CODELENS -> LEN_
            LEN_ -> LEN
    Read deflate codes in fixed or dynamic block:
                LEN -> LENEXT or LIT or TYPE
                LENEXT -> DIST -> DISTEXT -> MATCH -> LEN
                LIT -> LEN
    Process trailer:
        CHECK -> LENGTH -> DONE
 */

/* State maintained between inflate() calls -- approximately 7K bytes, not
   including the allocated sliding window, which is up to 32K bytes. */
struct inflate_state {
    z_streamp strm;             /* pointer back to this zlib stream */
    inflate_mode mode;          /* current inflate mode */
    i32 last;                   /* true if processing last block */
    i32 wrap;                   /* bit 0 true for zlib, bit 1 true for gzip,
                                   bit 2 true to validate check value */
    i32 havedict;               /* true if dictionary provided */
    i32 flags;                  /* gzip header method and flags, 0 if zlib, or
                                   -1 if raw or no header yet */
    u32 dmax;              /* zlib header max distance (INFLATE_STRICT) */
    u64 check;        /* protected copy of check value */
    u64 total;        /* protected copy of output count */
    gz_headerp head;            /* where to save gzip header information */
        /* sliding window */
    u32 wbits;             /* log base 2 of requested window size */
    u32 wsize;             /* window size or zero if not using window */
    u32 whave;             /* valid bytes in the window */
    u32 wnext;             /* window write index */
    u8 FAR *window;  /* allocated sliding window, if needed */
        /* bit accumulator */
    u64 hold;         /* input bit accumulator */
    u32 bits;              /* number of bits in "in" */
        /* for string and stored block copying */
    u32 length;            /* literal or length of data to copy */
    u32 offset;            /* distance back to copy string from */
        /* for table and code decoding */
    u32 extra;             /* extra bits needed */
        /* fixed and dynamic code tables */
    code const FAR *lencode;    /* starting table for length/literal codes */
    code const FAR *distcode;   /* starting table for distance codes */
    u32 lenbits;           /* index bits for lencode */
    u32 distbits;          /* index bits for distcode */
        /* dynamic table building */
    u32 ncode;             /* number of code length code lengths */
    u32 nlen;              /* number of length code lengths */
    u32 ndist;             /* number of distance code lengths */
    u32 have;              /* number of code lengths in lens[] */
    code FAR *next;             /* next available space in codes[] */
    u16 lens[320];   /* temporary storage for code lengths */
    u16 work[288];   /* work area for code table building */
    code codes[ENOUGH];         /* space for code tables */
    i32 sane;                   /* if false, allow invalid distance too far */
    i32 back;                   /* bits back of last unprocessed length/lit */
    u32 was;               /* initial length of match */
};

#endif  /* INFLATE_H */
