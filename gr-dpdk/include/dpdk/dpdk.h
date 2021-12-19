/* -*- c++ -*- */
/*
 * Copyright 2021 bk.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DPDK_DPDK_H
#define INCLUDED_DPDK_DPDK_H

#include <dpdk/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dpdk {

    /*!
     * \brief <+description of block+>
     * \ingroup dpdk
     *
     */
    class DPDK_API dpdk : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dpdk> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dpdk::dpdk.
       *
       * To avoid accidental use of raw pointers, dpdk::dpdk's
       * constructor is in a private implementation
       * class. dpdk::dpdk::make is the public interface for
       * creating new instances.
       */
      static sptr make(uint16_t udp_rx_port);
    };

  } // namespace dpdk
} // namespace gr

#endif /* INCLUDED_DPDK_DPDK_H */

