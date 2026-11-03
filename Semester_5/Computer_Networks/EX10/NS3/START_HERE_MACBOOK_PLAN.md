 # Your NS-3 TCP lab plan for MacBook

You can do these exercises on your MacBook. Start with a native NS-3 installation, learn the supplied small examples, and then write two TCP experiments. The folder is teaching material, not an installed simulator or a finished solution.

The main reason the task looks confusing is that the lab sheet mixes simulators and the supporting documents mix NS-3 versions. The title and both self-questions ask for NS-3, but the sheet's `set n0 [$ns node]`, `Agent/TCP`, `Application/FTP`, `$ns run`, and NAM diagram belong to NS-2. They cannot be pasted into an NS-3 C++ program. QualNet is also mentioned in the objectives, but neither self-question requests a QualNet implementation. Follow the explicit NS-3 questions unless your teacher separately requires something else.

This is a plan, not an execution report. No simulator has been installed or built, and no experimental results have been generated in this review. The original 16 files are unchanged. See [FILE_BY_FILE_REVIEW.md](FILE_BY_FILE_REVIEW.md) for the analysis of every supplied file.

## 1. What you are actually building

NS-3 simulates imaginary computers and links inside a program. You do not need physical routers, multiple laptops, or changes to your Mac's network settings.

The workflow is:

```text
C++ experiment (.cc)
        |
        v
NS-3 builds and runs a virtual network
        |
        +--> measurements and event traces
        +--> packet capture files, if enabled
        |
        v
Python plots graphs --> you explain the results in your lab record
```

These terms are enough to start:

| Term | Meaning in your experiment |
|---|---|
| Node | A simulated computer or router. |
| NetDevice | A simulated network interface attached to a node. |
| Channel/link | A simulated connection between interfaces. |
| Bandwidth/data rate | How many bits the link can transmit each second. |
| Propagation delay | Time for a signal to travel across a link; only one component of total delay. |
| Queue | Where packets wait when the link is busy. A finite queue can overflow. |
| Bottleneck | The link that limits the path's capacity. |
| TCP flow | One TCP connection carrying data from sender to receiver, with acknowledgements returning. |
| cwnd | TCP's congestion window: a sender-side limit on outstanding, unacknowledged data. It is not the router queue size. |
| Trace | A record of simulation events or changing values. |

