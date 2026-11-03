#!/usr/bin/env python3
"""Run either question from a built ns-3.46.1 checkout. No shell interpolation."""
import argparse
from pathlib import Path
import shutil
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("question", choices=["q1", "q2"])
p.add_argument("--ns3", required=True, type=Path, help="ns-3.46.1 source directory")
p.add_argument("--output", type=Path, default=Path("results"))
p.add_argument("--runs", type=int, default=5, help="Repeats per Q2 condition; default 5")
a = p.parse_args()
root, out = a.ns3.expanduser().resolve(), a.output.expanduser().resolve()
if not (root / "ns3").is_file():
    p.error("--ns3 must point to the directory containing the ns3 launcher")
if a.runs < 1:
    p.error("--runs must be positive")
target = root / "scratch" / "tcp-lab.cc"
source = Path(__file__).with_name("tcp-lab.cc")
if target.exists() and target.read_bytes() != source.read_bytes():
    p.error(f"{target} differs from this source; review it before replacing it")
shutil.copy2(source, target)
subprocess.run([str(root / "ns3"), "build", "tcp-lab"], cwd=root, check=True)
cases = []
if a.question == "q1":
    for name, bw, queue in [("baseline", 1, 20), ("large-queue", 1, 100), ("faster-link", 5, 20)]:
        cases.append((f"q1-{name}", ["--question=1", "--tcp=NewReno", f"--bottleneckMbps={bw}",
                                   f"--queuePackets={queue}", "--duration=30", "--warmup=0"]))
else:
    for name, bw, error in [("wide", 10, 0), ("narrow", 2, 0),
                            ("error-low", 2, 0.001), ("error-high", 2, 0.01)]:
        for tcp in ["NewReno", "Cubic"]:
            for run in range(1, a.runs + 1):
                cases.append((f"q2-{name}-{tcp}-r{run}", ["--question=2", "--flows=4", f"--tcp={tcp}",
                              f"--bottleneckMbps={bw}", f"--errorRate={error}", "--queuePackets=20",
                              "--duration=61", "--warmup=10", f"--run={run}"]))
for name, _ in cases:
    if (out / name).exists():
        p.error(f"{out / name} already exists. Use a fresh --output directory.")
for index, (name, options) in enumerate(cases, 1):
    print(f"[{index}/{len(cases)}] {name}", flush=True)
    subprocess.run([str(root / "ns3"), "run", "--no-build", "tcp-lab", "--",
                    *options, f"--output={out / name}"], cwd=root, check=True)
print(f"Done. Plot with: python3 {Path(__file__).with_name('plot_lab.py')} {out}")
