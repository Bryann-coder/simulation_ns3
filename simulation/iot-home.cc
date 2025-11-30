#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("IoTHomeFinal");

int main(int argc, char *argv[]) {
    uint32_t nCamera = 6;
    uint32_t nThermo = 12;
    uint32_t nLight = 15;
    uint32_t nAssistant = 3;
    uint32_t nTV = 2;
    uint32_t nLaptop = 4;
    double duration = 120.0;
    
    CommandLine cmd;
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    // 1. Topologie
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nCamera + nThermo + nLight + nAssistant + nTV + nLaptop);
    NodeContainer wifiApNode; wifiApNode.Create(1);
    NodeContainer serverNodes; serverNodes.Create(6);

    // 2. WiFi 802.11ac (Rapide mais sensible)
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ac);
    wifi.SetRemoteStationManager("ns3::IdealWifiManager"); 

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    YansWifiPhyHelper phy; 
    phy.SetChannel(channel.Create());
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));

    WifiMacHelper mac;
    Ssid ssid = Ssid("SmartHome_AI");
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevices = wifi.Install(phy, mac, wifiApNode);
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(true));
    NetDeviceContainer staDevices = wifi.Install(phy, mac, wifiStaNodes);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodes);
    mobility.Install(wifiApNode);

    // 3. Filaire CSMA
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("1Gbps"));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    NodeContainer csmaSwitch;
    csmaSwitch.Add(wifiApNode.Get(0));
    csmaSwitch.Add(serverNodes);
    NetDeviceContainer csmaDevices = csma.Install(csmaSwitch);

    // 4. IP
    InternetStackHelper stack; stack.InstallAll();
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer serverInterfaces = address.Assign(csmaDevices);
    address.SetBase("192.168.1.0", "255.255.255.0");
    address.Assign(apDevices); address.Assign(staDevices);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // 5. Applications
    Ptr<UniformRandomVariable> rnd = CreateObject<UniformRandomVariable>();
    uint32_t idx = 0;

    // Camera (UDP)
    for(uint32_t i=0; i<nCamera; i++) {
        uint16_t port = 1000+i;
        OnOffHelper c("ns3::UdpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(1), port));
        c.SetAttribute("DataRate", StringValue("1Mbps"));
        c.SetAttribute("PacketSize", UintegerValue(1000));
        c.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
        c.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
        ApplicationContainer app = c.Install(wifiStaNodes.Get(idx++));
        app.Start(Seconds(rnd->GetValue(0,2))); app.Stop(Seconds(duration));
        PacketSinkHelper s("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        s.Install(serverNodes.Get(0)).Start(Seconds(0.0));
    }
    // Thermo (UDP)
    for(uint32_t i=0; i<nThermo; i++) {
        uint16_t port = 2000+i;
        OnOffHelper c("ns3::UdpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(2), port));
        c.SetAttribute("DataRate", StringValue("1kbps"));
        c.SetAttribute("PacketSize", UintegerValue(50));
        c.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));
        c.SetAttribute("OffTime", StringValue("ns3::UniformRandomVariable[Min=20|Max=60]"));
        ApplicationContainer app = c.Install(wifiStaNodes.Get(idx++));
        app.Start(Seconds(rnd->GetValue(0,5))); app.Stop(Seconds(duration));
        PacketSinkHelper s("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        s.Install(serverNodes.Get(1)).Start(Seconds(0.0));
    }
    // Light (UDP)
    for(uint32_t i=0; i<nLight; i++) {
        uint16_t port = 3000+i;
        OnOffHelper c("ns3::UdpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(3), port));
        c.SetAttribute("DataRate", StringValue("1kbps"));
        c.SetAttribute("PacketSize", UintegerValue(50));
        c.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.1]"));
        c.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=120]"));
        ApplicationContainer app = c.Install(wifiStaNodes.Get(idx++));
        app.Start(Seconds(rnd->GetValue(0,5))); app.Stop(Seconds(duration));
        PacketSinkHelper s("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        s.Install(serverNodes.Get(2)).Start(Seconds(0.0));
    }
    // Assistant (UDP)
    for(uint32_t i=0; i<nAssistant; i++) {
        uint16_t port = 4000+i;
        OnOffHelper c("ns3::UdpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(4), port));
        c.SetAttribute("DataRate", StringValue("128kbps"));
        c.SetAttribute("PacketSize", UintegerValue(200));
        c.SetAttribute("OnTime", StringValue("ns3::UniformRandomVariable[Min=2|Max=5]"));
        c.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=60]"));
        ApplicationContainer app = c.Install(wifiStaNodes.Get(idx++));
        app.Start(Seconds(rnd->GetValue(2,8))); app.Stop(Seconds(duration));
        PacketSinkHelper s("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        s.Install(serverNodes.Get(3)).Start(Seconds(0.0));
    }

    // --- LE FACTEUR DOMINANT ---
    // TV (TCP Bulk - Illimité)
    for(uint32_t i=0; i<nTV; i++) {
        uint16_t port = 5000+i;
        BulkSendHelper c("ns3::TcpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(5), port));
        c.SetAttribute("MaxBytes", UintegerValue(0));
        c.SetAttribute("SendSize", UintegerValue(1500)); // Gros paquets
        ApplicationContainer app = c.Install(wifiStaNodes.Get(idx++));
        app.Start(Seconds(1.0)); // Démarre tôt !
        app.Stop(Seconds(duration));
        PacketSinkHelper s("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        s.Install(serverNodes.Get(4)).Start(Seconds(0.0));
    }
    // Laptop (TCP Bulk - Illimité)
    // C'est eux qui vont inonder le réseau et créer les 1600+ échantillons
    for(uint32_t i=0; i<nLaptop; i++) {
        uint16_t port = 6000+i;
        BulkSendHelper c("ns3::TcpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(6), port));
        c.SetAttribute("MaxBytes", UintegerValue(0));
        c.SetAttribute("SendSize", UintegerValue(1500));
        ApplicationContainer app = c.Install(wifiStaNodes.Get(idx++));
        app.Start(Seconds(2.0)); // Démarre tôt !
        app.Stop(Seconds(duration));
        PacketSinkHelper s("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        s.Install(serverNodes.Get(5)).Start(Seconds(0.0));
    }

    // 6. Trace
    csma.EnablePcap("iot-wired", csmaDevices.Get(0), true);

    Simulator::Stop(Seconds(duration+1));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}