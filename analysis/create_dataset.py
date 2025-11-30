import pandas as pd
import numpy as np
from scapy.all import rdpcap, IP, TCP, UDP
import glob, sys

def get_label(port):
    if 1000<=port<2000: return "Camera"
    if 2000<=port<3000: return "Thermo"
    if 4000<=port<5000: return "Assistant"
    if 5000<=port<6000: return "TV"
    if 6000<=port<7000: return "Laptop"
    return "Unknown"

def get_features(pkts):
    if not pkts: return None
    sizes = [len(p) for p in pkts]
    times = [float(p.time) for p in pkts]
    iat = np.diff(times) if len(times)>1 else [0]
    duration = times[-1]-times[0] if len(times)>1 else 0
    return {
        'packet_count': len(pkts),
        'size_mean': np.mean(sizes),
        'size_std': np.std(sizes),
        'iat_mean': np.mean(iat),
        'iat_std': np.std(iat),
        'duration': duration,
        'bitrate': (sum(sizes)*8)/duration if duration>0 else 0
    }

def main():
    pcap_files = glob.glob("iot-wired-*.pcap")
    if not pcap_files: sys.exit("L Pas de fichier PCAP")
    
    print(f"✅ Analyse de {pcap_files[0]}...")
    try: packets = rdpcap(pcap_files[0])
    except: sys.exit("Erreur lecture")
    
    print(f"✅ {len(packets)} paquets chargés.")
    flows = {}
    
    for p in packets:
        if IP in p:
            port = 0
            proto = ""
            if TCP in p:
                proto = "TCP"
                if 1000 <= p[TCP].dport < 7000: port = p[TCP].dport
                elif 1000 <= p[TCP].sport < 7000: port = p[TCP].sport
            elif UDP in p:
                proto = "UDP"
                if 1000 <= p[UDP].dport < 7000: port = p[UDP].dport
                elif 1000 <= p[UDP].sport < 7000: port = p[UDP].sport
            
            label = get_label(port)
            if label != "Unknown":
                key = (p[IP].src, port, label, proto)
                if key not in flows: flows[key] = []
                flows[key].append(p)
    
    dataset = []
    for (ip, port, label, proto), pkts in flows.items():
        pkts.sort(key=lambda x: float(x.time))
        start = float(pkts[0].time)
        end = float(pkts[-1].time)
        curr = start
        while curr < end:
            win = [p for p in pkts if curr <= float(p.time) < curr+5.0]
            # On accepte 1 seul paquet pour essayer de voir les faibles
            if len(win) >= 1: 
                f = get_features(win)
                f['label'] = label
                f['protocol'] = proto
                f['port'] = port
                dataset.append(f)
            curr += 5.0

    if dataset:
        df = pd.DataFrame(dataset)
        df.to_csv("iot_dataset_final.csv", index=False)
        print("\n=== RÉSULTATS ===")
        print(df['label'].value_counts())
    else:
        print("L Vide.")

if __name__ == "__main__":
    main()