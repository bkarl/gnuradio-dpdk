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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <boost/make_unique.hpp>
#include "dpdk_impl.h"


namespace gr {
  namespace dpdk {

    dpdk::sptr
    dpdk::make(uint16_t udp_rx_port)
    {
      return gnuradio::get_initial_sptr(new dpdk_impl(udp_rx_port));
    }


    /*
     * The private constructor
     */
    dpdk_impl::dpdk_impl(uint16_t udp_rx_port)
      : gr::sync_block( "dpdk",
                        gr::io_signature::make(0,0,0), //input signature
                        gr::io_signature::make(1, 1, sizeof(gr_complex))), // output signature
                        sample_source(boost::make_unique<DpdkSampleSource>(0, 12)),
                        num_samples_transmitted(0)
    {

    }

    /*
     * Our virtual destructor.
     */
    dpdk_impl::~dpdk_impl()
    {
    }

    int
    dpdk_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];
      auto nsamples = sample_source->getSamples(noutput_items, out);
      // Do <+signal processing+>
      // Tell runtime system how many output items we produced.

      //if (num_samples_transmitted)
      //  return WORK_DONE;
      //num_samples_transmitted += noutput_items;
      return nsamples;
    }

  } /* namespace dpdk */
} /* namespace gr */

