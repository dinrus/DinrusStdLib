/* ******************************************************************
 * Huffman encoder, part of New Generation Entropy library
 * Copyright (c) 2013-2020, Yann Collet, Facebook, Inc.
 *
 *  You can contact the author at :
 *  - FSE+HUF source repository : https://github.com/Cyan4973/FiniteStateEntropy
 *  - Public forum : https://groups.google.com/forum/#!forum/lz4c
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
****************************************************************** */

/* **************************************************************
*  Compiler specifics
****************************************************************/
#ifdef _MSC_VER    /* Visual Studio */
#  pragma warning(disable : 4127)        /* disable: C4127: conditional expression is constant */
#endif


/* **************************************************************
*  Includes
****************************************************************/
#include <string.h>     /* memcpy, memset */
#include <stdio.h>      /* printf (debug) */
#include "compiler.h"
#include "bitstream.h"
#include "hist.h"
#define FSE_STATIC_LINKING_ONLY   /* FSE_optimalTableLog_internal */
#include "fse.h"        /* header compression */
#define HUF_STATIC_LINKING_ONLY
#include "huf.h"
#include "error_private.h"


/* **************************************************************
*  Error Management
****************************************************************/
#define HUF_isError ERR_isError
#define HUF_STATIC_ASSERT(c) DEBUG_STATIC_ASSERT(c)   /* use only *after* variable declarations */


/* **************************************************************
*  Utils
****************************************************************/
u32 HUF_optimalTableLog(u32 maxTableLog, size_t srcSize, u32 maxSymbolVal)
{
    return FSE_optimalTableLog_internal(maxTableLog, srcSize, maxSymbolVal, 1);
}


/* *******************************************************
*  HUF : Huffman block compression
*********************************************************/
/* HUF_compressWeights() :
 * Same as FSE_compress(), but dedicated to huff0's weights compression.
 * The use case needs much less stack memory.
 * Note : all elements within weightTable are supposed to be <= HUF_TABLELOG_MAX.
 */
#define MAX_FSE_TABLELOG_FOR_HUFF_HEADER 6
static size_t HUF_compressWeights (uk dst, size_t dstSize, ukk weightTable, size_t wtSize)
{
    BYTE* const ostart = (BYTE*) dst;
    BYTE* op = ostart;
    BYTE* const oend = ostart + dstSize;

    u32 maxSymbolVal = HUF_TABLELOG_MAX;
    U32 tableLog = MAX_FSE_TABLELOG_FOR_HUFF_HEADER;

    FSE_CTable CTable[FSE_CTABLE_SIZE_U32(MAX_FSE_TABLELOG_FOR_HUFF_HEADER, HUF_TABLELOG_MAX)];
    BYTE scratchBuffer[1<<MAX_FSE_TABLELOG_FOR_HUFF_HEADER];

    u32 count[HUF_TABLELOG_MAX+1];
    S16 norm[HUF_TABLELOG_MAX+1];

    /* init conditions */
    if (wtSize <= 1) return 0;  /* Not compressible */

    /* Scan input and build symbol stats */
    {   u32 const maxCount = HIST_count_simple(count, &maxSymbolVal, weightTable, wtSize);   /* never fails */
        if (maxCount == wtSize) return 1;   /* only a single symbol in src : rle */
        if (maxCount == 1) return 0;        /* each symbol present maximum once => not compressible */
    }

    tableLog = FSE_optimalTableLog(tableLog, wtSize, maxSymbolVal);
    CHECK_F( FSE_normalizeCount(norm, tableLog, count, wtSize, maxSymbolVal) );

    /* Write table description header */
    {   CHECK_V_F(hSize, FSE_writeNCount(op, (size_t)(oend-op), norm, maxSymbolVal, tableLog) );
        op += hSize;
    }

    /* Compress */
    CHECK_F( FSE_buildCTable_wksp(CTable, norm, maxSymbolVal, tableLog, scratchBuffer, sizeof(scratchBuffer)) );
    {   CHECK_V_F(cSize, FSE_compress_usingCTable(op, (size_t)(oend - op), weightTable, wtSize, CTable) );
        if (cSize == 0) return 0;   /* not enough space for compressed data */
        op += cSize;
    }

    return (size_t)(op-ostart);
}


struct HUF_CElt_s {
  U16  val;
  BYTE nbBits;
};   /* typedef'd to HUF_CElt within "huf.h" */

/*! HUF_writeCTable() :
    `CTable` : Huffman tree to save, using huf representation.
    @return : size of saved CTable */
