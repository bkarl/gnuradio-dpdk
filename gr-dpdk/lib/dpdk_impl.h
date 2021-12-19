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

#ifndef INCLUDED_DPDK_DPDK_IMPL_H
#define INCLUDED_DPDK_DPDK_IMPL_H

#include <dpdk/dpdk.h>
#include "dpdk_source.h"

namespace gr {
  namespace dpdk {

    class dpdk_impl : public dpdk
    {
     private:
      // Nothing to declare in this block.
      std::unique_ptr<IDpdkSampleSource> sample_source;
      int num_samples_transmitted;
     public:
        dpdk_impl(uint16_t udp_rx_port);
        ~dpdk_impl();

        // Where all the action really happens
        int work(
                int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items
        );
    };

  } // namespace dpdk
} // namespace gr

#endif /* INCLUDED_DPDK_DPDK_IMPL_H */

