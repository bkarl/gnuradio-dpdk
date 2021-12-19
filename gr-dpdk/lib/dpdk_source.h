#pragma once

#include <gnuradio/gr_complex.h>
#include <rte_eal.h>
#include <rte_common.h>
#include <rte_malloc.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_net.h>
#include <rte_flow.h>
#include <rte_cycles.h>
#include <rte_udp.h>

namespace gr {
namespace dpdk {

class IDpdkSampleSource 
{
public:
	virtual int getSamples(int numberOfSamples, gr_complex *samples) = 0;
};

class DpdkDummySampleSource : public IDpdkSampleSource
{
public:
	int getSamples(int numberOfSamples, gr_complex *samples)
	{
		for (int i=0; i < numberOfSamples; i++)
		{
			samples[i].real(0);
			samples[i].real(1);
		}
		return numberOfSamples;
	}
};

class DpdkSampleSource : public IDpdkSampleSource
{

public:
	~DpdkSampleSource();
	DpdkSampleSource(int dpdk_port_id, uint16_t udp_rx_port);
	int getSamples(int number_of_samples, gr_complex *samples);

	bool shouldQuit() { return quit; };
	int getPortID() { return dpdk_port_id; };
	uint16_t getUdpRxPort() { return udp_rx_port; };
	struct rte_ring* getRteRing() { return ring; };
private:
	constexpr static uint16_t NR_QUEUES = 1;
	constexpr static uint8_t SELECTED_QUEUE = 0;
	constexpr static auto RING_SIZE = 1024;
	constexpr static auto PAYLOAD_SIZE = 1464; //1472 is max standard UDP payload - 8 byte chdr
	constexpr static auto SAMPLE_SIZE_IN_BYTE = 4;

	constexpr static auto SAMPLES_PER_PACKET = PAYLOAD_SIZE/SAMPLE_SIZE_IN_BYTE;

	void setupPort();
    void setupDpdk();
    void assertLinkStatus();
    static int RXThread(DpdkSampleSource *sampleSource);
    //void RXThreadWrapper(DpdkSampleSource* sampleSource);

    void setupRing();
    void startRXThread();
    void stopRXThread();

    int dpdk_port_id;
    uint16_t udp_rx_port;
    bool quit;
    gr_complex overlapping_samples[SAMPLES_PER_PACKET];
    int buffered_overlapping_samples;


    struct udp_hdr {
            uint16_t src_port;    
            uint16_t dst_port;    
            uint16_t dgram_len;   
            uint16_t dgram_cksum; 
    } __attribute__((__packed__));
	
    struct chdr_packet_all_headers {
        
        struct rte_ether_hdr ether_hdr;
        struct rte_ipv4_hdr ipv4_hdr;
        struct udp_hdr udp;
        uint64_t chdr;
    } __attribute__((__packed__));



	struct rte_mempool *mbuf_pool;
	struct rte_ring *ring;
	//struct rte_flow *flow;
};

} //gr
} //dpdk