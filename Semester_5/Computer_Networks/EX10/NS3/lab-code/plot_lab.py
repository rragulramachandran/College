#!/usr/bin/env python3
"""Plot simulation CSV files; requires matplotlib. Accepts a run or results folder."""
import argparse
import csv
from collections import defaultdict
from pathlib import Path
import statistics
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

def read(path):
    with path.open(newline="") as stream:
        return list(csv.DictReader(stream))

def line_plot(folder, filename, field, ylabel, step=False):
    groups = defaultdict(list)
    for row in read(folder / filename):
        groups[int(row["flow"])].append(row)
    fig, ax = plt.subplots(figsize=(9, 4.5), layout="constrained")
    for flow, rows in sorted(groups.items()):
        x = [float(r["time_s"]) for r in rows]
        y = [float(r[field]) for r in rows]
        if step:
            # Include the old value immediately before the first observed change.
            x.insert(0, x[0]); y.insert(0, float(rows[0]["old_cwnd_bytes"]))
            ax.step(x, y, where="post", label=f"Flow {flow}", linewidth=1)
        else:
            ax.plot(x, y, label=f"Flow {flow}", linewidth=1)
    ax.set(xlabel="Simulation time (s)", ylabel=ylabel, title=folder.name)
    ax.grid(alpha=0.25); ax.legend()
    fig.savefig(folder / (Path(filename).stem + ".png"), dpi=160)
    plt.close(fig)

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("results", type=Path)
a = p.parse_args()
root = a.results.expanduser().resolve()
folders = [root] if (root / "run.csv").exists() else sorted(x.parent for x in root.glob("*/run.csv"))
if not folders:
    p.error("No run.csv found; run the simulations first")
combined, grouped = [], defaultdict(list)
for folder in folders:
    line_plot(folder, "cwnd.csv", "cwnd_bytes", "Congestion window (bytes)", step=True)
    line_plot(folder, "goodput.csv", "goodput_mbps", "Application goodput over preceding 1 s (Mbps)")
    run = read(folder / "run.csv")[0]
    flows = read(folder / "summary.csv")
    total_rx = sum(int(f["rx_packets"]) for f in flows)
    total_tx = sum(int(f["tx_packets"]) for f in flows)
    run["pdr_percent"] = 100 * total_rx / total_tx if total_tx else float("nan")
    run["mean_delay_ms"] = (sum(float(f["mean_delay_ms"]) * int(f["rx_packets"])
                                  for f in flows if int(f["rx_packets"])) / total_rx
                            if total_rx else float("nan"))
    run["lost_packets"] = sum(int(f["lost_packets"]) for f in flows)
    run["retransmitted_data_segments"] = sum(int(f["retransmitted_data_segments"]) for f in flows)
    run["directory"] = folder.name
    combined.append(run)
    key = (run["question"], run["bottleneck_mbps"], run["queue_packets"], run["error_rate"], run["tcp"])
    grouped[key].append(run)
with (root / "comparison.csv").open("w", newline="") as stream:
    writer = csv.DictWriter(stream, fieldnames=list(combined[0]))
    writer.writeheader(); writer.writerows(combined)
metrics = [("aggregate_goodput_mbps", "Application goodput (Mbps)"),
           ("jain_fairness", "Jain fairness"), ("mean_delay_ms", "Mean delivered IP delay (ms)"),
           ("lost_packets", "FlowMonitor lost IP packets"),
           ("retransmitted_data_segments", "Retransmitted data segments"),
           ("pdr_percent", "IP packet delivery ratio (%)")]
fig, axes = plt.subplots(3, 2, figsize=(15, 15), layout="constrained")
keys = sorted(grouped)
labels = [f"Q{k[0]} {k[4]}\n{k[1]} Mbps / {k[2]} packets\nerror={k[3]}" for k in keys]
for ax, (field, label) in zip(axes.flat, metrics):
    values = [[float(row[field]) for row in grouped[key]] for key in keys]
    means = [statistics.mean(v) for v in values]
    deviations = [statistics.stdev(v) if len(v) > 1 else 0 for v in values]
    ax.bar(range(len(keys)), means, yerr=deviations, capsize=3,
           color=["#247ba0" if k[4] == "NewReno" else "#e07a32" for k in keys])
    ax.set_xticks(range(len(keys)), labels, rotation=35, ha="right", fontsize=8)
    ax.set_ylabel(label); ax.grid(axis="y", alpha=0.25)
fig.suptitle("TCP lab comparisons — error bars: sample standard deviation across runs")
fig.savefig(root / "comparison.png", dpi=160)
plt.close(fig)
print(f"Saved per-run cwnd.png/goodput.png and {root / 'comparison.png'}")