size_t HUF_writeCTable (uk dst, size_t maxDstSize,
                        const HUF_CElt* CTable, u32 maxSymbolVal, u32 huffLog)
{
    BYTE bitsToWeight[HUF_TABLELOG_MAX + 1];   /* precomputed conversion table */
    BYTE huffWeight[HUF_SYMBOLVALUE_MAX];
    BYTE* op = (BYTE*)dst;
    U32 n;

     /* check conditions */
    if (maxSymbolVal > HUF_SYMBOLVALUE_MAX) return ERROR(maxSymbolVal_tooLarge);

    /* convert to weight */
    bitsToWeight[0] = 0;
    for (n=1; n<huffLog+1; n++)
        bitsToWeight[n] = (BYTE)(huffLog + 1 - n);
    for (n=0; n<maxSymbolVal; n++)
        huffWeight[n] = bitsToWeight[CTable[n].nbBits];

    /* attempt weights compression by FSE */
    {   CHECK_V_F(hSize, HUF_compressWeights(op+1, maxDstSize-1, huffWeight, maxSymbolVal) );
        if ((hSize>1) & (hSize < maxSymbolVal/2)) {   /* FSE compressed */
            op[0] = (BYTE)hSize;
            return hSize+1;
    }   }

    /* write raw values as 4-bits (max : 15) */
    if (maxSymbolVal > (256-128)) return ERROR(GENERIC);   /* should not happen : likely means source cannot be compressed */
    if (((maxSymbolVal+1)/2) + 1 > maxDstSize) return ERROR(dstSize_tooSmall);   /* not enough space within dst buffer */
    op[0] = (BYTE)(128 /*special case*/ + (maxSymbolVal-1));
    huffWeight[maxSymbolVal] = 0;   /* to be sure it doesn't cause msan issue in final combination */
    for (n=0; n<maxSymbolVal; n+=2)
        op[(n/2)+1] = (BYTE)((huffWeight[n] << 4) + huffWeight[n+1]);
    return ((maxSymbolVal+1)/2) + 1;
}


size_t HUF_readCTable (HUF_CElt* CTable, u32* maxSymbolValPtr, ukk src, size_t srcSize, u32* hasZeroWeights)
{
    BYTE huffWeight[HUF_SYMBOLVALUE_MAX + 1];   /* init not required, even though some static analyzer may complain */
    U32 rankVal[HUF_TABLELOG_ABSOLUTEMAX + 1];   /* large enough for values from 0 to 16 */
    U32 tableLog = 0;
    U32 nbSymbols = 0;

    /* get symbol weights */
    CHECK_V_F(readSize, HUF_readStats(huffWeight, HUF_SYMBOLVALUE_MAX+1, rankVal, &nbSymbols, &tableLog, src, srcSize));

    /* check result */
    if (tableLog > HUF_TABLELOG_MAX) return ERROR(tableLog_tooLarge);
    if (nbSymbols > *maxSymbolValPtr+1) return ERROR(maxSymbolVal_tooSmall);

    /* Prepare base value per rank */
    {   U32 n, nextRankStart = 0;
        for (n=1; n<=tableLog; n++) {
            U32 current = nextRankStart;
            nextRankStart += (rankVal[n] << (n-1));
            rankVal[n] = current;
    }   }

    /* fill nbBits */
    *hasZeroWeights = 0;
    {   U32 n; for (n=0; n<nbSymbols; n++) {
            const U32 w = huffWeight[n];
            *hasZeroWeights |= (w == 0);
            CTable[n].nbBits = (BYTE)(tableLog + 1 - w) & -(w != 0);
    }   }

    /* fill val */
    {   U16 nbPerRank[HUF_TABLELOG_MAX+2]  = {0};  /* support w=0=>n=tableLog+1 */
        U16 valPerRank[HUF_TABLELOG_MAX+2] = {0};
        { U32 n; for (n=0; n<nbSymbols; n++) nbPerRank[CTable[n].nbBits]++; }
        /* determine stating value per rank */
        valPerRank[tableLog+1] = 0;   /* for w==0 */
        {   U16 min = 0;
            U32 n; for (n=tableLog; n>0; n--) {  /* start at n=tablelog <-> w=1 */
                valPerRank[n] = min;     /* get starting value within each rank */
                min += nbPerRank[n];
                min >>= 1;
        }   }
        /* assign value within rank, symbol order */
        { U32 n; for (n=0; n<nbSymbols; n++) CTable[n].val = valPerRank[CTable[n].nbBits]++; }
    }

    *maxSymbolValPtr = nbSymbols - 1;
    return readSize;
}

U32 HUF_getNbBits(ukk symbolTable, U32 symbolVal)
{
    const HUF_CElt* table = (const HUF_CElt*)symbolTable;
    assert(symbolVal <= HUF_SYMBOLVALUE_MAX);
    return table[symbolVal].nbBits;
}


typedef struct nodeElt_s {
    U32 count;
    U16 parent;
    BYTE u8;
    BYTE nbBits;
} nodeElt;

