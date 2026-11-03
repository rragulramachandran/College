# Review of the supplied NS-3 files

The folder originally contains 16 files: nine PDFs totaling 413 pages, two Word documents, four C++ source files, and one short text note. The review covered their text, source code, slide diagrams and code screenshots, including the three embedded Linux screenshots in the Mint document. This is a content and compatibility review; the supplied programs have not been compiled or executed here.

The essential finding is that these files teach pieces of NS-3, but do not supply a complete solution for either attached TCP question. Use [START_HERE_MACBOOK_PLAN.md](START_HERE_MACBOOK_PLAN.md) for the ordered MacBook workflow and experiment design.

## The nine PDFs

**1. [0 NS3 Overview.pdf](<0 NS3 Overview.pdf>) — 45 pages**

Pages 1–12 introduce network simulation, NS-2 versus NS-3 and historical setup. Pages 13–27 describe nodes, devices, channels, applications, helpers, command-line arguments, logging and containers. Pages 28–40 walk through a two-node UDP echo program. Pages 41–45 cover building, sample output, a small hands-on exercise and references.

This is the best conceptual starting point. The core construction sequence remains useful: create nodes, connect interfaces, install the stack, assign addresses, install applications, schedule them, run and destroy the simulation.

Corrections: the Python 2.4, Mercurial, old compiler and Waf setup is historical. The statement suggesting a Linux-only environment does not describe current macOS support. NS-3 is principally C++ with Python interfaces, not a requirement to learn two implementation languages. The build-path example `ns-allinone2.31/ns3/script` is inconsistent; use your chosen release's `scratch/` directory. The UDP server comment mentions port 1500 while the actual code uses 9. For the hands-on question about multiple servers on one node, distinct receiving ports avoid binding conflicts.

**Use now:** concepts and the UDP walkthrough. **Adapt:** all build commands. **Not supplied:** TCP congestion control or measurement code.

**2. [1 ns3 intro.pdf](<1 ns3 intro.pdf>) — 37 pages**

Covers the network hourglass model, discrete-event time, object abstractions, attributes, configuration paths, tracing levels, callbacks, simulator/module organization, Python bindings and another first-example walkthrough. The attribute and trace-source sections are particularly relevant to configuring TCP and recording cwnd.

The material links to NS-3.13 and includes historical interfaces such as `PcapTrace`, old static tracing calls and old configuration examples. Use it to understand the purpose of configuration and callbacks, then consult the chosen release's actual API. `Simulation::Run()` on the slide should be `Simulator::Run()`. “All scenarios should be run under scratch” is a beginner workflow recommendation, not a restriction on what NS-3 can compile or run. The `/scratch/myfirst` example is not the normal relative target you should use.

**Use now:** pages 5–24 as needed after first.cc. **Reference later:** module catalog and binding internals.

**3. [2 Wired Networks in NS3.pdf](<2 Wired Networks in NS3.pdf>) — 13 pages**

Introduces a four-node CSMA LAN, then a point-to-point link attached to a LAN through a shared node. Describes `CsmaHelper`, `Ipv4GlobalRoutingHelper`, multiple interfaces on the router, `PopulateRoutingTables()`, `BulkSendHelper`, and `PacketSinkHelper`. Ends with a multiple-sender exercise sharing an outgoing point-to-point link.

This is the closest conceptual match to your lab. Pages 9–11 explain the application helpers needed to replace UDP echo with continuous TCP traffic. A bulk sender with `MaxBytes=0` can keep offering data until stopped, while TCP controls actual transmission.

The illustrated LAN is not Q1's requested topology. For Q1 use three nodes and two point-to-point links. For Q2 retain the shared-bottleneck idea but add per-flow metrics and algorithm selection. The slides do not supply the full experiment code, congestion-window callback, fair comparison, or complete analysis.

**Use now:** the whole document, especially routing and TCP application helpers.

**4. [3 Trace File Analysis.pdf](<3 Trace File Analysis.pdf>) — 18 pages**

Covers ASCII tracing, queue event symbols, Java TraceMetrics, PCAP generation, tcpdump, Wireshark, RTT graphs, and an exercise varying link rate/delay on a point-to-point-plus-LAN topology. Screenshots illustrate trace fields, TraceMetrics output and Wireshark.

The tracing concept and `EnableAsciiAll`/`EnablePcapAll` examples are useful. However, page 4 incorrectly equates a queue dequeue (`-`) with reception at a node. Correct meanings are enqueue (`+`), dequeue (`-`), drop (`d`) and device receive (`r`). One packet can generate events at several interfaces, so a count of all `+` or `r` lines is not automatically end-to-end PDR.

