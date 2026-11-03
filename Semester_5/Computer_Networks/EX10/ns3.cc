#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include <iostream>
#include <iomanip>
#include <string>

using namespace ns3;
using namespace std;

int main()
{
    // Node creation
    NodeContainer nodes;
    nodes.Create(2);

    // Link creation
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer devices;
    devices = p2p.Install(nodes);

    // Internet stack
    InternetStackHelper stack;
    stack.Install(nodes);

    // IP address
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces;
    interfaces = address.Assign(devices);

    // TCP Receiver
    uint16_t port = 5000;
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(nodes.Get(1));
    sinkApp.Start(Seconds(1.0));
    sinkApp.Stop(Seconds(10.0));

    // TCP Sender
    OnOffHelper source("ns3::TcpSocketFactory", InetSocketAddress(interfaces.GetAddress(1), port));
    source.SetAttribute("DataRate", StringValue("5Mbps"));
    source.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer sourceApp = source.Install(nodes.Get(0));
    sourceApp.Start(Seconds(2.0));
    sourceApp.Stop(Seconds(9.0));

    // Configuration summary
    const string separator(64, '-');
    const string border(64, '=');
    cout << left;
    cout << border << endl;
    cout << "              TCP CONGESTION CONTROL SIMULATION" << endl;
    cout << border << endl;
    cout << "\nNETWORK CONFIGURATION" << endl;
    cout << setw(20) << "Sender IP" << ": " << interfaces.GetAddress(0) << endl;
    cout << setw(20) << "Receiver IP" << ": " << interfaces.GetAddress(1) << endl;
    cout << setw(20) << "Link bandwidth" << ": 2 Mbps" << endl;
    cout << setw(20) << "Link delay" << ": 10 ms" << endl;
    cout << setw(20) << "Sender on-state rate" << ": 5 Mbps" << endl;
    cout << setw(20) << "Simulation time" << ": 10 seconds" << endl;

    // Manual buffer demonstration, independent of the ns-3 TCP simulation
    int bufferSize = 5;
    int buffer = 0;
    cout << "\nBUFFER DEMONSTRATION" << endl;
    cout << "Manual example; not measured TCP queue activity." << endl;
    cout << separator << endl;
    cout << setw(9) << "Packet"
         << setw(16) << "Buffer Before"
         << setw(16) << "Buffer After"
         << "Status" << endl;
    cout << separator << endl;

    for (int i = 1; i <= 12; i++)
    {
        int bufferBefore = buffer;
        string status;
        if (buffer < bufferSize)
        {
            buffer++;
            status = "Added";
        }
        else
        {
            buffer = buffer - 2;
            status = "Full; drained 2";
        }
        cout << setw(9) << i
             << setw(16) << (to_string(bufferBefore) + "/" + to_string(bufferSize))
             << setw(16) << (to_string(buffer) + "/" + to_string(bufferSize))
             << status << endl;
    }
    cout << separator << endl;

    cout << "\nNS-3 SIMULATION" << endl;
    cout << setw(20) << "Status" << ": Running..." << endl;
    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Ptr<PacketSink> receiver = DynamicCast<PacketSink>(sinkApp.Get(0));
    cout << setw(20) << "Status" << ": Completed" << endl;
    cout << setw(20) << "Total bytes received" << ": " << receiver->GetTotalRx() << " bytes" << endl;
    cout << border << endl;

    Simulator::Destroy();
    return 0;
}