static U32 HUF_setMaxHeight(nodeElt* huffNode, U32 lastNonNull, U32 maxNbBits)
{
    const U32 largestBits = huffNode[lastNonNull].nbBits;
    if (largestBits <= maxNbBits) return largestBits;   /* early exit : no elt > maxNbBits */

    /* there are several too large elements (at least >= 2) */
    {   i32 totalCost = 0;
        const U32 baseCost = 1 << (largestBits - maxNbBits);
        i32 n = (i32)lastNonNull;

        while (huffNode[n].nbBits > maxNbBits) {
            totalCost += baseCost - (1 << (largestBits - huffNode[n].nbBits));
            huffNode[n].nbBits = (BYTE)maxNbBits;
            n --;
        }  /* n stops at huffNode[n].nbBits <= maxNbBits */
        while (huffNode[n].nbBits == maxNbBits) n--;   /* n end at index of smallest symbol using < maxNbBits */

        /* renorm totalCost */
        totalCost >>= (largestBits - maxNbBits);  /* note : totalCost is necessarily a multiple of baseCost */

        /* repay normalized cost */
        {   U32 const noSymbol = 0xF0F0F0F0;
            U32 rankLast[HUF_TABLELOG_MAX+2];

            /* Get pos of last (smallest) symbol per rank */
            memset(rankLast, 0xF0, sizeof(rankLast));
            {   U32 currentNbBits = maxNbBits;
                i32 pos;
                for (pos=n ; pos >= 0; pos--) {
                    if (huffNode[pos].nbBits >= currentNbBits) continue;
                    currentNbBits = huffNode[pos].nbBits;   /* < maxNbBits */
                    rankLast[maxNbBits-currentNbBits] = (U32)pos;
            }   }

            while (totalCost > 0) {
                U32 nBitsToDecrease = BIT_highbit32((U32)totalCost) + 1;
                for ( ; nBitsToDecrease > 1; nBitsToDecrease--) {
                    U32 const highPos = rankLast[nBitsToDecrease];
                    U32 const lowPos = rankLast[nBitsToDecrease-1];
                    if (highPos == noSymbol) continue;
                    if (lowPos == noSymbol) break;
                    {   U32 const highTotal = huffNode[highPos].count;
                        U32 const lowTotal = 2 * huffNode[lowPos].count;
                        if (highTotal <= lowTotal) break;
                }   }
                /* only triggered when no more rank 1 symbol left => find closest one (note : there is necessarily at least one !) */
                /* HUF_MAX_TABLELOG test just to please gcc 5+; but it should not be necessary */
                while ((nBitsToDecrease<=HUF_TABLELOG_MAX) && (rankLast[nBitsToDecrease] == noSymbol))
                    nBitsToDecrease ++;
                totalCost -= 1 << (nBitsToDecrease-1);
                if (rankLast[nBitsToDecrease-1] == noSymbol)
                    rankLast[nBitsToDecrease-1] = rankLast[nBitsToDecrease];   /* this rank is no longer empty */
                huffNode[rankLast[nBitsToDecrease]].nbBits ++;
                if (rankLast[nBitsToDecrease] == 0)    /* special case, reached largest symbol */
                    rankLast[nBitsToDecrease] = noSymbol;
                else {
                    rankLast[nBitsToDecrease]--;
                    if (huffNode[rankLast[nBitsToDecrease]].nbBits != maxNbBits-nBitsToDecrease)
                        rankLast[nBitsToDecrease] = noSymbol;   /* this rank is now empty */
            }   }   /* while (totalCost > 0) */

            while (totalCost < 0) {  /* Sometimes, cost correction overshoot */
                if (rankLast[1] == noSymbol) {  /* special case : no rank 1 symbol (using maxNbBits-1); let's create one from largest rank 0 (using maxNbBits) */
                    while (huffNode[n].nbBits == maxNbBits) n--;
                    huffNode[n+1].nbBits--;
                    assert(n >= 0);
                    rankLast[1] = (U32)(n+1);
                    totalCost++;
                    continue;
                }
                huffNode[ rankLast[1] + 1 ].nbBits--;
                rankLast[1]++;
                totalCost ++;
    }   }   }   /* there are several too large elements (at least >= 2) */

    return maxNbBits;
}

typedef struct {
    U32 base;
    U32 current;
} rankPos;

typedef nodeElt huffNodeTable[HUF_CTABLE_WORKSPACE_SIZE_U32];

#define RANK_POSITION_TABLE_SIZE 32

typedef struct {
  huffNodeTable huffNodeTbl;
  rankPos rankPosition[RANK_POSITION_TABLE_SIZE];
} HUF_buildCTable_wksp_tables;

