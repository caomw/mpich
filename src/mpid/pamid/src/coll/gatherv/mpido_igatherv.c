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
 * \file src/coll/gatherv/mpido_igatherv.c
 * \brief ???
 */

/*#define TRACE_ON*/
#include <mpidimpl.h>

int MPIDO_Igatherv(const void *sendbuf,
                   int sendcount,
                   MPI_Datatype sendtype,
                   void *recvbuf,
                   const int *recvcounts,
                   const int *displs,
                   MPI_Datatype recvtype,
                   int root,
                   MPIR_Comm * comm_ptr,
                   MPIR_Request **request)

{
   TRACE_ERR("Entering MPIDO_Igatherv\n");

   /*if (unlikely((data_size == 0) || (user_selected_type == MPID_COLL_USE_MPICH)))*/
   {
      /*
       * If the mpich mpir non-blocking collectives are enabled, return without
       * first constructing the MPIR_Request. This signals to the
       * MPIR_Igatherv_impl() function to invoke the mpich nbc implementation
       * of MPI_Igatherv().
       */
      if (MPIDI_Process.mpir_nbc != 0)
       return 0;

      /*
       * MPIR_* nbc implementation is not enabled. Fake a non-blocking
       * MPIR_Igatherv() with a blocking MPIR_Gatherv().
       */
      if(unlikely(MPIDI_Process.verbose >= MPIDI_VERBOSE_DETAILS_ALL && comm_ptr->rank == 0))
         fprintf(stderr,"Using MPICH blocking gatherv algorithm\n");

      int mpierrno = 0;
      int rc = MPIR_Gatherv_impl(sendbuf, sendcount, sendtype,
                                 recvbuf, recvcounts, displs, recvtype,
                                 root, comm_ptr, &mpierrno);

      /*
       * The blocking gather has completed - create and complete a
       * MPIR_Request object so the MPIR_Igatherv_impl() function does not
       * perform an additional igatherv.
       */
      MPIR_Request * mpid_request = MPID_Request_create_inline();
      mpid_request->kind = MPIR_REQUEST_KIND__COLL;
      *request = mpid_request;
      MPIDI_Request_complete_norelease_inline(mpid_request);

      return rc;
   }


   return 0;
}
