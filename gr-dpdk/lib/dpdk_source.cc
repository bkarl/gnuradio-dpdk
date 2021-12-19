#include "dpdk_source.h"
#include <arpa/inet.h>
#include <vector>

namespace gr {
namespace dpdk {

	DpdkSampleSource::DpdkSampleSource(int dpdk_port_id, uint16_t udp_rx_port)
	: 	dpdk_port_id(dpdk_port_id),
		quit(false),
		buffered_overlapping_samples(0),
		udp_rx_port(udp_rx_port)
	{
		setupDpdk();
		setupPort();
		setupRing();
		startRXThread();
	}

	DpdkSampleSource::~DpdkSampleSource()
	{
		stopRXThread();
		rte_eal_cleanup();
	}

	int DpdkSampleSource::getSamples(int number_of_samples, gr_complex *samples)
	{
		auto packets_to_dequeue = ((number_of_samples - buffered_overlapping_samples) / SAMPLES_PER_PACKET) + 1;
		auto sample_idx_overlapping = 0;
		struct rte_mbuf *mbufs[32];
		auto total_samples_received = 0;
		auto packets_dequeued = rte_ring_sc_dequeue_burst(ring, (void **)mbufs, packets_to_dequeue, NULL);

		if (buffered_overlapping_samples != 0) {
			memcpy(samples, overlapping_samples, buffered_overlapping_samples*sizeof(gr_complex));
			total_samples_received += buffered_overlapping_samples;
		}

		for (int packet_idx = 0; packet_idx < packets_dequeued; packet_idx++)
		{
			int16_t *payload = rte_pktmbuf_mtod_offset(mbufs[packet_idx],int16_t *, sizeof(chdr_packet_all_headers));
			for (int sample_idx_packet = 0; sample_idx_packet < SAMPLES_PER_PACKET; sample_idx_packet++)
			{
				if (packet_idx*SAMPLES_PER_PACKET + sample_idx_packet + buffered_overlapping_samples < number_of_samples)
				{
					samples[packet_idx*SAMPLES_PER_PACKET + sample_idx_packet + buffered_overlapping_samples].real(payload[sample_idx_packet*2]);
					samples[packet_idx*SAMPLES_PER_PACKET + sample_idx_packet + buffered_overlapping_samples].imag(payload[sample_idx_packet*2+1]);
					total_samples_received++;
				}
				else
				{
					overlapping_samples[sample_idx_overlapping].real(payload[sample_idx_packet*2]);
					overlapping_samples[sample_idx_overlapping].imag(payload[sample_idx_packet*2+1]);
					sample_idx_overlapping++;
				}
			}
			rte_pktmbuf_free(mbufs[packet_idx]);
		}
		buffered_overlapping_samples = sample_idx_overlapping;
		//printf("Returning %d samples, saving %d\n", total_samples_received, buffered_overlapping_samples);
		return total_samples_received;
	}


    void DpdkSampleSource::setupDpdk()
    {
        int ret;
        uint16_t nr_ports;
        struct rte_flow_error error;
		std::vector<const char *> argv{"dpdk", "-l", "0,1"};
        //char *argv[] = {"dpdk", "-l", "0,1", 0};
        int argc = 3;
        /* Initialize EAL. 8< */
        printf("EAL init start.\n");
        fflush(stdout);
        ret = rte_eal_init(argc, (char **)argv.data());
        if (ret < 0)
          rte_exit(EXIT_FAILURE, ":: invalid EAL arguments\n");
        /* >8 End of Initialization of EAL. */

        nr_ports = rte_eth_dev_count_avail();
        if (nr_ports == 0)
          rte_exit(EXIT_FAILURE, ":: no Ethernet ports found\n");

        if (nr_ports != 1) {
          printf(":: warn: %d ports detected, but we use only one: port %u\n",
            nr_ports, dpdk_port_id);
        }

        /* Allocates a mempool to hold the mbufs. 8< */
        mbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", 1024, 0, 0,
                    RTE_MBUF_DEFAULT_BUF_SIZE,
                    rte_socket_id());
        /* >8 End of allocating a mempool to hold the mbufs. */

        printf("setupDpdk done.\n");
        if (mbuf_pool == NULL)
          rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

    }

