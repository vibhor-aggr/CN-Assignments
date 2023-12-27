import scapy.all as scapy
import matplotlib.pyplot as plt

def calculate_metrics(pcap_file):
    throughput = {}
    latency = {}

    packets = scapy.rdpcap(pcap_file)
    for packet in packets:
        if scapy.TCP in packet:
            src_port = packet[scapy.TCP].sport
            dst_port = packet[scapy.TCP].dport
            flow_num = (packet[scapy.IP].src, src_port, packet[scapy.IP].dst, dst_port)

            if flow_num not in throughput:
                throughput[flow_num] = 0
            throughput[flow_num] += len(packet) * 8

            if flow_num not in latency:
                latency[flow_num] = []
            latency[flow_num].append(packet.time - packets[0].time)

    average_throughput = {k: v / max(latency[k]) for k, v in throughput.items()}
    average_latency = {k: sum(latency[k]) / len(latency[k]) * 1000 for k in latency}

    return average_throughput, average_latency

def plot_metrics(throughput, latency):
    plt.figure(figsize=(12, 6))
    
    plt.subplot(1, 2, 1)
    for flow, value in throughput.items():
        plt.bar(str(flow), value)
    plt.xlabel('Flow')
    plt.ylabel('Average Throughput (bps)')
    plt.title('Average Throughput for each TCP Flow')

    plt.subplot(1, 2, 2)
    for flow, value in latency.items():
        plt.bar(str(flow), value)
    plt.xlabel('Flow')
    plt.ylabel('Average Latency (ms)')
    plt.title('Average Latency for each TCP Flow')

    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    pcap_file = 'C:/Users/Vibhor Aggarwal/Documents/5thSem/CN/A3/epoll_3000.pcap'
    avg_throughput, avg_latency = calculate_metrics(pcap_file)
    plot_metrics(avg_throughput, avg_latency)