The Java 7/TraceMetrics setup is an optional historical analysis path. Use explicit NS-3 instrumentation and Python plotting for these labs. An RTT graph is not a congestion-window graph. PCAP captures also do not directly expose the sender's internal cwnd; use the TCP trace source.

**Use now:** tracing and packet-capture sections. **Optional:** Wireshark for inspection. **Defer:** TraceMetrics setup.

**5. [4 wireless Networks.pdf](<4 wireless Networks.pdf>) — 12 pages**

Sets a two-node wireless TCP exercise with a congestion-window trace, then explains Wi-Fi PHY/MAC models, Yans helpers, propagation and mobility. It lists objectives but does not include a complete working TCP program or the requested metrics implementation.

The reference to `IEEE 801.11` is a typo for 802.11. Examples include old `NqosWifiMacHelper`, `WifiHelper::Default()`, `YansWifiPhyHelper::Default()`, old standard enum names, `StaticMobilityModel` and old random-variable string syntax. These require version-specific replacement. Channel defaults should also be checked in the installed release rather than assumed from an old slide.

**Relevance:** cwnd as an objective is related, but neither attached question requires Wi-Fi. Reading and porting this now would add unrelated work.

**6. [5 Wireless Networks2.pdf](<5 Wireless Networks2.pdf>) — 33 pages**

Pages 1–15 cover wireless categories, infrastructure/ad-hoc Wi-Fi, WiMAX and IEEE 802.16 architecture. Pages 16–23 discuss program structure, old Waf commands, animation and WiMAX/LTE/routing examples. Pages 24–30 show Wireshark features and filters. Pages 31–33 discuss adding an example and close the presentation.

Useful background, but mostly outside this wired TCP lab. The claim that Waf compiles only files in scratch is wrong: it also builds the simulator and enabled registered examples/tests. The NetAnim.exe instruction does not describe a native Mac executable. Wscript modifications are for the historical build system; modern NS-3 uses CMake. The final example also names `wimax-simple2` while its source line still points at `wimax-simple.cc`, so it does not clearly build the newly named copy.

**Use later:** optional packet inspection/animation concepts. **Defer now:** wireless standards and wireless simulation setup.

**7. [6 ns3 code.pdf](<6 ns3 code.pdf>) — 41 pages**

Pages 3–14 explain the parts of first.cc. Pages 15–27 describe a mixed Wi-Fi, point-to-point and LAN example with capture output. Page 28 shows a cwnd plot generated from `fifth`, a useful pointer to the missing TCP tracing tutorial. Pages 29–37 show a wireless/MANET-labeled example, including seven code screenshot pages and FlowMonitor output. Pages 38–40 list possible follow-up changes to range, mobility, data rate and other attributes.

The first-example walkthrough is useful. The later screenshot program is not a complete modern lab solution: source line numbers skip sections, it uses old module headers and random-variable/Wi-Fi APIs, and its visible application uses `UdpSocketFactory`. The drawing labels five nodes, but the shown destination lookup is `interface.GetAddress(9)`; it therefore requires a larger topology or correction, and the missing source must not be guessed. The comment about node 0 to node 1 does not agree with that destination lookup.

The FlowMonitor snippet skips `flowId <= 2` under an example-specific assumption about echo traffic. That is not a valid general TCP data-flow filter. It divides received IP bytes by the whole 10-second simulation interval; this is a whole-run rate including idle time, not necessarily the active-transfer or steady-state rate. It also stops the simulator without showing a cleanup period. Adapt the measurement idea, not these assumptions.

**Use now:** first.cc explanation and page 28's pointer to fifth.cc. **Use cautiously:** FlowMonitor structure on pages 35–36. **Do not copy as your solution:** the wireless screenshot code.

**8. [7 ns3TrFileAnalysis.pdf](<7 ns3TrFileAnalysis.pdf>) — 5 pages**

A tutorial-style explanation of trace sources/sinks, ASCII tracing, event symbols, a detailed decoded UDP trace, PCAP generation, tcpdump and Wireshark. It correctly distinguishes dequeue from receive, unlike the simplified statement in the other tracing slides.

It is the most useful short tracing reference in the folder. The long path beginning `/NodeList/.../DeviceList/...` identifies the object and event source; the timestamp is simulation time. File naming encodes node/device identifiers. Its exact sample times come from that example's settings and are not expected outputs for every first.cc variant.

