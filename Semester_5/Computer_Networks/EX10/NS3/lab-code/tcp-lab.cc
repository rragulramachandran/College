// TCP congestion-control lab. Target: ns-3.46.1, C++.
// Q1: sender -- router -- receiver. Q2: N senders -- R0 -- R1 -- N receivers.
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace ns3;
static std::ofstream cwndFile, rateFile, eventFile;
static std::vector<Ptr<PacketSink>> sinks;
static std::vector<uint64_t> previousBytes, windowBytes, retransmissions;
static uint64_t queueDrops = 0, errorDrops = 0;
static double measureStart, senderStop;

static void Cwnd(uint32_t flow, uint32_t oldValue, uint32_t newValue)
{
    cwndFile << Simulator::Now().GetSeconds() << ',' << flow << ','
             << oldValue << ',' << newValue << '\n';
}

static void Retransmit(uint32_t flow, Ptr<const Packet> packet, const TcpHeader& header,
                       const Address&, const Address&, Ptr<const TcpSocketBase>)
{
    if (packet->GetSize() == 0) return; // Count retransmitted DATA segments only.
    ++retransmissions.at(flow);
    eventFile << Simulator::Now().GetSeconds() << ",retransmission," << flow << ','
              << packet->GetSize() << ',' << header.GetSequenceNumber().GetValue() << '\n';
}

static void Attach(uint32_t flow, Ptr<BulkSendApplication> app)
{
    // BulkSend creates its socket at StartApplication. Attach 1 ns later,
    // before the first handshake reply/data can traverse our millisecond links.
    auto socket = app->GetSocket();
    NS_ABORT_MSG_IF(!socket, "BulkSend socket not created");
    bool a = socket->TraceConnectWithoutContext("CongestionWindow",
                                                MakeBoundCallback(&Cwnd, flow));
    bool b = socket->TraceConnectWithoutContext("Retransmission",
                                                MakeBoundCallback(&Retransmit, flow));
    NS_ABORT_MSG_IF(!a || !b, "Required TCP traces unavailable; use ns-3.46.1");
}

static void Drop(bool isError, Ptr<const Packet> packet)
{
    if (isError) ++errorDrops; else ++queueDrops;
    eventFile << Simulator::Now().GetSeconds() << ','
              << (isError ? "link_error" : "queue_drop") << ",-1,"
              << packet->GetSize() << ",0\n";
}

static void Receive(uint32_t flow, Ptr<const Packet> packet, const Address&)
{
    const double t = Simulator::Now().GetSeconds();
    if (t >= measureStart && t < senderStop) windowBytes.at(flow) += packet->GetSize();
}

static void Sample()
{
    const double t = Simulator::Now().GetSeconds();
    for (uint32_t i = 0; i < sinks.size(); ++i)
    {
        auto bytes = sinks[i]->GetTotalRx();
        rateFile << t << ',' << i << ',' << (bytes - previousBytes[i]) * 8.0 / 1e6 << '\n';
        previousBytes[i] = bytes;
    }
    if (t + 1 <= senderStop) Simulator::Schedule(Seconds(1), &Sample);
}