static void HUF_sort(nodeElt* huffNode, const u32* count, U32 maxSymbolVal, rankPos* rankPosition)
{
    U32 n;

    memset(rankPosition, 0, sizeof(*rankPosition) * RANK_POSITION_TABLE_SIZE);
    for (n=0; n<=maxSymbolVal; n++) {
        U32 r = BIT_highbit32(count[n] + 1);
        rankPosition[r].base ++;
    }
    for (n=30; n>0; n--) rankPosition[n-1].base += rankPosition[n].base;
    for (n=0; n<32; n++) rankPosition[n].current = rankPosition[n].base;
    for (n=0; n<=maxSymbolVal; n++) {
        U32 const c = count[n];
        U32 const r = BIT_highbit32(c+1) + 1;
        U32 pos = rankPosition[r].current++;
        while ((pos > rankPosition[r].base) && (c > huffNode[pos-1].count)) {
            huffNode[pos] = huffNode[pos-1];
            pos--;
        }
        huffNode[pos].count = c;
        huffNode[pos].u8  = (BYTE)n;
    }
}


/** HUF_buildCTable_wksp() :
 *  Same as HUF_buildCTable(), but using externally allocated scratch buffer.
 *  `workSpace` must be aligned on 4-bytes boundaries, and be at least as large as sizeof(HUF_buildCTable_wksp_tables).
 */
#define STARTNODE (HUF_SYMBOLVALUE_MAX+1)

size_t HUF_buildCTable_wksp (HUF_CElt* tree, const u32* count, U32 maxSymbolVal, U32 maxNbBits, uk workSpace, size_t wkspSize)
{
    HUF_buildCTable_wksp_tables* const wksp_tables = (HUF_buildCTable_wksp_tables*)workSpace;
    nodeElt* const huffNode0 = wksp_tables->huffNodeTbl;
    nodeElt* const huffNode = huffNode0+1;
    i32 nonNullRank;
    i32 lowS, lowN;
    i32 nodeNb = STARTNODE;
    i32 n, nodeRoot;

    /* safety checks */
    if (((size_t)workSpace & 3) != 0) return ERROR(GENERIC);  /* must be aligned on 4-bytes boundaries */
    if (wkspSize < sizeof(HUF_buildCTable_wksp_tables))
      return ERROR(workSpace_tooSmall);
    if (maxNbBits == 0) maxNbBits = HUF_TABLELOG_DEFAULT;
    if (maxSymbolVal > HUF_SYMBOLVALUE_MAX)
      return ERROR(maxSymbolVal_tooLarge);
    memset(huffNode0, 0, sizeof(huffNodeTable));

    /* sort, decreasing order */
    HUF_sort(huffNode, count, maxSymbolVal, wksp_tables->rankPosition);

    /* init for parents */
    nonNullRank = (i32)maxSymbolVal;
    while(huffNode[nonNullRank].count == 0) nonNullRank--;
    lowS = nonNullRank; nodeRoot = nodeNb + lowS - 1; lowN = nodeNb;
    huffNode[nodeNb].count = huffNode[lowS].count + huffNode[lowS-1].count;
    huffNode[lowS].parent = huffNode[lowS-1].parent = (U16)nodeNb;
    nodeNb++; lowS-=2;
    for (n=nodeNb; n<=nodeRoot; n++) huffNode[n].count = (U32)(1U<<30);
    huffNode0[0].count = (U32)(1U<<31);  /* fake entry, strong barrier */

    /* create parents */
    while (nodeNb <= nodeRoot) {
        i32 const n1 = (huffNode[lowS].count < huffNode[lowN].count) ? lowS-- : lowN++;
        i32 const n2 = (huffNode[lowS].count < huffNode[lowN].count) ? lowS-- : lowN++;
        huffNode[nodeNb].count = huffNode[n1].count + huffNode[n2].count;
        huffNode[n1].parent = huffNode[n2].parent = (U16)nodeNb;
        nodeNb++;
    }

    /* distribute weights (unlimited tree height) */
    huffNode[nodeRoot].nbBits = 0;
    for (n=nodeRoot-1; n>=STARTNODE; n--)
        huffNode[n].nbBits = huffNode[ huffNode[n].parent ].nbBits + 1;
    for (n=0; n<=nonNullRank; n++)
        huffNode[n].nbBits = huffNode[ huffNode[n].parent ].nbBits + 1;

    /* enforce maxTableLog */
    maxNbBits = HUF_setMaxHeight(huffNode, (U32)nonNullRank, maxNbBits);

    /* fill result into tree (val, nbBits) */
    {   U16 nbPerRank[HUF_TABLELOG_MAX+1] = {0};
        U16 valPerRank[HUF_TABLELOG_MAX+1] = {0};
        i32 const alphabetSize = (i32)(maxSymbolVal + 1);
        if (maxNbBits > HUF_TABLELOG_MAX) return ERROR(GENERIC);   /* check fit into table */
        for (n=0; n<=nonNullRank; n++)
            nbPerRank[huffNode[n].nbBits]++;
        /* determine stating value per rank */
        {   U16 min = 0;
            for (n=(i32)maxNbBits; n>0; n--) {
                valPerRank[n] = min;      /* get starting value within each rank */
                min += nbPerRank[n];
                min >>= 1;
        }   }
        for (n=0; n<alphabetSize; n++)
            tree[huffNode[n].u8].nbBits = huffNode[n].nbBits;   /* push nbBits per symbol, symbol order */
        for (n=0; n<alphabetSize; n++)
            tree[n].val = valPerRank[tree[n].nbBits]++;   /* assign value within rank, symbol order */
    }

    return maxNbBits;
}

