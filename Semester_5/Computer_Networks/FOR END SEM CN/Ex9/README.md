# Minimal modification of the original NS-3 code

This version keeps the original function names:
- `NodeCreation()`
- `LinkCreation()`
- `CongestionMaking()`
- `ErrorModeling()`
- `OutputSummary()`

It adds the missing connection between nodes 1 and 2, TCP Reno/CUBIC selection,
TCP sender/receiver applications, FlowMonitor summary output, and CSV data for plots.

Important: the original functions describe only four used nodes. This minimal
version therefore requires `--nodes=4`; although the node count variable is
customizable for creation, additional nodes are not connected by this topology.
That is intentional to avoid restructuring the user's original function layout.

Run from the NS-3 root (modern NS-3):
```bash
cp tcpRenoCubic.cpp scratch/tcpRenoCubic.cpp
./ns3 run "scratch/tcpRenoCubic --tcp=Reno --output=reno.csv"
./ns3 run "scratch/tcpRenoCubic --tcp=Cubic --output=cubic.csv"
```

Generate graphs:
```bash
python3 -m pip install matplotlib
python3 plot_results.py
```

Optional settings:
```bash
./ns3 run "scratch/tcpRenoCubic --tcp=Reno --time=20 --bottleneckRate=2Mbps --errorRate=0 --output=reno.csv"
```