The `./waf` commands are old. Also, an ordinary packet trace is different from a trace of TCP's changing congestion window. You need both kinds of observation only when each serves a clear measurement purpose.

**Use now:** all five pages.

**9. [ns-3-manual.pdf](<ns-3-manual.pdf>) — 209 pages**

The cover says **Release ns-3-dev, January 2, 2020**. It is a simulator/programming manual, not a complete TCP model guide or a lab answer. Its chapters cover the following:

| Chapter | PDF pages | What it contributes to this lab |
|---|---|---|
| 1 Organization | 9–10 | Understand libraries, modules and helper APIs. |
| 2 Git for users | 11–16 | Source management; useful background, not prerequisite reading. |
| 3 Git for maintainers | 17–22 | Release and merge workflow; unnecessary for the lab. |
| 4 Random variables | 23–28 | Important for Q2: fixed seeds, streams, distinct run numbers and reproducibility. |
| 5 Hash functions | 29–32 | Simulator development detail; no lab requirement. |
| 6 Events and simulator | 33–36 | Scheduling, simulation time, startup and stopping. |
| 7 Callbacks | 37–46 | Explains how a trace calls your recording function. |
| 8 Object model | 47–52 | Smart pointers, CreateObject, aggregation and GetObject. |
| 9 Configuration and attributes | 53–72 | Configure link rates, queues, TCP types and object attributes. |
| 10 Object names | 73–74 | Placeholder chapter in this copy. |
| 11 Logging | 75–82 | Diagnose behavior; explains why logs are not a stable analysis interface. |
| 12 Tracing | 83–100 | Trace sources/sinks, cwnd connection paths, ASCII/PCAP helpers and naming. |
| 13 Data collection | 101–128 | Probes, collectors, aggregators, file/gnuplot helpers and limitations. |
| 14 Statistical framework | 129–138 | Repeated trials, metadata, external scripts and a Wi-Fi distance example. |
| 15 Realtime | 139–140 | Synchronizing simulated time with wall time; unnecessary here. |
| 16 Helpers | 141–142 | Why containers and helpers simplify experiment scripts. |
| 17 Gnuplot | 143–150 | Plot files, labels and error bars; Python is an alternative for this plan. |
| 18 Python bindings | 151–158 | Historical PyBindGen/Waf process; not needed for C++ simulations. |
| 19 Tests | 159–176 | Verification versus validation, test.py, test suites and regression checks. |
| 20 Support | 177–206 | Error-model design, new modules, docs, build subsets and troubleshooting. |

Front matter occupies pages 1–8; bibliography/index pages 207–209. Page numbers above are PDF page positions, not the printed page numbers, which have a six-page offset in the numbered main text.

The most useful concepts are configuration, callbacks, tracing, random runs and reproducible measurement. The advanced chapters explain how to extend NS-3 itself; you do not have to master them to assemble these lab topologies.

This manual contains placeholders and old examples even within a 2020 cover date. For example, the queue discussion mixes `MaxSize` with an older `MaxPackets` call; the Python-binding chapter contains a historical Mac data-model claim that should not be used for your arm64 Mac; and the test section repeats a unit-test command where it discusses example tests. Treat code/API/build details as version-bound. Use version-matched documentation and the actual source for the selected release.

## The two Word documents

**10. [Install ns3.docx](<Install ns3.docx>)**

Ubuntu installation guide using NS-3.21, Ubuntu 14.04, Waf, Python-era packages, Qt4, Java 7/8, PyViz, gnuplot, Wireshark and NetAnim. It includes a first-example run and expected console output. The conceptual stages—install dependencies, download, build, test, run—are sound.

The actual package commands are for Linux, not macOS. Several optional packages are presented as though they are necessary for a basic simulation. The typographic dash in `./waf –run first` is not a valid replacement for the command-line `--run`. Its sample sends at 3 seconds, whereas your supplied first.cc starts the client at 2 seconds, so identical timestamps should not be expected.

**Action:** keep as historical context. Use the Mac setup in the new plan instead.

**11. [NS3 installation in Mint 17.docx](<NS3 installation in Mint 17.docx>)**

Linux Mint 17 instructions with Bake and three embedded terminal screenshots showing checks, download and build. The prose recommends 3.20, later command text switches to 3.30.1, and screenshots remain 3.20. The archive is named `.tar.bz2` in one place but `.tar.gz` in the extraction command using the bzip2 `j` option. It also mixes `bake.py` with `./bake` invocations and lists old Qt4/Python/32-bit dependency packages.

These contradictions make it a poor installation recipe even before considering macOS. The screenshots show what success looked like in that old environment, not what you need to reproduce on Apple Silicon.