/** HUF_buildCTable() :
 * @return : maxNbBits
 *  Note : count is used before tree is written, so they can safely overlap
 */
size_t HUF_buildCTable (HUF_CElt* tree, const u32* count, u32 maxSymbolVal, u32 maxNbBits)
{
    HUF_buildCTable_wksp_tables workspace;
    return HUF_buildCTable_wksp(tree, count, maxSymbolVal, maxNbBits, &workspace, sizeof(workspace));
}

size_t HUF_estimateCompressedSize(const HUF_CElt* CTable, const u32* count, u32 maxSymbolVal)
{
    size_t nbBits = 0;
    i32 s;
    for (s = 0; s <= (i32)maxSymbolVal; ++s) {
        nbBits += CTable[s].nbBits * count[s];
    }
    return nbBits >> 3;
}

i32 HUF_validateCTable(const HUF_CElt* CTable, const u32* count, u32 maxSymbolVal) {
  i32 bad = 0;
  i32 s;
  for (s = 0; s <= (i32)maxSymbolVal; ++s) {
    bad |= (count[s] != 0) & (CTable[s].nbBits == 0);
  }
  return !bad;
}

size_t HUF_compressBound(size_t size) { return HUF_COMPRESSBOUND(size); }

FORCE_INLINE_TEMPLATE void
HUF_encodeSymbol(BIT_CStream_t* bitCPtr, U32 symbol, const HUF_CElt* CTable)
{
    BIT_addBitsFast(bitCPtr, CTable[symbol].val, CTable[symbol].nbBits);
}

#define HUF_FLUSHBITS(s)  BIT_flushBits(s)

#define HUF_FLUSHBITS_1(stream) \
    if (sizeof((stream)->bitContainer)*8 < HUF_TABLELOG_MAX*2+7) HUF_FLUSHBITS(stream)

#define HUF_FLUSHBITS_2(stream) \
    if (sizeof((stream)->bitContainer)*8 < HUF_TABLELOG_MAX*4+7) HUF_FLUSHBITS(stream)

FORCE_INLINE_TEMPLATE size_t
HUF_compress1X_usingCTable_internal_body(uk dst, size_t dstSize,
                                   ukk src, size_t srcSize,
                                   const HUF_CElt* CTable)
{
    const BYTE* ip = (const BYTE*) src;
    BYTE* const ostart = (BYTE*)dst;
    BYTE* const oend = ostart + dstSize;
    BYTE* op = ostart;
    size_t n;
    BIT_CStream_t bitC;

    /* init */
    if (dstSize < 8) return 0;   /* not enough space to compress */
    { size_t const initErr = BIT_initCStream(&bitC, op, (size_t)(oend-op));
      if (HUF_isError(initErr)) return 0; }

    n = srcSize & ~3;  /* join to mod 4 */
    switch (srcSize & 3)
    {
        case 3 : HUF_encodeSymbol(&bitC, ip[n+ 2], CTable);
                 HUF_FLUSHBITS_2(&bitC);
		 /* fall-through */
        case 2 : HUF_encodeSymbol(&bitC, ip[n+ 1], CTable);
                 HUF_FLUSHBITS_1(&bitC);
		 /* fall-through */
        case 1 : HUF_encodeSymbol(&bitC, ip[n+ 0], CTable);
                 HUF_FLUSHBITS(&bitC);
		 /* fall-through */
        case 0 : /* fall-through */
        default: break;
    }

    for (; n>0; n-=4) {  /* note : n&3==0 at this stage */
        HUF_encodeSymbol(&bitC, ip[n- 1], CTable);
        HUF_FLUSHBITS_1(&bitC);
        HUF_encodeSymbol(&bitC, ip[n- 2], CTable);
        HUF_FLUSHBITS_2(&bitC);
        HUF_encodeSymbol(&bitC, ip[n- 3], CTable);
        HUF_FLUSHBITS_1(&bitC);
        HUF_encodeSymbol(&bitC, ip[n- 4], CTable);
        HUF_FLUSHBITS(&bitC);
    }

    return BIT_closeCStream(&bitC);
}