TCP increases its sending window as acknowledgements arrive and adjusts it when it detects congestion/loss. NewReno and Cubic are alternative rules for that adjustment. You select the existing implementations; you do not need to implement TCP yourself. Their behavior is described in the [official TCP model documentation](https://www.nsnam.org/docs/models/html/tcp.html).

## 2. What is already on your Mac

Read-only checks during this review found:

| Item | Observed state |
|---|---|
| Architecture | `arm64`, Apple Silicon |
| macOS | `27.0`, build `26A428` |
| Apple Command Line Tools | Present at `/Library/Developer/CommandLineTools` |
| C++ compiler | Apple Clang 21 |
| Homebrew | Present at `/opt/homebrew/bin/brew` |
| Python | Python 3.13.3 |
| Git | Present |
| CMake and Ninja | Not found on the current shell's PATH |
| This NS3 folder | Handouts and examples; no NS-3 source/build tree |

The checks establish prerequisites, not a successful NS-3 build on this exact OS/toolchain. The next milestone is to build and run a tutorial example.

Use native macOS first. NS-3 documents macOS support with Apple's developer tools and CMake plus a build tool such as Ninja. See the [official installation guide](https://www.nsnam.org/docs/installation/singlehtml/index.html). A Linux VM is a fallback if a particular old college environment becomes mandatory; it is not a prerequisite for these two questions.

## 3. Set up the simulator

The commands below deliberately use a fixed release, **ns-3.46.1**, rather than a changing development branch. Its official Git tag was verified during this review. This is a reproducible baseline, not a claim that it is the newest release. The [release page](https://www.nsnam.org/releases/ns-3-46/) recommends 3.46.1 over 3.46 for build fixes.

Open **Terminal** using Spotlight: press Command-Space, type `Terminal`, and press Return. Run each block in order. If a command fails, stop there and keep its error message; the next command may depend on it succeeding. Do not type the `$` or `student@...` prompt text shown in the old handouts.

First, install the missing build tools:

```bash
brew install cmake ninja
```

Then download an isolated simulator directory alongside the teacher's files:

```bash
cd "$HOME/Desktop/NS3"
mkdir -p simulator
cd simulator
git clone --depth 1 --branch ns-3.46.1 https://gitlab.com/nsnam/ns-3-dev.git ns-3.46.1
cd ns-3.46.1
```

A Git message about a detached HEAD is expected when checking out a release tag. If the destination already exists from an earlier attempt, inspect/use it rather than deleting it or cloning over it.

Configure a focused build for the wired labs:

```bash
./ns3 configure --build-profile=debug --enable-examples --enable-tests --disable-python-bindings --enable-modules='core;network;internet;point-to-point;applications;flow-monitor;traffic-control;csma'
./ns3 build
./ns3 run first
```

The configuration flags were checked against the tagged release's launcher. `first` should show a UDP packet sent, received at the server, echoed, and received back at the client. Wording can differ from the old screenshots. Optional disabled features are not necessarily errors; the requested modules and example must build successfully.

Run the enabled test suite once:

```bash
./test.py
```

Resolve failures relevant to this build before relying on experiment results. A configuration with zero tests enabled is not a meaningful verification.

Python bindings are disabled because the simulations will be C++. Python is still needed by the build launcher, and we will use ordinary Python separately to plot data. This avoids adding an unnecessary bindings setup to your first lab.

Prepare a separate plotting environment:

```bash
cd "$HOME/Desktop/NS3"
mkdir -p lab-work/scripts lab-work/results lab-work/plots lab-work/report
python3 -m venv lab-work/.venv
source lab-work/.venv/bin/activate
python -m pip install matplotlib
```

When opening a new Terminal later, reactivate that environment before running plotting scripts. You do not need Java, TraceMetrics, Qt, NetAnim, PyViz, or Wireshark for the first working solution. NetAnim can be added later if an animation screenshot is explicitly required. Graphs and numerical results address the actual performance questions.

## 4. Learn only the relevant examples first

Allow roughly 1–2 hours for the basics, apart from download/build time. Treat this as an estimate, not a deadline.

1. Read the overview's explanation of nodes, devices, channels and applications. Then read `first.cc` alongside the code-structure slides, pages 3–14.
2. Run `first`, change its packet count in a separately named scratch copy, and run it again. Explain what each helper is creating.
3. Read/run `second` to understand a node with two interfaces and routing between two subnets. This example uses a LAN; your first lab will use two point-to-point links instead.
4. Read/run `fourth` to understand a callback that records a changing value. Its expected message is `Traced 0 to 1234`; it is not a network simulation.
5. Read wired-network slides 9–11 for `BulkSendHelper` and `PacketSinkHelper`, then read the five-page tracing handout.
6. Look at the release's `examples/tutorial/fifth.cc` for a real TCP congestion-window tracing example. That file was referenced by the slides but was not supplied in your folder. Use it to learn the tracing connection; its original topology is not your completed lab.

Use the examples shipped with the selected release for initial runs. Preserve the teacher's copies for comparison. To run a separately named copy of the supplied `first.cc`:

```bash
cd "$HOME/Desktop/NS3/simulator/ns-3.46.1"
cp "$HOME/Desktop/NS3/first.cc" scratch/lab-intro.cc
./ns3 run scratch/lab-intro
```

Do not compile these programs with a standalone `g++ first.cc` command: they depend on the NS-3 libraries and build configuration. Do not copy all four supplied files into scratch immediately; the old Wi-Fi example can introduce unrelated compatibility errors.

## 5. Question 1 plan

**Goal:** explain how one TCP connection responds to a congested path.

Use exactly the requested three-node topology:

```text
Sender n0 -------- Router n1 -------- Receiver n2
          link A             link B
       10 Mbps, 2 ms       1 Mbps, 10 ms
                         bottleneck
```

Proposed initial settings, which you will record in the report:

| Setting | Initial choice |
|---|---|
| Link A subnet | `10.1.1.0/24` |
| Link B subnet | `10.1.2.0/24` |
| TCP algorithm | Explicitly select `ns3::TcpNewReno` |
| Traffic | `BulkSendHelper` with `ns3::TcpSocketFactory`, `MaxBytes=0` |
| Receiver | TCP `PacketSinkHelper`, port 5000 |
| Segment size | 1448 bytes, explicitly recorded |
| Queue | FIFO/drop-tail behavior, bottleneck capacity initially 20 packets |
| Timing | Sink starts at 0 s; sender starts at 1 s, stops at 30 s; simulation/sink continue to 35 s initially |
| Random link errors | None in the initial experiment |

Implement `lab-q1.cc` in the simulator's `scratch/` directory. Create both links, install the Internet stack, assign separate subnets, populate routes, install the TCP applications, attach traces, run, then export the measurements before destroying the simulator.

**Make queue placement explicit.** A device queue and a traffic-control queue discipline are different buffers. For the simplest teaching experiment, explicitly remove automatic root queue disciplines after address assignment and use the configured device DropTail queues. Set access queues to a documented value, such as 100 packets, and the bottleneck egress queue to 20 packets. Alternatively use an explicit FIFO queue discipline and document the extra device buffer; do not silently combine defaults and claim a single 20-packet queue.

Attach the cwnd callback to the sender's actual TCP socket once it exists. BulkSend creates its socket when it starts, so attaching a socket-list path before application startup can fail. Check the trace connection and save the initial value plus subsequent changes; avoid relying blindly on `SocketList/0`. A direct socket reference after startup is easier to verify.

Run this small comparison after the first successful run:

| Run | Bottleneck rate | Bottleneck queue | Purpose |
|---|---:|---:|---|
| Q1-A | 1 Mbps | 20 packets | Initial congested case |
| Q1-B | 1 Mbps | 100 packets | Queue size versus delay/loss |
| Q1-C | 5 Mbps | 20 packets | Effect of relaxing the bottleneck |

Keep other settings unchanged. Bulk TCP can fill a larger link too, so do not label Q1-C “no congestion” unless the measured drops/queues support that statement.

Save a cwnd graph against simulation time, throughput/goodput over time, and a summary table containing throughput, PDR, mean delay and loss. A bottleneck queue occupancy/drop trace helps explain the cwnd decreases. The expected qualitative pattern is growth followed by reductions when TCP detects loss, but the actual trace determines the conclusion. A few seconds of clean output is not sufficient evidence of congestion.

**Completion check:** exactly three nodes; TCP data crosses the router; the intended link limits capacity; the cwnd trace is nonempty; all requested metrics have stated units and definitions; the loss and queue evidence supports your explanation.

## 6. Question 2 plan

**Goal:** compare NewReno and Cubic fairly while several TCP flows share one bottleneck.

Build `lab-q2.cc` using a simple dumbbell:

```text
S0 ----\                         /---- D0
S1 ----- R0 ==== bottleneck ==== R1 ----- D1
S2 -----/                        \----- D2
S3 ----/                          \---- D3
```

There are four senders, two routers and four receivers. Each sender has one TCP flow to its matching receiver. Give each link its own subnet and identify flows by addresses and ports.

Start with 100 Mbps/2 ms access links, a 20-packet bottleneck queue, four bulk flows, and a 10 ms bottleneck propagation delay. Use the same queue design as Q1. Run senders from approximately 1 s to 61 s, with a small documented start offset between flows; initially end the simulation at 66 s. Use a shared steady measurement window of 11–61 s for goodput and fairness, while recording complete cwnd traces from startup. Validate that the cleanup interval is sufficient; increase it if necessary.

First compare **four NewReno flows against four Cubic flows in separate otherwise identical runs**. A mixed run with two of each is an optional extension about coexistence, not a replacement for the controlled comparison.

| Condition | Bottleneck | Packet error probability | Purpose |
|---|---:|---:|---|
| A | 10 Mbps | 0 | More available capacity |
| B | 2 Mbps | 0 | Stronger bandwidth limitation |
| C | 2 Mbps | 0.001 | Same bottleneck plus 0.1% random packet errors |
| D | 2 Mbps | 0.01 | Same bottleneck plus 1% random packet errors |

Run each condition for both algorithms. For random-error cases use at least five distinct run numbers; the simplest uniform matrix is 2 algorithms × 4 conditions × 5 runs = 40 runs. Deterministic zero-error cases may repeat identically; do not describe those identical repeats as independent random evidence. Record fixed seed and changing run number, and assign random streams explicitly for repeatability.

Set the error model's unit to **packets**. Apply it on the receiving interface of the forward bottleneck direction, document that choice, and leave reverse ACK errors off in the initial study. A bit error probability, byte error probability and packet error probability are not interchangeable. Random corruption loss is not queue congestion, even though both may cause TCP to reduce its window.

Expose parameters through `CommandLine`: algorithm, number of flows, bottleneck rate, queue packets, error probability, duration, run number and output directory. Use unique output directories per run so results are not overwritten.

Produce:

1. Per-flow and aggregate goodput comparison, with variability across stochastic runs.
2. Jain's fairness index for the four forward data flows.
3. Packet loss and mean delay comparisons.
4. Retransmission counts, with an explicit counting method.
5. Per-flow cwnd evolution, showing representative NewReno and Cubic runs on comparable axes.

A higher total rate does not automatically make an algorithm better: consider sharing, delay and retransmission cost too. Cubic's growth law differs from NewReno's, but do not prewrite “Cubic wins.” Explain what the measured experiments show under each condition and acknowledge close or variable results.

## 7. Measure the right quantities

Use separate files for raw observations, summary statistics and plots. Suggested outputs: `cwnd-flow-0.csv`, `goodput-timeseries.csv`, `summary.csv`, `flowmon.xml`, and optional per-device `.pcap` files. These names are proposed outputs, not files already generated.

| Metric | Definition for this lab |
|---|---|
| Application goodput, Mbps | `8 × newly received application bytes / measurement seconds / 1e6`; use PacketSink byte-counter differences over the chosen interval. |
| IP throughput, Mbps | `8 × received IP bytes / measurement seconds / 1e6`, clearly labeled as IP-layer throughput. |
| Packet delivery ratio, % | For a selected forward IP flow: `100 × rxPackets / txPackets`. State the observation period. |
| Mean end-to-end packet delay, ms | For delivered IP packets: `1000 × delaySum(seconds) / rxPackets`. Report N/A if none arrive. |
| Packet loss | Report FlowMonitor's loss estimate after cleanup, plus direct bottleneck drops and error-model drops separately where instrumented. Explain unresolved `txPackets − rxPackets` rather than automatically equating it with congestion. |
| cwnd | Sender congestion window in bytes, or divide by the configured segment size and label the axis in segments. |
| Fairness | `J = (sum(x_i))^2 / (n × sum(x_i^2))`, where `x_i` is each flow's goodput over the same interval. J is 1 for equal positive rates; report N/A if all rates are zero. |
| Retransmissions | Instrument TCP transmissions and count data-bearing segments that overlap previously transmitted sequence-byte ranges, per connection at the sender. Exclude ACK-only/SYN/FIN packets. Count retransmitted bytes too if possible; distinguish segments from bytes. |

FlowMonitor observes IP packets, includes IP headers in its byte totals, and treats a TCP retransmission as another IP packet. It does not directly provide a TCP retransmission counter. Stop traffic before ending the simulation so in-flight data can clear. See the [FlowMonitor documentation](https://www.nsnam.org/docs/models/html/flow-monitor.html).

Filter forward flows using the classifier's protocol, source/destination addresses and ports. Do not average all flow IDs together: reverse ACK traffic is also present. Do not use `flowId > 2` from the old slide example as a general flow filter.

For steady-window throughput, use byte-counter differences at both window boundaries. For whole-run FlowMonitor statistics, label them as whole-run statistics. Do not divide a whole-run byte total by the steady-window duration. A more advanced steady-window delay/PDR measurement requires matching time-filtered packet cohorts and allowing their deliveries to complete.

TCP can retransmit lost packets and eventually deliver all application bytes. Therefore high application reliability can coexist with IP packet loss. Also, delivered-packet delay does not fully capture the time lost waiting for retransmissions and in-order application delivery. Mention this distinction in your analysis.

For PCAP cross-checking, capture at one sender observation point per flow. Counting identical packets in captures from several links would count the same transmission multiple times. Wireshark's retransmission analysis is a useful cross-check, but label it as heuristic if it is your measurement method.

## 8. Work in milestones

| Stage | Approximate effort | Stop condition |
|---|---|---|
| Setup | 30–90 minutes, dependent on downloads/build | `first` runs and enabled tests are checked. |
| Fundamentals | 1–2 hours | Explain first, second and fourth examples in your own words. |
| Q1 | 2–4 hours | Three-node TCP experiment, verified measurements and three comparison runs. |
| Q2 | 3–5 hours | Controlled algorithm comparison and repeatable experiment matrix. |
| Record and viva | 1–2 hours | Graphs, parameter table, conclusions and reproducible commands. |

Spread this across several sessions if needed. The times are planning estimates for a beginner working with assistance; troubleshooting can add time.

For each question your lab record should contain: aim, topology diagram, parameters and NS-3 version, brief algorithm/theory, procedure, source code, exact run command, numerical observations, labeled graphs, analysis, and conclusion. Keep the raw result files as evidence. Include terminal output and graph screenshots where the record asks for screenshots; add an animation only if the teacher requests it.

Be able to answer: Why does the bottleneck queue fill? Why does cwnd decrease? Why can a larger queue increase delay? Why are packet loss and retransmissions different? What does fairness 1 mean? Why must NewReno and Cubic use the same conditions?

## 9. Troubleshooting guide

| Symptom | Likely next check |
|---|---|
| `apt-get: command not found` | You are following a Linux handout on macOS. Use the Mac setup above. |
| `./waf: no such file` | Modern NS-3 uses the `./ns3` launcher; do not combine release instructions. |
| `./ns3: no such file` | Change into the downloaded simulator root, not the teacher's document folder. |
| NS-3 headers cannot be found | Build through NS-3 from its root; do not compile the source file alone. |
| Wi-Fi `Default()`/old helper errors | Use a version-matched example; keep third.cc out of this wired-lab build. |
| No received traffic | Check sink start time, TCP socket factory, destination IP/port, interface addresses and routing. |
| Empty cwnd output | Confirm that the sender socket exists and the callback is attached to it. |
| No observed loss/cwnd reductions | Check offered load, run duration, actual bottleneck queues and any automatic queue discipline. |
| Goodput apparently exceeds link capacity | Check bits versus bytes, Mbps conversion, flow filtering and the averaging interval. Very short bins can show delivery bursts, so examine a sustained interval too. |
| Many packets “lost” at the end | Check cleanup time, queued data, pending TCP transmissions and loss-accounting timeout. |
| Plotting import error | Activate `lab-work/.venv` and check matplotlib is installed in that environment. |

**Your first practical target is just Stage 1: build NS-3 and run `first`.** After that, the handouts become examples you can test and understand, and the two lab programs can be built one piece at a time.
