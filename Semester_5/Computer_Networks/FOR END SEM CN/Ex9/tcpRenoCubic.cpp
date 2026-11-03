#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/error-model.h"

#include <fstream>
#include <iomanip>

using namespace ns3;

NodeContainer nodes;
NetDeviceContainer accessDevices;
NetDeviceContainer bottleneckDevices;
Ptr<FlowMonitor> monitor;

// CUSTOMIZATION: change these values or override them using command-line arguments.
uint32_t numberOfNodes = 4;            // Minimum 4 for the access + bottleneck topology below.
double simulationTime = 10.0;          // Total simulation time in seconds.
std::string tcpVariant = "Reno";        // Use "Reno" or "Cubic".
std::string accessRate = "10Mbps";      // Data rate of the access link.
std::string bottleneckRate = "1Mbps";   // Data rate of the bottleneck link.
std::string accessDelay = "2ms";        // Delay of the access link.
std::string bottleneckDelay = "10ms";   // Delay of the bottleneck link.
double packetErrorRate = 0.05;          // Packet error probability; use 0.0 to disable.
std::string outputFile = "tcp_results.csv";

void NodeCreation()
{
    // CUSTOMIZATION: numberOfNodes controls how many nodes are created.
    // This simple topology uses nodes 0, 1, 2, and 3; extra nodes are created
    // but are not connected unless LinkCreation/CongestionMaking are extended.
    nodes.Create(numberOfNodes);

    InternetStackHelper stack;
    stack.Install(nodes);
}

void LinkCreation()
{
    PointToPointHelper p2p;

    // CUSTOMIZATION: access-link bandwidth and delay.
    p2p.SetDeviceAttribute("DataRate", StringValue(accessRate));
    p2p.SetChannelAttribute("Delay", StringValue(accessDelay));

    accessDevices = p2p.Install(nodes.Get(0), nodes.Get(1));
}

void CongestionMaking()
{
    PointToPointHelper bottleneck;

    // CUSTOMIZATION: bottleneck bandwidth and delay.
    bottleneck.SetDeviceAttribute("DataRate", StringValue(bottleneckRate));
    bottleneck.SetChannelAttribute("Delay", StringValue(bottleneckDelay));

    bottleneckDevices = bottleneck.Install(nodes.Get(2), nodes.Get(3));
}

void ErrorModeling()
{
    // CUSTOMIZATION: packetErrorRate = 0 disables packet errors.
    if (packetErrorRate <= 0.0)
    {
        return;
    }

    Ptr<RateErrorModel> error = CreateObject<RateErrorModel>();
    error->SetAttribute("ErrorRate", DoubleValue(packetErrorRate));
    error->SetAttribute("ErrorUnit",
                        EnumValue(RateErrorModel::ERROR_UNIT_PACKET));

    bottleneckDevices.Get(1)->SetAttribute(
        "ReceiveErrorModel", PointerValue(error));
}

void OutputSummary()
{
    FlowMonitorHelper flowmon;
    monitor = flowmon.InstallAll();

    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();

    monitor->CheckForLostPackets();

    uint64_t txPackets = 0;
    uint64_t rxPackets = 0;
    uint64_t lostPackets = 0;
    uint64_t rxBytes = 0;
    double delaySum = 0.0;

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = stats.begin();
         it != stats.end(); ++it)
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(it->first);

        // Include TCP flows only. For the original two separate links, no end-to-end
        // TCP flow exists yet; the traffic setup in main() creates one below.
        if (tuple.protocol == 6)
        {
            txPackets += it->second.txPackets;
            rxPackets += it->second.rxPackets;
            lostPackets += it->second.lostPackets;
            rxBytes += it->second.rxBytes;
            delaySum += it->second.delaySum.GetSeconds();
        }
    }

    double activeTime = simulationTime - 1.0;
    if (activeTime <= 0.0)
    {
        activeTime = simulationTime;
    }

    double throughputMbps = (rxBytes * 8.0) / (activeTime * 1000000.0);
    double meanDelayMs = (rxPackets > 0) ? (delaySum * 1000.0 / rxPackets) : 0.0;
    double lossPercent = (txPackets > 0) ? (100.0 * lostPackets / txPackets) : 0.0;

    std::ofstream out(outputFile.c_str());
    out << "tcp,txPackets,rxPackets,lostPackets,rxBytes,throughputMbps,meanDelayMs,lossPercent\n";
    out << tcpVariant << "," << txPackets << "," << rxPackets << ","
        << lostPackets << "," << rxBytes << "," << std::fixed
        << std::setprecision(6) << throughputMbps << "," << meanDelayMs
        << "," << lossPercent << "\n";
    out.close();

    std::cout << "\n--- TCP " << tcpVariant << " Results ---\n"
              << "Transmitted packets: " << txPackets << "\n"
              << "Received packets: " << rxPackets << "\n"
              << "Lost packets: " << lostPackets << "\n"
              << "Throughput: " << throughputMbps << " Mbps\n"
              << "Mean delay: " << meanDelayMs << " ms\n"
              << "Packet loss: " << lossPercent << " %\n"
              << "CSV file: " << outputFile << "\n";

    monitor->SerializeToXmlFile("flowmon-" + tcpVariant + ".xml", true, true);
    Simulator::Destroy();
}

