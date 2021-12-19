#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 bk.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import dpdk_swig as dpdk
import numpy as np
import time
class qa_dpdk(gr_unittest.TestCase):

	def setUp(self):
		self.tb = gr.top_block()

	def tearDown(self):
		self.tb = None

	def test_001_t(self):
		expected_result = np.zeros((4096), dtype=np.complex64)
		expected_result.real = 1
		# set up fg
		dpdk_inst = dpdk.dpdk(udp_rx_port=12)
		# "Instantiate the binary sink"
		dst = blocks.vector_sink_c();
		self.tb.connect(dpdk_inst, dst)
		#self.tb.connect(dpdk_inst, dst)
		self.tb.run()
		# check data
		result_data = dst.data()
		self.assertComplexTuplesAlmostEqual(expected_result, result_data)
		#self.assertEqual(len(expected_result), len(result_data))


if __name__ == '__main__':
	gr_unittest.run(qa_dpdk)
