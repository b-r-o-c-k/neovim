// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* for debugging */
/* #define CHECK(c, s)	if (c) EMSG(s) */
#define CHECK(c, s)

/*
 * memline.c: Contains the functions for appending, deleting and changing the
 * text lines. The memfile functions are used to store the information in
 * blocks of memory, backed up by a file. The structure of the information is
 * a tree.  The root of the tree is a pointer block. The leaves of the tree
 * are data blocks. In between may be several layers of pointer blocks,
 * forming branches.
 *
 * Three types of blocks are used:
 * - Block nr 0 contains information for recovery
 * - Pointer blocks contain list of pointers to other blocks.
 * - Data blocks contain the actual text.
 *
 * Block nr 0 contains the block0 structure (see below).
 *
 * Block nr 1 is the first pointer block. It is the root of the tree.
 * Other pointer blocks are branches.
 *
 *  If a line is too big to fit in a single page, the block containing that
 *  line is made big enough to hold the line. It may span several pages.
 *  Otherwise all blocks are one page.
 *
 *  A data block that was filled when starting to edit a file and was not
 *  changed since then, can have a negative block number. This means that it
 *  has not yet been assigned a place in the file. When recovering, the lines
 *  in this data block can be read from the original file. When the block is
 *  changed (lines appended/deleted/changed) or when it is flushed it gets a
 *  positive number. Use mf_trans_del() to get the new number, before calling
 *  mf_get().
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#include "nvim/ascii.h"
#include "nvim/vim.h"
#include "nvim/memline.h"
#include "nvim/buffer.h"
#include "nvim/cursor.h"
#include "nvim/eval.h"
#include "nvim/fileio.h"
#include "nvim/func_attr.h"
#include "nvim/main.h"
#include "nvim/mark.h"
#include "nvim/mbyte.h"
#include "nvim/memfile.h"
#include "nvim/memory.h"
#include "nvim/message.h"
#include "nvim/misc1.h"
#include "nvim/option.h"
#include "nvim/os_unix.h"
#include "nvim/path.h"
#include "nvim/screen.h"
#include "nvim/sha256.h"
#include "nvim/spell.h"
#include "nvim/strings.h"
#include "nvim/ui.h"
#include "nvim/version.h"
#include "nvim/undo.h"
#include "nvim/window.h"
#include "nvim/os/os.h"
#include "nvim/os/input.h"

#ifdef INCLUDE_GENERATED_DECLARATIONS
# include "memline.c.generated.h"
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
int ml_open(buf_T *buf)
{
  bhdr_T      *hp = NULL;
  buf->b_p_swf = false;

  /*
   * When 'updatecount' is non-zero swap file may be opened later.
   */
  if (!buf->terminal && p_uc && buf->b_p_swf) {
    buf->b_may_swap = true;
  } else {
    buf->b_may_swap = false;
  }

  buf->b_ml.ml_mfp = 1;
  buf->b_ml.ml_flags = ML_EMPTY;
  buf->b_ml.ml_line_count = 1;
  curwin->w_nrwidth_line_count = 0;

  return OK;
}

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
void ml_setname(buf_T *buf)
{
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
void ml_open_files(void)
{
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
void ml_open_file(buf_T *buf)
{
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
void 
check_need_swap (
    int newfile                    /* reading file into new buffer */
)
{
  if (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
    ml_open_file(curbuf);
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
void ml_close(buf_T *buf, int del_file)
{
}

/*
 * Close all existing memlines and memfiles.
 * Only used when exiting.
 * When 'del_file' is TRUE, delete the memfiles.
 * But don't delete files that were ":preserve"d when we are POSIX compatible.
 */
void ml_close_all(int del_file)
{
}

/*
 * Close all memfiles for not modified buffers.
 * Only use just before exiting!
 */
void ml_close_notmod(void)
{
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
void ml_timestamp(buf_T *buf)
{
}

/*
 * Try to recover curbuf from the .swp file.
 */
void ml_recover(void)
{
}

/*
 * Find the names of swap files in current directory and the directory given
 * with the 'directory' option.
 *
 * Used to:
 * - list the swap files for "vim -r"
 * - count the number of swap files when recovering
 * - list the swap files when recovering
 * - find the name of the n'th swap file when recovering
 */
int 
recover_names (
    char_u *fname,             /* base for swap file name */
    int list,                       /* when TRUE, list the swap file names */
    int nr,                         /* when non-zero, return nr'th swap file name */
    char_u **fname_out        /* result when "nr" > 0 */
)
{
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
void ml_sync_all(int check_file, int check_char, bool do_fsync)
{
}

/*
 * sync one buffer, including negative blocks
 *
 * after this all the blocks are in the swap file
 *
 * Used for the :preserve command and when the original file has been
 * changed or deleted.
 *
 * when message is TRUE the success of preserving is reported
 */
void ml_preserve(buf_T *buf, int message, bool do_fsync)
{
}

/*
 * NOTE: The pointer returned by the ml_get_*() functions only remains valid
 * until the next call!
 *  line1 = ml_get(1);
 *  line2 = ml_get(2);	// line1 is now invalid!
 * Make a copy of the line if necessary.
 */
/*
 * Return a pointer to a (read-only copy of a) line.
 *
 * On failure an error message is given and IObuff is returned (to avoid
 * having to check for error everywhere).
 */
const char_u *ml_get(linenr_T lnum)
{
  return ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
const char_u *ml_get_pos(pos_T *pos)
{
  return ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col;
}

/*
 * Return a pointer to a line in a specific buffer
 *
 * "will_change": if TRUE mark the buffer dirty (chars in the line will be
 * changed)
 */
const char_u *
ml_get_buf (
    buf_T *buf,
    linenr_T lnum,
    int will_change                        /* line will be changed */
)
{
  // The line should always be immutable.
  assert(!will_change);
  static int recursive = 0;

  if (lnum > buf->b_ml.ml_line_count) { /* invalid line number */
    if (recursive == 0) {
      // Avoid giving this message for a recursive call, may happen when
      // the GUI redraws part of the text.
      recursive++;
      IEMSGN(_("E315: ml_get: invalid lnum: %" PRId64), lnum);
      recursive--;
    }
errorret:
    STRCPY(IObuff, "???");
    return IObuff;
  }

  if (lnum <= 0)                        /* pretend line 0 is line 1 */
    lnum = 1;

  void *vsnvim_data = buf->vsnvim_data;
  if (vsnvim_data == NULL) {
    return "";
  }
  return vsnvim_get_line(vsnvim_data, lnum);
}

/*
 * Append a line after lnum (may be 0 to insert a line in front of the file).
 * "line" does not need to be allocated, but can't be another line in a
 * buffer, unlocking may make it invalid.
 *
 *   newfile: TRUE when starting to edit a new file, meaning that pe_old_lnum
 *		will be set for recovery
 * Check: The caller of this function should probably also call
 * appended_lines().
 *
 * return FAIL for failure, OK otherwise
 */
int 
ml_append (
    linenr_T lnum,                  /* append after this line (can be 0) */
    char_u *line,              /* text of the new line */
    colnr_T len,                    /* length of new line, including NUL, or 0 */
    int newfile                    /* flag, see above */
)
{
  return ml_append_buf(curbuf, lnum, line, len, newfile);
}

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
int 
ml_append_buf (
    buf_T *buf,
    linenr_T lnum,                  /* append after this line (can be 0) */
    char_u *line,              /* text of the new line */
    colnr_T len,                    /* length of new line, including NUL, or 0 */
    int newfile                    /* flag, see above */
)
{
  void *vsnvim_data = buf->vsnvim_data;
  if (vsnvim_data == NULL) {
    return OK;
  }
  return vsnvim_append_line(vsnvim_data, lnum, line, len);
}

/*
 * Replace line lnum, with buffering, in current buffer.
 *
 * If "copy" is TRUE, make a copy of the line, otherwise the line has been
 * copied to allocated memory already.
 *
 * Check: The caller of this function should probably also call
 * changed_lines(), unless update_screen(NOT_VALID) is used.
 *
 * return FAIL for failure, OK otherwise
 */
int ml_replace(linenr_T lnum, char_u *line, bool copy)
{
  void *vsnvim_data = curbuf->vsnvim_data;
  if (vsnvim_data == NULL) {
    return OK;
  }
  return vsnvim_replace_line(vsnvim_data, lnum, line);
}

/// Delete line `lnum` in the current buffer.
///
/// @note The caller of this function should probably also call
/// deleted_lines() after this.
///
/// @param message  Show "--No lines in buffer--" message.
/// @return FAIL for failure, OK otherwise
int ml_delete(linenr_T lnum, int message)
{
  void *vsnvim_data = curbuf->vsnvim_data;
  if (vsnvim_data == NULL) {
    return OK;
  }
  return vsnvim_delete_line(vsnvim_data, lnum);
}

/*
 * set the B_MARKED flag for line 'lnum'
 */
void ml_setmarked(linenr_T lnum)
{
}

/*
 * find the first line with its B_MARKED flag set
 */
linenr_T ml_firstmarked(void)
{
  return 0;
}

/*
 * clear all DB_MARKED flags
 */
void ml_clearmarked(void)
{
}

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
char_u *makeswapname(char_u *fname, char_u *ffname, buf_T *buf, char_u *dir_name)
{
  return NULL;
}

/*
 * Get file name to use for swap file or backup file.
 * Use the name of the edited file "fname" and an entry in the 'dir' or 'bdir'
 * option "dname".
 * - If "dname" is ".", return "fname" (swap file in dir of file).
 * - If "dname" starts with "./", insert "dname" in "fname" (swap file
 *   relative to dir of file).
 * - Otherwise, prepend "dname" to the tail of "fname" (swap file in specific
 *   dir).
 *
 * The return value is an allocated string and can be NULL.
 */
char_u *
get_file_in_dir (
    char_u *fname,
    char_u *dname         /* don't use "dirname", it is a global for Alpha */
)
{
  return NULL;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
void ml_setflags(buf_T *buf)
{
}

/*
 * Find offset for line or line with offset.
 * Find line with offset if "lnum" is 0; return remaining offset in offp
 * Find offset of line if "lnum" > 0
 * return -1 if information is not available
 */
long ml_find_line_or_offset(buf_T *buf, linenr_T lnum, long *offp)
{
  return -1;
}

/// Goto byte in buffer with offset 'cnt'.
void goto_byte(long cnt)
{
  long boff = cnt;
  linenr_T lnum;

  setpcmark();
  if (boff) {
    boff--;
  }
  lnum = ml_find_line_or_offset(curbuf, (linenr_T)0, &boff);
  if (lnum < 1) {         // past the end
    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    curwin->w_curswant = MAXCOL;
    coladvance((colnr_T)MAXCOL);
  } else {
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = (colnr_T)boff;
    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = TRUE;
  }
  check_cursor();

  // Make sure the cursor is on the first byte of a multi-byte char.
  if (has_mbyte) {
    mb_adjust_cursor();
  }
}

/// Increment the line pointer "lp" crossing line boundaries as necessary.
/// Return 1 when going to the next line.
/// Return 2 when moving forward onto a NUL at the end of the line).
/// Return -1 when at the end of file.
/// Return 0 otherwise.
int inc(pos_T *lp)
{
  const char_u  *p = ml_get_pos(lp);

  if (*p != NUL) {      // still within line, move to next char (may be NUL)
    if (has_mbyte) {
      int l = (*mb_ptr2len)(p);

      lp->col += l;
      return (p[l] != NUL) ? 0 : 2;
    }
    lp->col++;
    lp->coladd = 0;
    return (p[1] != NUL) ? 0 : 2;
  }
  if (lp->lnum != curbuf->b_ml.ml_line_count) {     // there is a next line
    lp->col = 0;
    lp->lnum++;
    lp->coladd = 0;
    return 1;
  }
  return -1;
}

/// Same as inc(), but skip NUL at the end of non-empty lines.
int incl(pos_T *lp)
{
  int r;

  if ((r = inc(lp)) >= 1 && lp->col) {
    r = inc(lp);
  }
  return r;
}

int dec(pos_T *lp)
{
  const char_u      *p;

  lp->coladd = 0;
  if (lp->col > 0) {            // still within line
    lp->col--;
    if (has_mbyte) {
      p = ml_get(lp->lnum);
      lp->col -= (*mb_head_off)(p, p + lp->col);
    }
    return 0;
  }
  if (lp->lnum > 1) {           // there is a prior line
    lp->lnum--;
    p = ml_get(lp->lnum);
    lp->col = (colnr_T)STRLEN(p);
    if (has_mbyte) {
      lp->col -= (*mb_head_off)(p, p + lp->col);
    }
    return 1;
  }
  return -1;                    // at start of file
}

/// Same as dec(), but skip NUL at the end of non-empty lines.
int decl(pos_T *lp)
{
  int r;

  if ((r = dec(lp)) == 1 && lp->col) {
    r = dec(lp);
  }
  return r;
}