#if DYNAMIC_BMI2

static TARGET_ATTRIBUTE("bmi2") size_t
HUF_compress1X_usingCTable_internal_bmi2(uk dst, size_t dstSize,
                                   ukk src, size_t srcSize,
                                   const HUF_CElt* CTable)
{
    return HUF_compress1X_usingCTable_internal_body(dst, dstSize, src, srcSize, CTable);
}

static size_t
HUF_compress1X_usingCTable_internal_default(uk dst, size_t dstSize,
                                      ukk src, size_t srcSize,
                                      const HUF_CElt* CTable)
{
    return HUF_compress1X_usingCTable_internal_body(dst, dstSize, src, srcSize, CTable);
}

static size_t
HUF_compress1X_usingCTable_internal(uk dst, size_t dstSize,
                              ukk src, size_t srcSize,
                              const HUF_CElt* CTable, i32k bmi2)
{
    if (bmi2) {
        return HUF_compress1X_usingCTable_internal_bmi2(dst, dstSize, src, srcSize, CTable);
    }
    return HUF_compress1X_usingCTable_internal_default(dst, dstSize, src, srcSize, CTable);
}

#else

static size_t
HUF_compress1X_usingCTable_internal(uk dst, size_t dstSize,
                              ukk src, size_t srcSize,
                              const HUF_CElt* CTable, i32k bmi2)
{
    (void)bmi2;
    return HUF_compress1X_usingCTable_internal_body(dst, dstSize, src, srcSize, CTable);
}

#endif

size_t HUF_compress1X_usingCTable(uk dst, size_t dstSize, ukk src, size_t srcSize, const HUF_CElt* CTable)
{
    return HUF_compress1X_usingCTable_internal(dst, dstSize, src, srcSize, CTable, /* bmi2 */ 0);
}


static size_t
HUF_compress4X_usingCTable_internal(uk dst, size_t dstSize,
                              ukk src, size_t srcSize,
                              const HUF_CElt* CTable, i32 bmi2)
{
    size_t const segmentSize = (srcSize+3)/4;   /* first 3 segments */
    const BYTE* ip = (const BYTE*) src;
    const BYTE* const iend = ip + srcSize;
    BYTE* const ostart = (BYTE*) dst;
    BYTE* const oend = ostart + dstSize;
    BYTE* op = ostart;

    if (dstSize < 6 + 1 + 1 + 1 + 8) return 0;   /* minimum space to compress successfully */
    if (srcSize < 12) return 0;   /* no saving possible : too small input */
    op += 6;   /* jumpTable */

    assert(op <= oend);
    {   CHECK_V_F(cSize, HUF_compress1X_usingCTable_internal(op, (size_t)(oend-op), ip, segmentSize, CTable, bmi2) );
        if (cSize==0) return 0;
        assert(cSize <= 65535);
        MEM_writeLE16(ostart, (U16)cSize);
        op += cSize;
    }

    ip += segmentSize;
    assert(op <= oend);
    {   CHECK_V_F(cSize, HUF_compress1X_usingCTable_internal(op, (size_t)(oend-op), ip, segmentSize, CTable, bmi2) );
        if (cSize==0) return 0;
        assert(cSize <= 65535);
        MEM_writeLE16(ostart+2, (U16)cSize);
        op += cSize;
    }

    ip += segmentSize;
    assert(op <= oend);
    {   CHECK_V_F(cSize, HUF_compress1X_usingCTable_internal(op, (size_t)(oend-op), ip, segmentSize, CTable, bmi2) );
        if (cSize==0) return 0;
        assert(cSize <= 65535);
        MEM_writeLE16(ostart+4, (U16)cSize);
        op += cSize;
    }

    ip += segmentSize;
    assert(op <= oend);
    assert(ip <= iend);
    {   CHECK_V_F(cSize, HUF_compress1X_usingCTable_internal(op, (size_t)(oend-op), ip, (size_t)(iend-ip), CTable, bmi2) );
        if (cSize==0) return 0;
        op += cSize;
    }

    return (size_t)(op-ostart);
}

size_t HUF_compress4X_usingCTable(uk dst, size_t dstSize, ukk src, size_t srcSize, const HUF_CElt* CTable)
{
    return HUF_compress4X_usingCTable_internal(dst, dstSize, src, srcSize, CTable, /* bmi2 */ 0);
}

typedef enum { HUF_singleStream, HUF_fourStreams } HUF_nbStreams_e;

