/*
 * Copyright © 2018  Ebrahim Byagowi
 * Copyright © 2018  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#ifndef HB_AAT_LAYOUT_KERX_TABLE_HH
#define HB_AAT_LAYOUT_KERX_TABLE_HH

#include "hb-open-type.hh"
#include "hb-aat-layout-common.hh"

/*
 * kerx -- Extended Kerning
 * https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6kerx.html
 */
#define HB_AAT_TAG_kerx HB_TAG('k','e','r','x')


namespace AAT {

using namespace OT;


struct KerxFormat0Records
{
  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this)));
  }

  protected:
  GlyphID	left;
  GlyphID	right;
  FWORD		value;
  public:
  DEFINE_SIZE_STATIC (6);
};

struct KerxSubTableFormat0
{
  // TODO(ebraminio) Enable when we got suitable BinSearchArrayOf
  // inline int get_kerning (hb_codepoint_t left, hb_codepoint_t right) const
  // {
  //   hb_glyph_pair_t pair = {left, right};
  //   int i = pairs.bsearch (pair);
  //   if (i == -1)
  //     return 0;
  //   return pairs[i].get_kerning ();
  // }
  inline bool apply (hb_aat_apply_context_t *c) const
  {
    TRACE_APPLY (this);

    /* TODO */

    return_trace (true);
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this) &&
			  recordsZ.sanitize (c, nPairs)));
  }

  protected:
  // TODO(ebraminio): A custom version of "BinSearchArrayOf<KerxPair> pairs;" is
  // needed here to use HBUINT32 instead
  HBUINT32	nPairs;		/* The number of kerning pairs in this subtable */
  HBUINT32	searchRange;	/* The largest power of two less than or equal to the value of nPairs,
				 * multiplied by the size in bytes of an entry in the subtable. */
  HBUINT32	entrySelector;	/* This is calculated as log2 of the largest power of two less
				 * than or equal to the value of nPairs. */
  HBUINT32	rangeShift;	/* The value of nPairs minus the largest power of two less than or equal to nPairs. */
  UnsizedArrayOf<KerxFormat0Records>
		recordsZ;	/* VAR=nPairs */
  public:
  DEFINE_SIZE_ARRAY (16, recordsZ);
};

struct KerxSubTableFormat1
{
  inline bool apply (hb_aat_apply_context_t *c) const
  {
    TRACE_APPLY (this);

    /* TODO */

    return_trace (true);
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this) &&
			  stateHeader.sanitize (c)));
  }

  protected:
  StateTable<HBUINT16>		stateHeader;
  LOffsetTo<ArrayOf<HBUINT16> >	valueTable;
  public:
  DEFINE_SIZE_STATIC (20);
};

// TODO(ebraminio): Maybe this can be replaced with Lookup<HBUINT16>?
struct KerxClassTable
{
  inline unsigned int get_class (hb_codepoint_t g) const { return classes[g - firstGlyph]; }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (firstGlyph.sanitize (c) &&
			  classes.sanitize (c)));
  }

  protected:
  HBUINT16		firstGlyph;	/* First glyph in class range. */
  ArrayOf<HBUINT16>	classes;	/* Glyph classes. */
  public:
  DEFINE_SIZE_ARRAY (4, classes);
};

struct KerxSubTableFormat2
{
  inline int get_kerning (hb_codepoint_t left, hb_codepoint_t right, const char *end) const
  {
    unsigned int l = (this+leftClassTable).get_class (left);
    unsigned int r = (this+leftClassTable).get_class (left);
    unsigned int offset = l * rowWidth + r * sizeof (FWORD);
    const FWORD *arr = &(this+array);
    if (unlikely ((const void *) arr < (const void *) this || (const void *) arr >= (const void *) end))
      return 0;
    const FWORD *v = &StructAtOffset<FWORD> (arr, offset);
    if (unlikely ((const void *) v < (const void *) arr || (const void *) (v + 1) > (const void *) end))
      return 0;
    return *v;
  }

  inline bool apply (hb_aat_apply_context_t *c) const
  {
    TRACE_APPLY (this);

    /* TODO */

    return_trace (true);
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this) &&
			  rowWidth.sanitize (c) &&
			  leftClassTable.sanitize (c, this) &&
			  rightClassTable.sanitize (c, this) &&
			  array.sanitize (c, this)));
  }

  protected:
  HBUINT32	rowWidth;	/* The width, in bytes, of a row in the table. */
  LOffsetTo<KerxClassTable>
		leftClassTable;	/* Offset from beginning of this subtable to
				 * left-hand class table. */
  LOffsetTo<KerxClassTable>
		rightClassTable;/* Offset from beginning of this subtable to
				 * right-hand class table. */
  LOffsetTo<FWORD>
		array;		/* Offset from beginning of this subtable to
				 * the start of the kerning array. */
  public:
  DEFINE_SIZE_STATIC (16);
};