**Action:** do not execute its commands on this Mac.

## The four C++ programs

**12. [first.cc](first.cc)**

A complete introductory program: two nodes; one 5 Mbps point-to-point link with 2 ms propagation delay; an IPv4 stack; subnet `10.1.1.0/24`; a UDP echo server on node 1/port 9; one 1024-byte UDP payload sent by node 0 starting at 2 seconds; application stop times at 10 seconds. It enables UDP application logging, runs, and cleans up.

There is no TCP connection, router, bottleneck experiment, cwnd trace, FlowMonitor or file tracing in this supplied version. Installing an Internet stack that supports TCP does not make a UDP application use TCP. `Interval=1s` has little practical effect with only one packet.

**Action:** use as the first learning exercise and structural starting point. Add a node and a second link, routing, TCP BulkSend/PacketSink, queue configuration and instrumentation for Q1. Compatibility looks simpler than the wireless example, but compilation has not been tested in this review.

**13. [second.cc](second.cc)**

Extends the tutorial to point-to-point plus a CSMA LAN. Defaults create five distinct nodes: two point-to-point nodes, with the second reused as a LAN member, plus three extra LAN nodes. Point-to-point rate/delay is 5 Mbps/2 ms; CSMA rate/delay is 100 Mbps/6560 ns. It uses separate `10.1.1.0/24` and `10.1.2.0/24` subnets, populates routing, and sends one UDP echo packet from the point-to-point end to the last LAN node.

The `nCsma` argument controls extra LAN nodes, and zero is clamped to one. The `verbose` argument controls application logs. PCAP is enabled for all point-to-point interfaces and one LAN interface. It still has no TCP congestion control or requested metrics.

**Action:** study node reuse, multiple interfaces, addressing and routing. Do not reproduce its LAN when the question specifically asks for point-to-point links.

**14. [third.cc](third.cc)**

Combines a Wi-Fi station group/access point, point-to-point backbone and CSMA LAN. Default topology has eight distinct nodes: two backbone nodes, three additional LAN nodes and three Wi-Fi stations. Uses the previous wired settings plus Wi-Fi, SSID `ns-3-ssid`, AARF rate control, random-walk stations and a stationary access point. One UDP echo packet travels from the last station to the last LAN node. Routing is populated, simulation stops at 10 s, and optional PCAP capture is controlled by `tracing` (default false).

There is a maximum `nWifi` check of 18 based on the position grid. There is no minimum check: `nWifi=0` makes `nWifi - 1` invalid for selecting the sender. The `YansWifiPhyHelper::Default()` call is a known old-API pattern needing comparison with a modern version-matched example. Random mobility also adds a variable unrelated to the requested wired TCP study.

**Action:** defer it. It is a useful later Wi-Fi tutorial, not the next exercise for this lab.

**15. [fourth.cc](fourth.cc)**

Defines `MyObject`, registers a TypeId and trace source called `MyInteger`, stores a `TracedValue<int32_t>`, connects `IntTrace` with `TraceConnectWithoutContext`, and assigns 1234. The callback prints the old and new values. There are no nodes, channels, sockets or event-loop calls because the assignment triggers the callback immediately.

This is directly relevant to understanding cwnd tracing: the lab will observe a TCP socket's existing trace source instead of the custom integer. The expected change is 0 to 1234, not a performance result.

**Action:** read/run after first and second, then compare with the selected release's fifth.cc.

## The text note

**16. [New Text Document.txt](<New Text Document.txt>)**

Three commands with copied student shell prompts: change into a 3.34 tutorial directory, copy first.cc to scratch/mypgm.cc, then use `./ns3 run`. It combines an older Waf-era version with a newer launcher. The directory spelling `ns-allinone.3.34` is unusual, `examples/tutorials` should be checked against `examples/tutorial`, and the initial directory change does not match the root-relative copy command that follows.

**Action:** retain the idea—put a separately named program in scratch and run it from the simulator root—but use the exact modern commands in the Mac plan. Do not paste the prompt text.

## What is missing from the supplied material

There is no full three-node TCP experiment; no complete NewReno/Cubic comparison; no reliable retransmission measurement; no common-window fairness calculation; no parameter sweep runner; and no plotting script or generated lab results. The real TCP tracing tutorial fifth.cc is referenced but absent. The folder is not an NS-3 installation.

The new plan fills the planning gap: what to install, what to read first, what to implement, what settings to compare, how to define the metrics, and what evidence to submit. Actual implementation and experiment execution remain the next stages.