	int DpdkSampleSource::RXThread(DpdkSampleSource *sampleSource)
	{
		printf("rx thread started, udp rx port %d\n", sampleSource->getUdpRxPort());
    	fflush(stdout);
		struct rte_mbuf *mbufs[32];
		struct chdr_packet_all_headers *packet;
		struct rte_flow_error error;
		uint16_t nb_rx;
		int ret;
		int j;

		while (!sampleSource->shouldQuit()) 
		{
			nb_rx = rte_eth_rx_burst(sampleSource->getPortID(),SELECTED_QUEUE, mbufs, 32);

			if (nb_rx) 
			{
				for (j = 0; j < nb_rx; j++) 
				{
					struct rte_mbuf *m = mbufs[j];

					packet = rte_pktmbuf_mtod(m, struct chdr_packet_all_headers *);
					//rte_pktmbuf_dump(stdout, m, sizeof(struct chdr_packet_all_headers));
					//printf("next_proto_id %d\n", packet->ipv4_hdr.next_proto_id);
					//printf("ether_type %d\n", packet->ether_hdr.ether_type);

					if (packet->ipv4_hdr.next_proto_id == IPPROTO_UDP && 
						packet->ether_hdr.ether_type == htons(RTE_ETHER_TYPE_IPV4) &&
						packet->udp.dst_port == htons(sampleSource->getUdpRxPort()) &&
						m->pkt_len == PAYLOAD_SIZE + sizeof(struct chdr_packet_all_headers)) 
					{
						rte_ring_enqueue(sampleSource->getRteRing(), (void *)m);
					}
					else
					{
						
						rte_pktmbuf_free(m);
					}
				}
			}
		}
		/* >8 End of reading the packets from all queues. */

		/* closing and releasing resources */
		rte_flow_flush(sampleSource->getPortID(), &error);
		ret = rte_eth_dev_stop(sampleSource->getPortID());
		if (ret < 0)
			printf("Failed to stop port %u: %s",
			       sampleSource->getPortID(), rte_strerror(-ret));
		rte_eth_dev_close(sampleSource->getPortID());
		return 0;
	}


    void DpdkSampleSource::setupPort()
    {
        int ret;
        uint16_t i;
        /* Ethernet port configured with default settings. 8< */
        struct rte_eth_conf port_conf = {
          .rxmode = {
            .split_hdr_size = 0,
          },
        };
        struct rte_eth_rxconf rxq_conf;
        struct rte_eth_dev_info dev_info;

        ret = rte_eth_dev_info_get(dpdk_port_id, &dev_info);
        if (ret != 0)
          rte_exit(EXIT_FAILURE,
            "Error during getting device (port %u) info: %s\n",
            dpdk_port_id, strerror(-ret));

        port_conf.txmode.offloads &= dev_info.tx_offload_capa;
        printf(":: initializing port: %d\n", dpdk_port_id);

        ret = rte_eth_dev_configure(dpdk_port_id, NR_QUEUES, 0, &port_conf); //configure for tx only
        if (ret < 0) {
          rte_exit(EXIT_FAILURE,
            ":: cannot configure device: err=%d, port=%u\n",
            ret, dpdk_port_id);
        }

        rxq_conf = dev_info.default_rxconf;
        rxq_conf.offloads = port_conf.rxmode.offloads;
        /* >8 End of ethernet port configured with default settings. */

        /* Configuring RX queue connected to single port. 8< */
        for (i = 0; i < NR_QUEUES; i++) {
          ret = rte_eth_rx_queue_setup(dpdk_port_id, i, 512,
                   rte_eth_dev_socket_id(dpdk_port_id),
                   &rxq_conf,
                   mbuf_pool);
          if (ret < 0) {
            rte_exit(EXIT_FAILURE,
              ":: Rx queue setup failed: err=%d, port=%u\n",
              ret, dpdk_port_id);
          }
        }

        /* Setting the RX port to promiscuous mode. 8< */
        ret = rte_eth_promiscuous_enable(dpdk_port_id);
        if (ret != 0)
          rte_exit(EXIT_FAILURE,
            ":: promiscuous mode enable failed: err=%s, port=%u\n",
            rte_strerror(-ret), dpdk_port_id);
        /* >8 End of setting the RX port to promiscuous mode. */

        /* Starting the port. 8< */
        ret = rte_eth_dev_start(dpdk_port_id);
        if (ret < 0) {
          rte_exit(EXIT_FAILURE,
            "rte_eth_dev_start:err=%d, port=%u\n",
            ret, dpdk_port_id);
        }
        /* >8 End of starting the port. */
        assertLinkStatus();

        printf(":: initializing port: %d done\n", dpdk_port_id);
    }

    void DpdkSampleSource::setupRing()
    {
    	 printf("setting up rx ringbuffer\n");
    	ring = rte_ring_create("rx_ring", RING_SIZE, rte_socket_id(), RING_F_SP_ENQ | RING_F_SC_DEQ);
    }

    void DpdkSampleSource::startRXThread()
    {

        rte_eal_remote_launch((lcore_function_t *)RXThread, this, 1);
        printf("rx thread started\n");
    }

    void DpdkSampleSource::stopRXThread()
    {
    	quit = true;
    	rte_eal_wait_lcore(1);
    }

    void DpdkSampleSource::assertLinkStatus(void)
    {
		struct rte_eth_link link = {0};
		int link_get_err = -EINVAL;

		link_get_err = rte_eth_link_get(dpdk_port_id, &link);
		if (link_get_err != 0 || link.link_status != RTE_ETH_LINK_UP)
			rte_exit(EXIT_FAILURE,"Link is not up");
    }

} //gr
} //dpdk