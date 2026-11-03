# Code for both TCP lab questions

Start here. `tcp-lab.cc` is the actual C++ simulation for **both questions**, selected
with `--question=1` or `--question=2`. It targets **ns-3.46.1**. These are NS-3
programs; do not run them with NS-2, `ns`, or a standalone C++ compiler.

| File | Purpose |
|---|---|
| `tcp-lab.cc` | Creates the topology, runs TCP, records measurements |
| `run_lab.py` | Builds the C++ program and runs the experiment matrix |
| `plot_lab.py` | Reads real CSV results and creates PNG graphs |
| `validation-results/` | Results generated while checking this code on your Mac |

## 1. Prepare your Mac once

If you already followed `../START_HERE_MACBOOK_PLAN.md`, use that installation.
Otherwise, run the following in Terminal. Homebrew and Apple command-line tools
were already available when this folder was inspected.

```bash
brew install cmake ninja
cd "$HOME/Desktop/NS3"
mkdir -p simulator
cd simulator
git clone --depth 1 --branch ns-3.46.1 https://gitlab.com/nsnam/ns-3-dev.git ns-3.46.1
cd ns-3.46.1
./ns3 configure --build-profile=debug --disable-python-bindings --enable-modules='core;network;internet;point-to-point;applications;flow-monitor;traffic-control'
./ns3 build
```

If the clone directory already exists, enter it instead of cloning again.
The detached-HEAD message for this fixed release is normal. Stop if a command
fails; do not continue blindly. The initial build takes much longer than later runs.

Prepare the Python environment for plotting:

```bash
cd "$HOME/Desktop/NS3/lab-code"
python3 -m venv .venv
source .venv/bin/activate
python -m pip install matplotlib
```

In a new Terminal window, run `cd "$HOME/Desktop/NS3/lab-code"` and
`source .venv/bin/activate` again. Python is only running experiments/plotting;
the simulator itself is C++ and needs no Python bindings.

## 2. Run Question 1

```bash
cd "$HOME/Desktop/NS3/lab-code"
source .venv/bin/activate
python run_lab.py q1 --ns3 ../simulator/ns-3.46.1 --output results-q1
python plot_lab.py results-q1
open results-q1/comparison.png
open results-q1/q1-baseline/cwnd.png
```

The script copies `tcp-lab.cc` into the simulator's `scratch` directory, builds it,
and runs these three cases:

| Case | Bottleneck | Queue capacity |
|---|---|---|
| Baseline | 1 Mbps | 20 packets |
| Larger queue | 1 Mbps | 100 packets |
| Faster link | 5 Mbps | 20 packets |

Every case has **exactly three nodes**:

```text
Sender -------- Router -------- Receiver
      10 Mbps          bottleneck
      2 ms delay       10 ms delay
```

The sender continuously offers TCP data with `BulkSendApplication`; the receiver
uses `PacketSink`. NewReno is explicitly selected. The sender starts at 1 second,
stops adding data at 30 seconds, and the simulation ends at 45 seconds to allow
queued/in-flight data to drain. TCP maximum segment size is 1448 bytes.

Read `summary.csv`, inspect `cwnd.png` and `goodput.png`, then compare the cases.
Explain rising cwnd, sudden reductions, and goodput changes using actual evidence
from `events.csv`. A saturated TCP sender can cause congestion even on the faster
link. A larger queue can increase delay; do not assume it always improves results.

## 3. Run Question 2

Start with one run per condition to understand the outputs:

```bash
python run_lab.py q2 --ns3 ../simulator/ns-3.46.1 --output results-q2-demo --runs 1
python plot_lab.py results-q2-demo
open results-q2-demo/comparison.png
```

For the full experiment:

```bash
python run_lab.py q2 --ns3 ../simulator/ns-3.46.1 --output results-q2 --runs 5
python plot_lab.py results-q2
open results-q2/comparison.png
```

Topology: four senders, two routers, four receivers (ten nodes).

```text
S0 ----\                          /---- D0
S1 ----- R0 ---- bottleneck ---- R1 ----- D1
S2 ----/                          \---- D2
S3 ---/                            \--- D3
```

Each sender sends to its matching receiver. Access links are 100 Mbps / 2 ms.
The bottleneck is 10 ms with a 20-packet FIFO queue. All four flows use NewReno in
one experiment; all four use Cubic in the matching experiment. Only the congestion
control choice changes within each pair; other TCP settings retain release defaults.

| Condition | Bottleneck | Forward packet error probability |
|---|---|---|
| Wide | 10 Mbps | 0 |
| Narrow | 2 Mbps | 0 |
| Low random error | 2 Mbps | 0.001 = 0.1% |
| High random error | 2 Mbps | 0.01 = 1% |

4 conditions × 2 algorithms × 5 runs = **40 simulations**. Senders start at
1.000, 1.001, 1.002, and 1.003 s and stop at 61 s. Goodput/fairness use the same
**[11, 61) s** window, excluding startup. The simulation drains until 76 s.
Random errors occur only at the forward bottleneck receiver; ACKs have no random
error model. Bandwidth limitation produces queue congestion; random errors are a
separate loss mechanism.