int main(int argc, char* argv[])
{
    uint32_t question = 1, flows = 4, queuePackets = 20;
    uint64_t run = 1;
    double bottleneckMbps = 1, errorRate = 0, duration = 30, warmup = 0, cleanup = 15;
    std::string tcp = "NewReno", output = "tcp-results";
    CommandLine cmd(__FILE__);
    cmd.AddValue("question", "1: three nodes; 2: multi-flow dumbbell", question);
    cmd.AddValue("flows", "Number of flows for Q2 (1..50)", flows);
    cmd.AddValue("tcp", "NewReno or Cubic", tcp);
    cmd.AddValue("bottleneckMbps", "Bottleneck link rate in Mbps", bottleneckMbps);
    cmd.AddValue("queuePackets", "DropTail device queue capacity in packets", queuePackets);
    cmd.AddValue("errorRate", "Forward bottleneck packet error probability", errorRate);
    cmd.AddValue("duration", "Sender stop time in seconds (starts at 1 s)", duration);
    cmd.AddValue("warmup", "Seconds excluded after 1 s from goodput/fairness", warmup);
    cmd.AddValue("cleanup", "Drain time after sender stops; at least 11 seconds", cleanup);
    cmd.AddValue("run", "Random run number; fixed seed 12345", run);
    cmd.AddValue("output", "New directory for this run (must not exist)", output);
    cmd.Parse(argc, argv);
    NS_ABORT_MSG_IF(question != 1 && question != 2, "question must be 1 or 2");
    NS_ABORT_MSG_IF(tcp != "NewReno" && tcp != "Cubic", "tcp must be NewReno or Cubic");
    NS_ABORT_MSG_IF(flows < 1 || flows > 50 || queuePackets < 1 || bottleneckMbps <= 0 ||
                    errorRate < 0 || errorRate >= 1 || warmup < 0 || duration <= 1 + warmup ||
                    cleanup < 11 || run < 1 ||
                    (question == 2 && duration <= 1 + (flows - 1) * 0.001), "Invalid parameter range");
    if (question == 1) flows = 1;
    NS_ABORT_MSG_IF(std::filesystem::exists(output), "Output directory already exists: " << output);
    std::filesystem::create_directories(output);
    measureStart = 1 + warmup;
    senderStop = duration;
    RngSeedManager::SetSeed(12345);
    RngSeedManager::SetRun(run);
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName("ns3::Tcp" + tcp)));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(128 * 1024));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(1 << 20));

    cwndFile.open(output + "/cwnd.csv");
    rateFile.open(output + "/goodput.csv");
    eventFile.open(output + "/events.csv");
    cwndFile << "time_s,flow,old_cwnd_bytes,cwnd_bytes\n";
    rateFile << "time_s,flow,goodput_mbps\n";
    eventFile << "time_s,event,flow,bytes,sequence\n";
    NS_ABORT_MSG_IF(!cwndFile || !rateFile || !eventFile, "Cannot open output files");

    NodeContainer senders, receivers, routers, all;
    senders.Create(flows);
    receivers.Create(flows);
    routers.Create(question == 1 ? 1 : 2);
    all.Add(senders); all.Add(routers); all.Add(receivers);
    InternetStackHelper internet;
    internet.Install(all);
    PointToPointHelper access, bottleneck;
    // Without a root qdisc, disable device flow control too: otherwise the
    // traffic-control layer can drop before the device queue's Drop trace.
    access.DisableFlowControl();
    bottleneck.DisableFlowControl();
    access.SetDeviceAttribute("DataRate", StringValue(question == 1 ? "10Mbps" : "100Mbps"));
    access.SetChannelAttribute("Delay", StringValue("2ms"));
    access.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize", StringValue("100p"));
    bottleneck.SetDeviceAttribute("DataRate", DataRateValue(DataRate(uint64_t(bottleneckMbps * 1e6))));
    bottleneck.SetChannelAttribute("Delay", StringValue("10ms"));
    bottleneck.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize",
                        StringValue(std::to_string(queuePackets) + "p"));
    Ipv4AddressHelper addresses;
    addresses.SetBase("10.1.1.0", "255.255.255.0");
    auto assign = [&](NetDeviceContainer devices) {
        auto interfaces = addresses.Assign(devices);
        addresses.NewNetwork();
        // Assign() may install a default root qdisc. Remove it so the configured
        // DropTail device queue is the actual, single queue at each link output.
        for (auto it = devices.Begin(); it != devices.End(); ++it)
        {
            auto layer = (*it)->GetNode()->GetObject<TrafficControlLayer>();
            if (layer->GetRootQueueDiscOnDevice(*it)) layer->DeleteRootQueueDiscOnDevice(*it);
        }
        return interfaces;
    };
    std::vector<Ipv4Address> sourceIps, destinationIps;
    for (uint32_t i = 0; i < flows; ++i)
    {
        auto interfaces = assign(access.Install(senders.Get(i), routers.Get(0)));
        sourceIps.push_back(interfaces.GetAddress(0));
    }
    auto bottleneckDevices = bottleneck.Install(routers.Get(0),
                            question == 1 ? receivers.Get(0) : routers.Get(1));
    auto middleIps = assign(bottleneckDevices);
    if (question == 1) destinationIps.push_back(middleIps.GetAddress(1));
    else for (uint32_t i = 0; i < flows; ++i)
    {
        auto interfaces = assign(access.Install(routers.Get(1), receivers.Get(i)));
        destinationIps.push_back(interfaces.GetAddress(1));
    }
    auto forwardDevice = DynamicCast<PointToPointNetDevice>(bottleneckDevices.Get(0));
    forwardDevice->GetQueue()->TraceConnectWithoutContext("Drop", MakeBoundCallback(&Drop, false));
    auto receiveDevice = DynamicCast<PointToPointNetDevice>(bottleneckDevices.Get(1));
    auto errors = CreateObject<RateErrorModel>();
    errors->SetUnit(RateErrorModel::ERROR_UNIT_PACKET);
    errors->SetRate(errorRate);
    errors->AssignStreams(10);
    receiveDevice->SetReceiveErrorModel(errors);
    receiveDevice->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&Drop, true));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    previousBytes.resize(flows); windowBytes.resize(flows); retransmissions.resize(flows);
    for (uint32_t i = 0; i < flows; ++i)
    {
        PacketSinkHelper sink("ns3::TcpSocketFactory",
                               InetSocketAddress(Ipv4Address::GetAny(), 5000));
        auto sinkApps = sink.Install(receivers.Get(i));
        sinkApps.Start(Seconds(0)); // Keep sinks open throughout drain period.
        auto sinkApp = DynamicCast<PacketSink>(sinkApps.Get(0));
        sinks.push_back(sinkApp);
        sinkApp->TraceConnectWithoutContext("Rx", MakeBoundCallback(&Receive, i));
        BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(destinationIps[i], 5000));
        source.SetAttribute("MaxBytes", UintegerValue(0)); // Saturated file transfer.
        source.SetAttribute("SendSize", UintegerValue(1448));
        auto sourceApps = source.Install(senders.Get(i));
        double start = 1.0 + i * 0.001;
        sourceApps.Start(Seconds(start));
        sourceApps.Stop(Seconds(duration));
        Simulator::Schedule(Seconds(start) + NanoSeconds(1), &Attach, i,
                            DynamicCast<BulkSendApplication>(sourceApps.Get(0)));
    }
    FlowMonitorHelper flowHelper;
    auto monitor = flowHelper.InstallAll();
    Simulator::Schedule(Seconds(1), &Sample);
    Simulator::Stop(Seconds(duration + cleanup));
    Simulator::Run();
    monitor->CheckForLostPackets();
    auto classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::vector<FlowMonitor::FlowStats> stats(flows);
    for (const auto& [id, stat] : monitor->GetFlowStats())
    {
        auto tuple = classifier->FindFlow(id);
        for (uint32_t i = 0; i < flows; ++i)
            if (tuple.protocol == 6 && tuple.sourceAddress == sourceIps[i] &&
                tuple.destinationAddress == destinationIps[i] && tuple.destinationPort == 5000)
                stats[i] = stat; // Only forward TCP; exclude reverse ACK flow.
    }
    std::ofstream summary(output + "/summary.csv");
    summary << "flow,tcp,goodput_mbps,ip_throughput_mbps,tx_packets,rx_packets,pdr_percent,"
               "mean_delay_ms,lost_packets,tx_minus_rx,retransmitted_data_segments,total_app_rx_bytes\n";
    double sum = 0, squares = 0;
    for (uint32_t i = 0; i < flows; ++i)
    {
        auto s = stats[i];
        double goodput = windowBytes[i] * 8.0 / (duration - measureStart) / 1e6;
        sum += goodput; squares += goodput * goodput;
        summary << i << ',' << tcp << ',' << goodput << ','
                << s.rxBytes * 8.0 / (duration + cleanup - 1) / 1e6 << ','
                << s.txPackets << ',' << s.rxPackets << ',';
        if (s.txPackets) summary << 100.0 * s.rxPackets / s.txPackets; else summary << "nan";
        summary << ',';
        if (s.rxPackets) summary << 1000 * s.delaySum.GetSeconds() / s.rxPackets; else summary << "nan";
        summary << ',' << s.lostPackets << ',' << s.txPackets - s.rxPackets << ','
                << retransmissions[i] << ',' << sinks[i]->GetTotalRx() << '\n';
    }
    double fairness = squares > 0 ? sum * sum / (flows * squares) : NAN;
    std::ofstream meta(output + "/run.csv");
    meta << "question,tcp,flows,bottleneck_mbps,queue_packets,error_rate,run,"
            "measure_start_s,measure_end_s,simulation_end_s,aggregate_goodput_mbps,jain_fairness,"
            "forward_queue_drops,forward_error_drops\n";
    meta << question << ',' << tcp << ',' << flows << ',' << bottleneckMbps << ',' << queuePackets
         << ',' << errorRate << ',' << run << ',' << measureStart << ',' << duration << ','
         << duration + cleanup << ',' << sum << ',' << fairness << ',' << queueDrops << ','
         << errorDrops << '\n';
    monitor->SerializeToXmlFile(output + "/flow-monitor.xml", true, true);
    std::cout << "Saved " << output << "\nAggregate application goodput: " << sum
              << " Mbps\nJain fairness: " << fairness << "\nForward queue drops: " << queueDrops
              << "; link error drops: " << errorDrops << '\n';
    Simulator::Destroy();
}
