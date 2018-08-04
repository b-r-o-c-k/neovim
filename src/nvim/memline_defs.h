#ifndef NVIM_MEMLINE_DEFS_H
#define NVIM_MEMLINE_DEFS_H

#include "nvim/memfile_defs.h"

/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
typedef struct info_pointer {
  blocknr_T ip_bnum;            /* block number */
  linenr_T ip_low;              /* lowest lnum in this block */
  linenr_T ip_high;             /* highest lnum in this block */
  int ip_index;                 /* index for block with current lnum */
} infoptr_T;    /* block/index pair */

typedef struct ml_chunksize {
  int mlcs_numlines;
  long mlcs_totalsize;
} chunksize_T;

/* Flags when calling ml_updatechunk() */

#define ML_CHNK_ADDLINE 1
#define ML_CHNK_DELLINE 2
#define ML_CHNK_UPDLINE 3

/*
 * the memline structure holds all the information about a memline
 */
typedef struct memline {
  linenr_T ml_line_count;       /* number of lines in the buffer */

  memfile_T   *ml_mfp;          /* pointer to associated memfile */

#define ML_EMPTY        1       /* empty buffer */
#define ML_LOCKED_DIRTY 4       /* ml_locked was changed */
#define ML_LOCKED_POS   8       /* ml_locked needs positive block number */
  int ml_flags;
} memline_T;

#endif // NVIM_MEMLINE_DEFS_H
