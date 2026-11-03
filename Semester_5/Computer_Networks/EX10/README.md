# EX10: Network Simulator 3 (NS-3)

Discrete-event network simulation modeling point-to-point topologies, congestion control, and throughput analysis.

## Overview

This exercise utilizes **NS-3** (Network Simulator 3) in C++ to model realistic network scenarios:
- Point-to-point links with configurable data rates (e.g. 5 Mbps, 2 Mbps) and propagation delays (e.g. 2 ms, 20 ms).
- Node placement and container architecture.
- Internet stack installation (IPv4, routing tables, ARP).
- Traffic generation using `OnOffApplication` and packet sink receivers.
- Tracing with `AsciiTraceHelper` and `PcapHelper` for Wireshark inspection.

## Files

| File | Description |
|------|-------------|
| `ns3.cc` | NS-3 simulation script defining topology, traffic generators, and flow monitors |
| `Questions.pdf` | Lab assignment specification and topology diagram |
| `NS-3.docx` | Experiment documentation and analysis report |

## Running in NS-3

Copy `ns3.cc` to your NS-3 `scratch/` directory:

```bash
cp ns3.cc <ns-3-folder>/scratch/network_sim.cc
cd <ns-3-folder>
./ns3 run scratch/network_sim
```
