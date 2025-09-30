namespace drx
{


#define LDUMP(x)  // DDUMP(x)
#define LLOG(x)   // DLOG(x)

#ifndef DEF_MEM_LEVEL
#define DEF_MEM_LEVEL 8
#endif

#ifndef OS_CODE
#define OS_CODE  0x03
#endif

//////////////////////////////////////////////////////////////////////

	static voidpf zalloc_new ( voidpf opaque, uInt items, uInt size )
	{
		return MemoryAlloc ( items * size );
	}

	static void zfree_new ( voidpf opaque, voidpf address )
	{
		MemoryFree ( address );
	}

	enum
	{
		GZ_MAGIC1    = 0x1f,
		GZ_MAGIC2    = 0x8b,

		ASCII_FLAG   = 0x01, /* bit 0 set: file probably ascii text */
		HEAD_CRC     = 0x02, /* bit 1 set: header CRC present */
		EXTRA_FIELD  = 0x04, /* bit 2 set: extra field present */
		ORIG_NAME    = 0x08, /* bit 3 set: original file name present */
		COMMENT      = 0x10, /* bit 4 set: file comment present */
		RESERVED     = 0xE0, /* bits 5..7: reserved */
	};

	static u8 sGZip_header[10] = { GZ_MAGIC1, GZ_MAGIC2, Z_DEFLATED, 0, 0, 0, 0, 0, 0, OS_CODE };

	void Crc32Stream::Out ( ukk ptr, u32 count )
	{
		crc = crc32 ( crc, ( u8* ) ptr, count );
	}

	void Crc32Stream::Clear()
	{
		crc = crc32 ( 0, NULL, 0 );
	}

	u32 CRC32 ( ukk ptr, u32 count )
	{
		Crc32Stream c;
		c.Put ( ptr, count );
		return c;
	}

	u32 CRC32 ( const Txt& s )
	{
		return CRC32 ( ~s, s.GetLength() );
	}

	void Zlib::Begin()
	{
		Free();
		error = false;

		if ( docrc || gzip )
			crc = crc32 ( 0, NULL, 0 );

		out.Clear();

		gzip_name.Clear();

		gzip_comment.Clear();

		footer.Clear();

		gzip_footer = false;

		gzip_header_done = false;
	}

	void Zlib::Compress()
	{
		Begin();

		if ( deflateInit2 ( &z, compression_level, Z_DEFLATED,
							hdr && !gzip ? MAX_WBITS : -MAX_WBITS, DEF_MEM_LEVEL,
							Z_DEFAULT_STRATEGY ) != Z_OK )
			Panic ( "deflateInit2 failed" );

		mode = DEFLATE;

		if ( gzip )
			WhenOut ( sGZip_header, 10 );
	}

	void Zlib::Decompress()
	{
		Begin();

		if ( inflateInit2 ( &z, hdr && !gzip ? + MAX_WBITS : -MAX_WBITS ) != Z_OK )
			Panic ( "inflateInit2 failed" );

		mode = INFLATE;
	}

	void Zlib::Pump ( bool finish )
	{
		if ( error )
			return;

		ASSERT ( mode );

		if ( !output )
			output.Alloc ( chunk );

		for ( ;; )
		{
			if ( gzip_footer )
			{
				footer.Cat ( z.next_in, z.avail_in );
				z.avail_in = 0;
				return;
			}

			i32 code;

			z.avail_out = chunk;
			z.next_out = output;
			code = ( mode == DEFLATE ? deflate : inflate ) ( &z, finish ? Z_FINISH : Z_NO_FLUSH );
			i32 count = chunk - z.avail_out;

			if ( count )
			{
				if ( ( docrc || gzip ) && mode == INFLATE )
					crc = crc32 ( crc, output, count );

				WhenOut ( ( tukk ) ~output, count );

				if ( mode == INFLATE )
					total += count;
			}

			if ( mode == INFLATE && code == Z_STREAM_END )
				gzip_footer = true;
			else
			{
				if ( mode == INFLATE ? code == Z_BUF_ERROR : count == 0 )
					break;

				if ( code != Z_OK && code != Z_STREAM_END )
				{
					LLOG ( "ZLIB ERROR " << code );
					Free();
					error = true;
					break;
				}
			}
		}
	}

	i32 Zlib::GzipHeader ( tukk ptr, i32 size )
	{
		i32 pos = 10;

		if ( pos > size )
			return 0;

		i32 flags = ptr[3];

		if ( ptr[2] != Z_DEFLATED || ( flags & RESERVED ) != 0 )
		{
			error = true;
			return 0;
		}

		if ( flags & EXTRA_FIELD )
		{
			if ( pos + 2 > size )
				return 0;

			i32 len = MAKEWORD ( ptr[pos], ptr[pos + 1] );

			if ( len < 0 )
			{
				error = true;
				return 0;
			}

			if ( ( pos += len + 2 ) > size )
				return 0;
		}

		gzip_name.Clear();

		gzip_comment.Clear();

		for ( i32 i = !! ( flags & ORIG_NAME ) + !! ( flags & COMMENT ); i > 0; i-- )
		{
			while ( ptr[pos] )
			{
				( i ? gzip_name : gzip_comment ).Cat ( ptr[pos] );

				if ( ++pos > size )
					return 0;
			}

			if ( ++pos > size )
				return 0;
		}

		if ( flags & HEAD_CRC )
			if ( ( pos += 2 ) > size )
				return 0;

		return pos;
	}

	void Zlib::Put0 ( tukk ptr, i32 size )
	{
		if ( error )
			return;

		ASSERT ( mode );

		if ( size <= 0 )
			return;

		if ( gzip && !gzip_header_done && mode == INFLATE )
		{
			if ( gzip_hs.GetCount() )
			{
				gzip_hs.Cat ( ptr, size );
				ptr = ~gzip_hs;
				size = gzip_hs.GetCount();
			}

			i32 pos = GzipHeader ( ptr, size );

			if ( !pos )
			{
				if ( gzip_hs.GetCount() == 0 )
					gzip_hs.Cat ( ptr, size );

				return;
			}

			gzip_header_done = true;

			size -= pos;
			ptr += pos;
		}

		if ( size <= 0 )
			return;

		if ( mode == DEFLATE )
		{
			total += size;

			if ( docrc || gzip )
				crc = crc32 ( crc, ( const Bytef* ) ptr, size );
		}

		z.next_in = ( Bytef* ) ptr;

		z.avail_in = size;
		Pump ( false );
	}

	void Zlib::Put ( ukk ptr, i32 size )
	{
		if ( error )
			return;

		LLOG ( "ZLIB Put " << size );

		tukk p = reinterpret_cast<tukk> ( ptr );

		while ( size )
		{
			i32 psz = ( i32 ) min ( size, INT_MAX / 4 );
			Put0 ( p, size );
			size -= psz;
			p += psz;
		}
	}

	void Zlib::PutOut ( ukk ptr, i32 size )
	{
		LLOG ( "ZLIB PutOut " << out.GetCount() );
		out.Cat ( ( tukk ) ptr, ( i32 ) size );
	}

	void Zlib::End()
	{
		LLOG ( "ZLIB End" );

		if ( mode != INFLATE || !gzip || gzip_header_done )
			Pump ( true );

		if ( gzip && mode == DEFLATE )
		{
			char h[8];
			Poke32le ( h, crc );
			Poke32le ( h + 4, total );
			WhenOut ( h, 8 );
		}

		if ( gzip && mode == INFLATE &&
			 ( footer.GetCount() != 8 || Peek32le ( ~footer ) != ( i32 ) crc || Peek32le ( ~footer + 4 ) != total ) )
		{
			LLOG ( "ZLIB GZIP FOOTER ERROR" );
			error = true;
		}

		Free();
	}

	void Zlib::Free()
	{
		if ( mode == INFLATE )
			inflateEnd ( &z );

		if ( mode == DEFLATE )
			deflateEnd ( &z );

		mode = NONE;

		gzip_hs.Clear();

		gzip_header_done = false;

		gzip_footer = false;

		total = 0;
	}

	void Zlib::Init()
	{
		mode = NONE;
		gzip = false;
		error = false;
		z.next_in = ( u8* ) "";
		z.avail_in = 0;
	}

	void Zlib::Clear()
	{
		Free();
		Init();
	}

	Zlib& Zlib::ChunkSize ( i32 n )
	{
		ASSERT ( n < INT_MAX / 4 );
		output.Clear();
		chunk = n;
		return *this;
	}

	Zlib::Zlib()
	{
		Init();
		z.zalloc = zalloc_new;
		z.zfree = zfree_new;
		z.opaque = 0;
		docrc = false;
		crc = 0;
		hdr = true;
		chunk = 4096;
		WhenOut = callback ( this, &Zlib::PutOut );
		compression_level = Z_DEFAULT_COMPRESSION;
	}

	Zlib::~Zlib()
	{
		Free();
	}

	i64 zPress ( Stream& out, Stream& in, i64 size, Gate<i64, i64> progress, bool gzip, bool compress,
				 u32 *crc = NULL, bool hdr = true )
	{
		Zlib zlib;
		zlib.GZip ( gzip ).CRC ( crc ).Header ( hdr );

		i64 r = -1;
		{
			OutFilterStream outs ( out, zlib );

			if ( compress )
				zlib.Compress();
			else
				zlib.Decompress();

			if ( CopyStream ( outs, in, size, progress ) >= 0 )
			{
				outs.Close();

				if ( !out.IsError() && !outs.IsError() )
					r = outs.GetCount();
			}
		}

		if ( r >= 0 && crc )
			*crc = zlib.GetCRC();

		return r;
	}

	i64 ZCompress ( Stream& out, Stream& in, i64 size, Gate<i64, i64>progress, bool hdr )
	{
		return zPress ( out, in, size, progress, false, true, NULL, hdr );
	}

	i64 ZDecompress ( Stream& out, Stream& in, i64 size, Gate<i64, i64>progress, bool hdr )
	{
		return zPress ( out, in, size, progress, false, false, NULL, hdr );
	}

	i64 ZCompress ( Stream& out, Stream& in, Gate<i64, i64>progress )
	{
		return ZCompress ( out, in, in.GetLeft(), progress );
	}

	i64 ZDecompress ( Stream& out, Stream& in, Gate<i64, i64>progress )
	{
		return zPress ( out, in, in.GetLeft(), progress, false, false );
	}

	Txt ZCompress ( ukk data, i64 len, Gate<i64, i64>progress )
	{
		TxtStream out;
		MemReadStream in ( data, len );
		return ZCompress ( out, in, progress ) < 0 ? Txt::GetVoid() : out.GetResult();
	}

	Txt ZCompress ( const Txt& s, Gate<i64, i64>progress )
	{
		return ZCompress ( ~s, s.GetLength(), progress );
	}

	Txt ZDecompress ( ukk data, i64 len, Gate<i64, i64>progress )
	{
		TxtStream out;
		MemReadStream in ( data, len );
		return ZDecompress ( out, in, progress ) < 0 ? Txt::GetVoid() : out.GetResult();
	}

	Txt ZDecompress ( const Txt& s, Gate<i64, i64>progress )
	{
		return ZDecompress ( ~s, s.GetLength(), progress );
	}

	i64  GZCompress ( Stream& out, Stream& in, i64 size, Gate<i64, i64>progress )
	{
		return zPress ( out, in, size, progress, true, true );
	}

	i64  GZDecompress ( Stream& out, Stream& in, i64 size, Gate<i64, i64>progress )
	{
		return zPress ( out, in, size, progress, true, false );
	}

	i64  GZCompress ( Stream& out, Stream& in, Gate<i64, i64>progress )
	{
		return GZCompress ( out, in, in.GetLeft(), progress );
	}

	Txt GZCompress ( ukk data, i32 len, Gate<i64, i64>progress )
	{
		TxtStream out;
		MemReadStream in ( data, len );
		return GZCompress ( out, in, progress ) < 0 ? Txt::GetVoid() : out.GetResult();
	}

	Txt GZCompress ( const Txt& s, Gate<i64, i64>progress )
	{
		return GZCompress ( ~s, s.GetCount(), progress );
	}

	i64  GZDecompress ( Stream& out, Stream& in, Gate<i64, i64>progress )
	{
		return GZDecompress ( out, in, in.GetLeft(), progress );
	}

	Txt GZDecompress ( ukk data, i32 len, Gate<i64, i64>progress )
	{
		TxtStream out;
		MemReadStream in ( data, len );
		return GZDecompress ( out, in, progress ) < 0 ? Txt::GetVoid() : out.GetResult();
	}

	Txt GZDecompress ( const Txt& s, Gate<i64, i64>progress )
	{
		return GZDecompress ( ~s, s.GetCount(), progress );
	}

	bool GZCompressFile ( tukk dstfile, tukk srcfile, Gate<i64, i64>progress )
	{
		FileIn in ( srcfile );

		if ( !in )
			return false;

		FileOut out ( dstfile );

		if ( !out )
			return false;

		if ( GZCompress ( out, in, in.GetLeft(), progress ) < 0 )
			return false;

		out.Close();

		return !out.IsError();
	}

	bool GZCompressFile ( tukk srcfile, Gate<i64, i64>progress )
	{
		return GZCompressFile ( ~ ( Txt ( srcfile ) + ".gz" ), srcfile, progress );
	}

	bool GZDecompressFile ( tukk dstfile, tukk srcfile, Gate<i64, i64>progress )
	{
		FileIn in ( srcfile );

		if ( !in )
			return false;

		FileOut out ( dstfile );

		if ( !out )
			return false;

		if ( GZDecompress ( out, in, in.GetLeft(), progress ) < 0 )
			return false;

		out.Close();

		return !out.IsError();
	}

	bool GZDecompressFile ( tukk srcfile, Gate<i64, i64>progress )
	{
		Txt dstfile = srcfile;

		if ( dstfile.EndsWith ( ".gz" ) )
			dstfile.Trim ( dstfile.GetLength() - 3 );
		else
			if ( dstfile.EndsWith ( ".gzip" ) )
				dstfile.Trim ( dstfile.GetLength() - 5 );
			else
				return false;

		return GZDecompressFile ( ~dstfile, srcfile, progress );
	}

	Gate<i64, i64> AsGate64 ( Gate<i32, i32> gate )
	{
		Gate<i64, i64> h;
		h << [=] ( i64 a, i64 b )
		{
			if ( b > ( ( i64 ) INT_MAX << 10 ) )
				return gate ( ( i32 ) ( a >> 32 ), ( i32 ) ( b >> 32 ) );

			if ( b > INT_MAX )
				return gate ( ( i32 ) ( a >> 22 ), ( i32 ) ( b >> 22 ) );

			return gate ( ( i32 ) a, ( i32 ) b );
		};

		return h;
	}

	Txt FastCompress ( ukk s, i32 sz )
	{
		size_t maxsize = LZ4_compressBound ( sz );

		if ( maxsize + sizeof ( i32 ) >= INT_MAX )
			Panic ( "Слишком огромный результат сжатия!" );

		TxtBuf b ( ( i32 ) maxsize + sizeof ( i32 ) );

		* ( i32* ) ~b = sz;

		i32 clen = LZ4_compress_default ( ( tukk ) s, ~b + sizeof ( i32 ), sz, ( i32 ) maxsize );

		b.SetCount ( clen + sizeof ( i32 ) );

		b.Shrink();

		return Txt ( b );
	}

	Txt FastCompress ( const Txt& s )
	{
		return FastCompress ( ~s, s.GetCount() );
	}

	Txt FastDecompress ( const Txt& data )
	{
		i32 sz = * ( i32* ) ~data;
		TxtBuf b ( sz );
		LZ4_decompress_safe ( ~data + sizeof ( i32 ), b, data.GetCount() - sizeof ( i32 ), sz );
		return Txt ( b );
	}

// following function is used in both plugin/lz4 and plugin/zstd
	void sCompressStreamCopy_ ( Stream& out, Stream& in, Gate<i64, i64> progress, Stream& orig_in, i64 insz )
	{
		i32k CHUNK = 32678;
		Buffer<u8> b ( CHUNK );

		while ( !in.IsEof() )
		{
			if ( progress ( orig_in.GetPos(), insz ) )
				break;

			i32 n = in.Get ( b, CHUNK );

			out.Put ( b, n );
		}

		progress ( orig_in.GetPos(), insz );
	}



};//endns