The seed is 12345 and each repeated run has a different run number. Error-free
cases are deterministic and will repeat identically; those repeats are not five
independent statistical samples. Matching random stream numbers do not guarantee
the same individual packets are lost across algorithms because their traffic differs.

## 4. Understand the outputs

Each run gets its own directory. Existing directories are rejected to avoid mixing
results. To rerun, choose a new `--output` directory.

| Output | Meaning |
|---|---|
| `cwnd.csv`, `cwnd.png` | Sender TCP congestion-window changes, in **bytes** |
| `goodput.csv`, `goodput.png` | Application bytes received in each preceding 1-second bin, converted to Mbps |
| `events.csv` | Forward bottleneck queue drops, link-error drops, sender data retransmissions |
| `summary.csv` | One row per forward TCP flow |
| `run.csv` | Parameters, aggregate application goodput, Jain fairness, direct drop counts |
| `flow-monitor.xml` | Detailed FlowMonitor statistics, including reverse ACK flows |
| `comparison.csv`, `comparison.png` | Combined results and six comparison graphs |

Metric definitions matter in your record:

- **Application goodput** = newly delivered application bytes × 8 / measurement
  window / 1,000,000. TCP removes duplicate/retransmitted bytes before the sink
  receives them. This is the main useful-throughput metric in the comparison graph.
- **IP throughput** in `summary.csv` includes IP/TCP headers and uses the full
  observation interval from 1 s until simulation end, including drain time.
  It has a different denominator from steady-window goodput and must be labelled.
- **PDR** = 100 × received forward IP packets / transmitted forward IP packets.
  Reverse ACK flows are excluded by matching source/destination addresses and port.
- **Mean delay** = sum of delivered forward IP packet delays / received packet
  count, in milliseconds. This is not application recovery/reordering delay.
- **Lost packets** are FlowMonitor's whole-run loss count after draining and its
  lost-packet check. `tx_minus_rx` is also exported as a consistency diagnostic.
  If it differs from `lost_packets`, or increasing `--cleanup` changes results
  materially, investigate/drain longer before drawing conclusions.
- **Retransmissions** count sender **data-segment retransmission events** using
  ns-3.46.1's `Retransmission` trace. Pure control packets are excluded. A segment
  retransmitted twice counts twice. This directly improves on the older plan's
  suggestion to reconstruct retransmissions from sequence-number overlaps.
- **Jain fairness** = `(sum of per-flow goodputs)^2 / (N × sum of squared goodputs)`.
  With four active flows, 1 means equal throughput. Equal throughput alone does
  not imply high throughput. The all-zero case is undefined (`nan`).
- IP metrics, retransmissions and drop counts cover the whole run; application
  goodput and fairness use the stated measurement window. Do not call all metrics
  steady-state measurements. Both members of each comparison use the same windows.

There is no root queue discipline on the links: the code explicitly removes the
default qdisc after address assignment and disables device flow control. Thus the configured DropTail device queue
is the queue being studied. Access device queues hold 100 packets; both directions
of the bottleneck have the specified capacity. Direct drop counters cover **only
the forward bottleneck**, not every queue in the topology, and can include control
packets. Therefore do not require direct drop counts to equal data retransmissions.

The scripts produce real measured results; they do not manufacture an expected
winner. The comparison graph displays means and sample standard deviations across
runs. Zero error bars on deterministic cases are expected. For cwnd evolution,
compare corresponding NewReno/Cubic run PNGs, not an average of misaligned curves.

## 5. Run one case manually / change parameters

After the runner has copied the source:

```bash
cd "$HOME/Desktop/NS3/simulator/ns-3.46.1"
./ns3 run 'tcp-lab --question=1 --tcp=NewReno --bottleneckMbps=1 --queuePackets=20 --output=my-q1'
./ns3 run 'tcp-lab --question=2 --flows=4 --tcp=Cubic --bottleneckMbps=2 --errorRate=0.001 --duration=61 --warmup=10 --output=my-q2'
./ns3 run 'tcp-lab --PrintHelp'
```

These relative output paths are inside the ns-3 directory. `duration` means the
absolute sender stop time, not the amount of time after startup. Keep it above
`1 + warmup` and above all sender start times. Use `--cleanup=30` to check whether
the normal 15-second drain is sufficient for a modified scenario.

To understand the C++ file, read it in this order: command-line parameters → node
creation → link installation/addressing → TCP applications → trace attachment →
simulation run → summary calculations. `Attach()` waits 1 ns after BulkSend starts
because its socket does not exist before startup. This is before any data handshake
can complete on the millisecond-delay topology.

For your lab record, include the aim, topology, parameter table, algorithm selected,
code, command/output screenshot, cwnd and comparison graphs, measurement definitions,
observations and conclusion. Explain what the measured changes suggest; do not claim
“Cubic always wins.” This supplies both NS-3 self-questions, not a separate QualNet lab.

References: [official TCP model documentation](https://www.nsnam.org/docs/models/html/tcp.html),
[FlowMonitor measurement caveats](https://www.nsnam.org/docs/models/html/flow-monitor.html),
and the [ns-3.46.1 tagged implementation](https://gitlab.com/nsnam/ns-3-dev/-/tree/ns-3.46.1).
The source/build checks used the fixed release rather than assuming development
documentation has exactly the same API.
