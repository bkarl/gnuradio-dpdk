/* -*- c++ -*- */

#define DPDK_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "dpdk_swig_doc.i"

%{
#include "dpdk/dpdk.h"
%}

%include "dpdk/dpdk.h"
GR_SWIG_BLOCK_MAGIC2(dpdk, dpdk);