struct KerxSubTableFormat4
{
  inline bool apply (hb_aat_apply_context_t *c) const
  {
    TRACE_APPLY (this);

    /* TODO */

    return_trace (true);
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this) &&
			  rowWidth.sanitize (c) &&
			  leftClassTable.sanitize (c, this) &&
			  rightClassTable.sanitize (c, this) &&
			  array.sanitize (c, this)));
  }

  protected:
  HBUINT32	rowWidth;	/* The width, in bytes, of a row in the table. */
  LOffsetTo<KerxClassTable>
		leftClassTable;	/* Offset from beginning of this subtable to
				 * left-hand class table. */
  LOffsetTo<KerxClassTable>
		rightClassTable;/* Offset from beginning of this subtable to
				 * right-hand class table. */
  LOffsetTo<FWORD>
		array;		/* Offset from beginning of this subtable to
				 * the start of the kerning array. */
  public:
  DEFINE_SIZE_STATIC (16);
};

struct KerxSubTableFormat6
{
  inline bool apply (hb_aat_apply_context_t *c) const
  {
    TRACE_APPLY (this);

    /* TODO */

    return_trace (true);
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this) &&
			  rowIndexTable.sanitize (c, this) &&
			  columnIndexTable.sanitize (c, this) &&
			  kerningArray.sanitize (c, this) &&
			  kerningVector.sanitize (c, this)));
  }

  protected:
  HBUINT32	flags;
  HBUINT16	rowCount;
  HBUINT16	columnCount;
  LOffsetTo<Lookup<HBUINT16> >	rowIndexTable;
  LOffsetTo<Lookup<HBUINT16> >	columnIndexTable;
  LOffsetTo<Lookup<HBUINT16> >	kerningArray;
  LOffsetTo<Lookup<HBUINT16> >	kerningVector;
  public:
  DEFINE_SIZE_STATIC (24);
};

struct KerxTable
{
  inline unsigned int get_size (void) const { return length; }
  inline unsigned int get_type (void) const { return coverage & SubtableType; }

  enum Coverage
  {
    Vertical		= 0x80000000,	/* Set if table has vertical kerning values. */
    CrossStream		= 0x40000000,	/* Set if table has cross-stream kerning values. */
    Variation		= 0x20000000,	/* Set if table has variation kerning values. */
    ProcessDirection	= 0x10000000,	/* If clear, process the glyphs forwards, that
					 * is, from first to last in the glyph stream.
					 * If we, process them from last to first.
					 * This flag only applies to state-table based
					 * 'kerx' subtables (types 1 and 4). */
    Reserved		= 0x0FFFFF00,	/* Reserved, set to zero. */
    SubtableType	= 0x000000FF,	/* Subtable type. */
  };

  template <typename context_t>
  inline typename context_t::return_t dispatch (context_t *c) const
  {
    unsigned int subtable_type = get_type ();
    TRACE_DISPATCH (this, subtable_type);
    switch (subtable_type) {
    case 0	:		return_trace (c->dispatch (u.format0));
    case 1	:		return_trace (c->dispatch (u.format1));
    case 2	:		return_trace (c->dispatch (u.format2));
    case 4	:		return_trace (c->dispatch (u.format4));
    case 6	:		return_trace (c->dispatch (u.format6));
    default:			return_trace (c->default_return_value ());
    }
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    if (!length.sanitize (c) ||
	length < min_size ||
	!c->check_range (this, length))
      return_trace (false);

    return_trace (dispatch (c));
  }

protected:
  HBUINT32	length;
  HBUINT32	coverage;
  HBUINT32	tupleCount;
  union {
  KerxSubTableFormat0	format0;
  KerxSubTableFormat1	format1;
  KerxSubTableFormat2	format2;
  KerxSubTableFormat4	format4;
  KerxSubTableFormat6	format6;
  } u;
public:
  DEFINE_SIZE_MIN (12);
};

struct SubtableXXX
{
  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    return_trace (likely (c->check_struct (this)));
  }

  protected:
  HBUINT32	length;
  HBUINT32	coverage;
  HBUINT32	tupleCount;
  public:
  DEFINE_SIZE_STATIC (12);
};


/*
 * The 'kerx' Table
 */

struct kerx
{
  static const hb_tag_t tableTag = HB_AAT_TAG_kerx;

  inline bool has_data (void) const { return version != 0; }

  inline void apply (hb_aat_apply_context_t *c) const
  {
    c->set_lookup_index (0);
    const KerxTable *table = &firstTable;
    unsigned int count = tableCount;
    for (unsigned int i = 0; i < count; i++)
    {
      table->dispatch (c);
      table = &StructAfter<KerxTable> (*table);
    }
  }

  inline bool sanitize (hb_sanitize_context_t *c) const
  {
    TRACE_SANITIZE (this);
    if (!version.sanitize (c) || version < 2 ||
	!tableCount.sanitize (c))
      return_trace (false);

    const KerxTable *table = &firstTable;
    unsigned int count = tableCount;
    for (unsigned int i = 0; i < count; i++)
    {
      if (!table->sanitize (c))
	return_trace (false);
      table = &StructAfter<KerxTable> (*table);
    }

    return_trace (true);
  }

  protected:
  HBUINT16	version;	/* The version number of the extended kerning table
				 * (currently 2, 3, or 4). */
  HBUINT16	unused;		/* Set to 0. */
  HBUINT32	tableCount;	/* The number of subtables included in the extended kerning
				 * table. */
  KerxTable	firstTable;	/* Subtables. */
/*subtableGlyphCoverageArray*/	/* Only if version >= 3. We don't use. */

  public:
  DEFINE_SIZE_MIN (8);
};

} /* namespace AAT */


#endif /* HB_AAT_LAYOUT_KERX_TABLE_HH */
