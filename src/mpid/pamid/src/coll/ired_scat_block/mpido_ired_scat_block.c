/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/*  (C)Copyright IBM Corp.  2007, 2011  */
/**
 * \file src/coll/ired_scat_block/mpido_ired_scat_block.c
 * \brief ???
 */

/*#define TRACE_ON*/
#include <mpidimpl.h>

int MPIDO_Ireduce_scatter_block(const void *sendbuf,
                                void *recvbuf,
                                int recvcount,
                                MPI_Datatype datatype,
                                MPI_Op op,
                                MPIR_Comm *comm_ptr,
                                MPIR_Request **request)
{
   TRACE_ERR("Entering MPIDO_Ireduce_scatter_block\n");

   /*if (unlikely((data_size == 0) || (user_selected_type == MPID_COLL_USE_MPICH)))*/
   {
      /*
       * If the mpich mpir non-blocking collectives are enabled, return without
       * first constructing the MPIR_Request. This signals to the
       * MPIR_Ireduce_scatter_block_impl() function to invoke the mpich nbc
       * implementation of MPI_Ireduce_scatter_block().
       */
      if (MPIDI_Process.mpir_nbc != 0)
       return 0;

      /*
       * MPIR_* nbc implementation is not enabled. Fake a non-blocking
       * MPIR_Ireduce_scatter_block() with a blocking MPIR_Reduce_scatter_block().
       */
      if(unlikely(MPIDI_Process.verbose >= MPIDI_VERBOSE_DETAILS_ALL && comm_ptr->rank == 0))
         fprintf(stderr,"Using MPICH blocking reduce_scatter_block algorithm\n");

      int mpierrno = 0;
      int rc = MPIR_Reduce_scatter_block_impl(sendbuf, recvbuf, recvcount, datatype,
                                              op, comm_ptr, &mpierrno);

      /*
       * The blocking gather has completed - create and complete a
       * MPIR_Request object so the MPIR_Ireduce_scatter_block_impl() function
       * does not perform an additional ireduce_scatter_block.
       */
      MPIR_Request * mpid_request = MPID_Request_create_inline();
      mpid_request->kind = MPIR_REQUEST_KIND__COLL;
      *request = mpid_request;
      MPIDI_Request_complete_norelease_inline(mpid_request);

      return rc;
   }

   return 0;
}
