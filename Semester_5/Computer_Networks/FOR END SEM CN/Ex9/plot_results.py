import csv
import matplotlib.pyplot as plt

files = [("TCP Reno", "reno.csv"), ("TCP CUBIC", "cubic.csv")]
rows = []
for label, filename in files:
    with open(filename, newline="") as f:
        rows.append((label, next(csv.DictReader(f))))

metrics = [
    ("throughputMbps", "Throughput (Mbps)"),
    ("meanDelayMs", "Mean delay (ms)"),
    ("lostPackets", "Lost packets"),
    ("lossPercent", "Packet loss (%)"),
]

fig, axes = plt.subplots(2, 2, figsize=(10, 7))
for ax, (key, title) in zip(axes.flat, metrics):
    ax.bar([label for label, _ in rows],
           [float(row[key]) for _, row in rows])
    ax.set_title(title)
    ax.grid(axis="y", alpha=0.3)
fig.tight_layout()
fig.savefig("tcp_comparison.png", dpi=160)
print("Graph saved as tcp_comparison.png")
