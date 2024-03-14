#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
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
UdpEchoServerHelper echoServer(9);
ApplicationContainer serverApps = echoServer.Install (ueNodes.Get(1));
serverApps.Start (Seconds(1.0));
serverApps.Stop (Seconds(10.0));
UdpEchoClientHelper echoClient (ueIpAddrs.GetAddress(1), 9);
echoClient.SetAttribute ("MaxPackets", UintegerValue(1));
echoClient.SetAttribute ("Interval", TimeValue(Seconds (1.0)));
echoClient.SetAttribute ("PacketSize", UintegerValue(1024));
ApplicationContainer clientApps = echoClient.Install(ueNodes.Get(0));
clientApps.Start (Seconds (2.0));
clientApps.Stop (Seconds (10.0));
LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
Simulator::Stop(Seconds(10.0));
Simulator::Run();
Simulator::Destroy();
return 0;
}
