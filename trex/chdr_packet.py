from trex_stl_lib.api import *
import argparse
import numpy as np

PAYLOAD_SIZE = 1464

class STLS1(object):

    def genSignal(self, no_bytes):
        fs = 1e9/32
        samples_per_packet = no_bytes//4
        n = np.arange(samples_per_packet) / float(fs)

        sine_freq = fs/samples_per_packet #calculates the largest sine period that fits in one packet
        res = 2*np.pi*sine_freq*n

        i = np.cos(res) * (2**15 - 1)
        q = np.sin(res) * (2**15 - 1)

        ip_packet = np.zeros((2*samples_per_packet), dtype=np.int16)
        ip_packet[::2] = i.astype(np.int16)
        ip_packet[1::2] = q.astype(np.int16)

        return ip_packet.tobytes()

    def create_stream (self):
        array = self.genSignal(PAYLOAD_SIZE)

        print(array)
        # Create base packet and pad it to size
        base_pkt =  Ether(dst="ff:ff:ff:ff:ff:ff")/IP(src="192.168.188.93", dst="192.168.188.93")/UDP(dport=12,sport=1025)/b'\x00\x00\x00\x00\x00\x00\x00\x00'/array#/

        vm = STLScVmRaw( [ STLVmFlowVar(name="seqnum", min_value=0, max_value=2**12-1, size=2, op="inc"),
                           STLVmWrFlowVar(fv_name="seqnum", pkt_offset=14+20+8)
                          ]
                       )

        return STLStream(packet = STLPktBuilder(pkt = base_pkt, vm=vm), mode = STLTXCont())

    def get_streams (self, tunables ,**kwargs):
        parser = argparse.ArgumentParser(description='Argparser for {}'.format(os.path.basename(__file__)), 
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)

        args = parser.parse_args(tunables)
        # create 1 stream 
        return [ self.create_stream() ]


# dynamic load - used for trex console or simulator
def register():
    return STLS1()