static size_t HUF_compressCTable_internal(
                BYTE* const ostart, BYTE* op, BYTE* const oend,
                ukk src, size_t srcSize,
                HUF_nbStreams_e nbStreams, const HUF_CElt* CTable, i32k bmi2)
{
    size_t const cSize = (nbStreams==HUF_singleStream) ?
                         HUF_compress1X_usingCTable_internal(op, (size_t)(oend - op), src, srcSize, CTable, bmi2) :
                         HUF_compress4X_usingCTable_internal(op, (size_t)(oend - op), src, srcSize, CTable, bmi2);
    if (HUF_isError(cSize)) { return cSize; }
    if (cSize==0) { return 0; }   /* uncompressible */
    op += cSize;
    /* check compressibility */
    assert(op >= ostart);
    if ((size_t)(op-ostart) >= srcSize-1) { return 0; }
    return (size_t)(op-ostart);
}

typedef struct {
    u32 count[HUF_SYMBOLVALUE_MAX + 1];
    HUF_CElt CTable[HUF_SYMBOLVALUE_MAX + 1];
    HUF_buildCTable_wksp_tables buildCTable_wksp;
} HUF_compress_tables_t;

/* HUF_compress_internal() :
 * `workSpace` must a table of at least HUF_WORKSPACE_SIZE_U32 u32 */
static size_t
HUF_compress_internal (uk dst, size_t dstSize,
                 ukk src, size_t srcSize,
                       u32 maxSymbolVal, u32 huffLog,
                       HUF_nbStreams_e nbStreams,
                       uk workSpace, size_t wkspSize,
                       HUF_CElt* oldHufTable, HUF_repeat* repeat, i32 preferRepeat,
                 i32k bmi2)
{
    HUF_compress_tables_t* const table = (HUF_compress_tables_t*)workSpace;
    BYTE* const ostart = (BYTE*)dst;
    BYTE* const oend = ostart + dstSize;
    BYTE* op = ostart;

    HUF_STATIC_ASSERT(sizeof(*table) <= HUF_WORKSPACE_SIZE);

    /* checks & inits */
    if (((size_t)workSpace & 3) != 0) return ERROR(GENERIC);  /* must be aligned on 4-bytes boundaries */
    if (wkspSize < HUF_WORKSPACE_SIZE) return ERROR(workSpace_tooSmall);
    if (!srcSize) return 0;  /* Uncompressed */
    if (!dstSize) return 0;  /* cannot fit anything within dst budget */
    if (srcSize > HUF_BLOCKSIZE_MAX) return ERROR(srcSize_wrong);   /* current block size limit */
    if (huffLog > HUF_TABLELOG_MAX) return ERROR(tableLog_tooLarge);
    if (maxSymbolVal > HUF_SYMBOLVALUE_MAX) return ERROR(maxSymbolVal_tooLarge);
    if (!maxSymbolVal) maxSymbolVal = HUF_SYMBOLVALUE_MAX;
    if (!huffLog) huffLog = HUF_TABLELOG_DEFAULT;

    /* Heuristic : If old table is valid, use it for small inputs */
    if (preferRepeat && repeat && *repeat == HUF_repeat_valid) {
        return HUF_compressCTable_internal(ostart, op, oend,
                                           src, srcSize,
                                           nbStreams, oldHufTable, bmi2);
    }

    /* Scan input and build symbol stats */
    {   CHECK_V_F(largest, HIST_count_wksp (table->count, &maxSymbolVal, (const BYTE*)src, srcSize, workSpace, wkspSize) );
        if (largest == srcSize) { *ostart = ((const BYTE*)src)[0]; return 1; }   /* single symbol, rle */
        if (largest <= (srcSize >> 7)+4) return 0;   /* heuristic : probably not compressible enough */
    }

    /* Check validity of previous table */
    if ( repeat
      && *repeat == HUF_repeat_check
      && !HUF_validateCTable(oldHufTable, table->count, maxSymbolVal)) {
        *repeat = HUF_repeat_none;
    }
    /* Heuristic : use existing table for small inputs */
    if (preferRepeat && repeat && *repeat != HUF_repeat_none) {
        return HUF_compressCTable_internal(ostart, op, oend,
                                           src, srcSize,
                                           nbStreams, oldHufTable, bmi2);
    }

    /* Build Huffman Tree */
    huffLog = HUF_optimalTableLog(huffLog, srcSize, maxSymbolVal);
    {   size_t const maxBits = HUF_buildCTable_wksp(table->CTable, table->count,
                                            maxSymbolVal, huffLog,
                                            &table->buildCTable_wksp, sizeof(table->buildCTable_wksp));
        CHECK_F(maxBits);
        huffLog = (U32)maxBits;
        /* Zero unused symbols in CTable, so we can check it for validity */
        memset(table->CTable + (maxSymbolVal + 1), 0,
               sizeof(table->CTable) - ((maxSymbolVal + 1) * sizeof(HUF_CElt)));
    }

    /* Write table description header */
    {   CHECK_V_F(hSize, HUF_writeCTable (op, dstSize, table->CTable, maxSymbolVal, huffLog) );
        /* Check if using previous huffman table is beneficial */
        if (repeat && *repeat != HUF_repeat_none) {
            size_t const oldSize = HUF_estimateCompressedSize(oldHufTable, table->count, maxSymbolVal);
            size_t const newSize = HUF_estimateCompressedSize(table->CTable, table->count, maxSymbolVal);
            if (oldSize <= hSize + newSize || hSize + 12 >= srcSize) {
                return HUF_compressCTable_internal(ostart, op, oend,
                                                   src, srcSize,
                                                   nbStreams, oldHufTable, bmi2);
        }   }

        /* Use the new huffman table */
        if (hSize + 12ul >= srcSize) { return 0; }
        op += hSize;
        if (repeat) { *repeat = HUF_repeat_none; }
        if (oldHufTable)
            memcpy(oldHufTable, table->CTable, sizeof(table->CTable));  /* Save new table */
    }
    return HUF_compressCTable_internal(ostart, op, oend,
                                       src, srcSize,
                                       nbStreams, table->CTable, bmi2);
}