int main(int argc, char *argv[])
{
    // Command-line customizations; function names and overall structure are retained.
    CommandLine cmd(__FILE__);
    cmd.AddValue("nodes", "Number of nodes (minimum 4 for this topology)", numberOfNodes);
    cmd.AddValue("time", "Simulation duration in seconds", simulationTime);
    cmd.AddValue("tcp", "TCP variant: Reno or Cubic", tcpVariant);
    cmd.AddValue("accessRate", "Access link data rate", accessRate);
    cmd.AddValue("bottleneckRate", "Bottleneck data rate", bottleneckRate);
    cmd.AddValue("accessDelay", "Access link delay", accessDelay);
    cmd.AddValue("bottleneckDelay", "Bottleneck link delay", bottleneckDelay);
    cmd.AddValue("errorRate", "Packet error rate between 0.0 and 1.0", packetErrorRate);
    cmd.AddValue("output", "CSV output file name", outputFile);
    cmd.Parse(argc, argv);

    if (numberOfNodes < 4 || simulationTime <= 1.0 ||
        packetErrorRate < 0.0 || packetErrorRate > 1.0)
    {
        NS_FATAL_ERROR("Use nodes >= 4, time > 1, and errorRate between 0 and 1.");
    }

    // Configure TCP before sockets are created.
    if (tcpVariant == "Cubic" || tcpVariant == "cubic")
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                           TypeIdValue(TypeId::LookupByName("ns3::TcpCubic")));
        tcpVariant = "Cubic";
    }
    else if (tcpVariant == "Reno" || tcpVariant == "reno")
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                           TypeIdValue(TypeId::LookupByName("ns3::TcpReno")));
        tcpVariant = "Reno";
    }
    else
    {
        NS_FATAL_ERROR("Invalid TCP variant. Choose Reno or Cubic.");
    }

    NodeCreation();
    LinkCreation();
    CongestionMaking();
    ErrorModeling();

    // Connect node 1 to node 2 so the access link and bottleneck form one path.
    // Nodes 4 onward remain unused in this minimal version, preserving the original
    // function layout. Use --nodes=4 unless you extend the topology.
    PointToPointHelper middleLink;
    middleLink.SetDeviceAttribute("DataRate", StringValue(accessRate));
    middleLink.SetChannelAttribute("Delay", StringValue(accessDelay));
    NetDeviceContainer middleDevices = middleLink.Install(nodes.Get(1), nodes.Get(2));

    InternetStackHelper unusedStack; // Stack is already installed by NodeCreation().
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer accessIf, middleIf, bottleneckIf;

    address.SetBase("10.1.1.0", "255.255.255.0");
    accessIf = address.Assign(accessDevices);
    address.SetBase("10.1.2.0", "255.255.255.0");
    middleIf = address.Assign(middleDevices);
    address.SetBase("10.1.3.0", "255.255.255.0");
    bottleneckIf = address.Assign(bottleneckDevices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // TCP receiver on node 3, sender on node 0.
    uint16_t port = 5000;
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                          InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(nodes.Get(3));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simulationTime));

    BulkSendHelper source("ns3::TcpSocketFactory",
                          InetSocketAddress(bottleneckIf.GetAddress(1), port));
    source.SetAttribute("MaxBytes", UintegerValue(0)); // 0 means unlimited data.
    source.SetAttribute("SendSize", UintegerValue(1024));
    ApplicationContainer sourceApp = source.Install(nodes.Get(0));
    sourceApp.Start(Seconds(1.0));
    sourceApp.Stop(Seconds(simulationTime - 0.1));

    OutputSummary();
    return 0;
}
