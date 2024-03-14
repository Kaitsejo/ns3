#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiNetwork");

int main(int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Create nodes
  NodeContainer stations;
  stations.Create(2);
  NodeContainer ap;
  ap.Create(1);

  // Install mobility model
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(stations);
  mobility.Install(ap);

  // Install Wifi module

  WifiHelper wifi;
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy;
  WifiMacHelper mac;
  Ssid ssid = Ssid("Mason");
  mac.SetType("ns3::StaWifiMac",
              "Ssid", SsidValue(ssid),
              "ActiveProbing", BooleanValue(false));
  NetDeviceContainer staDevices = wifi.Install(phy, mac, stations);
  mac.SetType("ns3::ApWifiMac",
              "Ssid", SsidValue(ssid));
  NetDeviceContainer apDevices = wifi.Install(phy, mac, ap);

  // Install Internet Stack
  InternetStackHelper stack;
  stack.Install (stations);
  stack.Install(ap);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces = address.Assign(staDevices);

  // Create and install UDP echo server on access point
  UdpEchoServerHelper echoServer(9);
  ApplicationContainer serverApps = echoServer.Install(ap);
  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));

  // Create and install UDP echo client on station 1
  UdpEchoClientHelper echoClient(staInterfaces.GetAddress(0), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps = echoClient.Install(stations.Get(0));
  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  // Enable logging
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Enable PCAP tracing
  phy.EnablePcapAll("wifi-trace");

  // Run the simulation
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