size_t HUF_compress1X_wksp (uk dst, size_t dstSize,
                      ukk src, size_t srcSize,
                      u32 maxSymbolVal, u32 huffLog,
                      uk workSpace, size_t wkspSize)
{
    return HUF_compress_internal(dst, dstSize, src, srcSize,
                                 maxSymbolVal, huffLog, HUF_singleStream,
                                 workSpace, wkspSize,
                                 NULL, NULL, 0, 0 /*bmi2*/);
}

size_t HUF_compress1X_repeat (uk dst, size_t dstSize,
                      ukk src, size_t srcSize,
                      u32 maxSymbolVal, u32 huffLog,
                      uk workSpace, size_t wkspSize,
                      HUF_CElt* hufTable, HUF_repeat* repeat, i32 preferRepeat, i32 bmi2)
{
    return HUF_compress_internal(dst, dstSize, src, srcSize,
                                 maxSymbolVal, huffLog, HUF_singleStream,
                                 workSpace, wkspSize, hufTable,
                                 repeat, preferRepeat, bmi2);
}

size_t HUF_compress1X (uk dst, size_t dstSize,
                 ukk src, size_t srcSize,
                 u32 maxSymbolVal, u32 huffLog)
{
    u32 workSpace[HUF_WORKSPACE_SIZE_U32];
    return HUF_compress1X_wksp(dst, dstSize, src, srcSize, maxSymbolVal, huffLog, workSpace, sizeof(workSpace));
}

/* HUF_compress4X_repeat():
 * compress input using 4 streams.
 * provide workspace to generate compression tables */
size_t HUF_compress4X_wksp (uk dst, size_t dstSize,
                      ukk src, size_t srcSize,
                      u32 maxSymbolVal, u32 huffLog,
                      uk workSpace, size_t wkspSize)
{
    return HUF_compress_internal(dst, dstSize, src, srcSize,
                                 maxSymbolVal, huffLog, HUF_fourStreams,
                                 workSpace, wkspSize,
                                 NULL, NULL, 0, 0 /*bmi2*/);
}

/* HUF_compress4X_repeat():
 * compress input using 4 streams.
 * re-use an existing huffman compression table */
size_t HUF_compress4X_repeat (uk dst, size_t dstSize,
                      ukk src, size_t srcSize,
                      u32 maxSymbolVal, u32 huffLog,
                      uk workSpace, size_t wkspSize,
                      HUF_CElt* hufTable, HUF_repeat* repeat, i32 preferRepeat, i32 bmi2)
{
    return HUF_compress_internal(dst, dstSize, src, srcSize,
                                 maxSymbolVal, huffLog, HUF_fourStreams,
                                 workSpace, wkspSize,
                                 hufTable, repeat, preferRepeat, bmi2);
}

size_t HUF_compress2 (uk dst, size_t dstSize,
                ukk src, size_t srcSize,
                u32 maxSymbolVal, u32 huffLog)
{
    u32 workSpace[HUF_WORKSPACE_SIZE_U32];
    return HUF_compress4X_wksp(dst, dstSize, src, srcSize, maxSymbolVal, huffLog, workSpace, sizeof(workSpace));
}

size_t HUF_compress (uk dst, size_t maxDstSize, ukk src, size_t srcSize)
{
    return HUF_compress2(dst, maxDstSize, src, srcSize, 255, HUF_TABLELOG_DEFAULT);
}
