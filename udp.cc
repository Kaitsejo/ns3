#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
using namespace ns3;
int main (int argc, char *argv[])
{

CommandLine cmd;
cmd.Parse (argc, argv);
Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
lteHelper->SetEpcHelper(epcHelper);
NodeContainer enbNodes;
NodeContainer ueNodes;
enbNodes.Create (1);
ueNodes.Create (2);
NodeContainer srvNodes;
srvNodes.Create (1);
MobilityHelper mobility;
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (enbNodes);
mobility.Install (ueNodes);
NetDeviceContainer enbDevs;
NetDeviceContainer ueDevs;
enbDevs = lteHelper->InstallEnbDevice (enbNodes);
ueDevs = lteHelper->InstallUeDevice (ueNodes);
InternetStackHelper tcpip;
tcpip.Install(ueNodes);
Ipv4InterfaceContainer ueIpAddrs;
ueIpAddrs = epcHelper->AssignUeIpv4Address(ueDevs);
lteHelper->Attach (ueDevs, enbDevs.Get (0));
ipv4AddressHelper.SetBase ("10.0.0.0", "255.0.0.0");
Ptr<Ipv4> ipv4 = tcpip.GetObject<Ipv4>();
Ipv4InterfaceContainer InternetIpInfs = ipv4->Assign (ipv4AddressHelper);
uint16_t port = 4000;
UdpServerHelper server (port);
ApplicationContainer apps = server.Install(srvNodes);
apps.Start (Seconds(1.0));
apps.Stop (Seconds(10.0));
uint32_t MaxPacketSize = 1024;
Time interPacketInterval = Seconds(0.05);
uint32_t maxPacketCount = 320;
UdpClientHelper client (InternetIpInfs.GetAddress(1),port);
client.SetAttribute("MaxPackets", UintegerValue (maxPacketCount));
client.SetAttribute("Interval", TimeValue(interPacketInterval));
client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
ApplicationContainer clientApps = client.Install(ueNodes);
clientApps.Start(Seconds(2.0));
clientApps.Stop(Seconds(10.0));
LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
Simulator::Stop(Seconds(10.0));
Simulator::Run();
Simulator::Destroy();
 return 0;
}

