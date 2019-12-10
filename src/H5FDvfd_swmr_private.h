/*
 * Copyright (c) 2019 The HDF Group.  All rights reserved.
 *
 * This file is part of HDF5.  The full HDF5 copyright notice, including
 * terms governing use, modification, and redistribution, is contained in
 * the COPYING file, which can be found at the root of the source code
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.
 * If you do not have access to either file, you may request a copy from
 * help@hdfgroup.org.
 */

#ifndef _H5FDvfd_swmr_private_H
#define _H5FDvfd_swmr_private_H

/* Forward declaration */
struct H5F_t;
struct H5F_shared_t;
struct H5FD_vfd_swmr_idx_entry_t;

/* 
 *  struct H5F_vfd_eot_queue_entry_t
 * 
 *  This is the structure for an entry on the end-of-tick queue (EOT queue) of files
 *  opened in either VFD SWMR write or VFD SWMR read mode.  This queue is maintained
 *  in increasing end of tick time order.
 *  The structure contains all information required to determine whether the end 
 *  of tick has arrived for the specified file, and to initiate end of tick processing
 *  if it has.
 *
 *  The fields of H5F_vfd_eot_queue_entry_t are discussed below:
 *
 *  vfd_swmr_file: Pointer to the instance of H5F_file_t containing the shared
 *      fields of the associated file that has been opened in VFD SWMR mode
 *  NOTE: for the time being use H5F_t instead of H5F_file_t 
 *
 *  vfd_swmr_writer:  Boolean flag that is set to TRUE if the associated file 
 *      has been opened in VFD SWMR writer mode, and FALSE if it has been 
 *      opened in VFD SWMR reader mode.
 *
 *  tick_num: Number of the current tick of the target file.
 *
 *  end_of_tick: Expiration time of the current tick of the target file.
 *
 *  next: Pointer to the next element in the end of tick queue, or NULL if there
 *      is no next entry.  Note that if next is not NULL, next->end_of_tick 
 *      must be greater than or equal to end_of_tick.
 *
 *  prev: Pointer to the previous element in the end of tick queue, or NULL if 
 *      there is no previous entry.  Note that if prev is not NULL, 
 *      prev->end_of_tick must be less than or equal to end_of_tick.
 *
 */
typedef struct H5F_vfd_swmr_eot_queue_entry_t {
    hbool_t     vfd_swmr_writer;
    uint64_t    tick_num;
    struct timespec end_of_tick;
    struct H5F_t  *vfd_swmr_file; /* NOTE: for the time being use H5F_t instead H5F_file_t */
    struct H5F_vfd_swmr_eot_queue_entry_t *next;
    struct H5F_vfd_swmr_eot_queue_entry_t *prev;
} H5F_vfd_swmr_eot_queue_entry_t;

extern unsigned int vfd_swmr_api_entries_g;
extern hbool_t vfd_swmr_writer_g;
extern struct timespec end_of_tick_g;

/* The head and tail pointers of the EOT queue */
extern H5F_vfd_swmr_eot_queue_entry_t *vfd_swmr_eot_queue_head_g;
extern H5F_vfd_swmr_eot_queue_entry_t *vfd_swmr_eot_queue_tail_g;

/***************************************/
/* Library-private Function Prototypes */
/***************************************/

H5_DLL herr_t H5F_vfd_swmr_init(struct H5F_t *f, hbool_t file_create);
H5_DLL herr_t H5F_vfd_swmr_close_or_flush(struct H5F_t *f, hbool_t closing);
H5_DLL herr_t H5F_update_vfd_swmr_metadata_file(struct H5F_t *f,
    uint32_t index_len, struct H5FD_vfd_swmr_idx_entry_t *index);
H5_DLL herr_t H5F_vfd_swmr_writer__delay_write(struct H5F_shared_t *, uint64_t,
    uint64_t *);
H5_DLL herr_t H5F_vfd_swmr_writer__prep_for_flush_or_close(struct H5F_t *f);
H5_DLL herr_t H5F_vfd_swmr_writer_end_of_tick(struct H5F_t *f);
H5_DLL herr_t H5F_vfd_swmr_writer__dump_index(struct H5F_t * f);
H5_DLL herr_t H5F_vfd_swmr_reader_end_of_tick(struct H5F_t *f);

H5_DLL herr_t H5F_vfd_swmr_remove_entry_eot(struct H5F_t *f);
H5_DLL herr_t H5F_vfd_swmr_insert_entry_eot(struct H5F_t *f);
H5_DLL herr_t H5F_dump_eot_queue(void);

#endif /* _H5FDvfd_swmr_private_H */