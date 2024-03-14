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
Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
lteHelper->SetEpcHelper (epcHelper);
UdpEchoServerHelper echoServer(9);
NodeContainer enbNodes;
NodeContainer ueNodes;
NodeContainer srvNodes;
enbNodes.Create (1);
ueNodes.Create (1);
srvNodes.Create (1);
MobilityHelper mobility;
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (enbNodes);
mobility.Install (ueNodes);
mobility.Install (srvNodes);
PointToPointHelper p2pHelper;
p2pHelper.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
p2pHelper.SetDeviceAttribute("Mtu", UintegerValue(1500));
p2pHelper.SetChannelAttribute("Delay", TimeValue(Seconds(0.01)));
NetDeviceContainer enbDevs;
NetDeviceContainer ueDevs;
NetDeviceContainer p2pDevs;
enbDevs = lteHelper->InstallEnbDevice (enbNodes);
ueDevs = lteHelper->InstallUeDevice (ueNodes);
Ptr<Node> pgw = epcHelper->GetPgwNode ();
Ptr<Node> srv = srvNodes.Get (0);
p2pDevs = p2pHelper.Install (pgw, srv);
InternetStackHelper tcpip;
tcpip.Install(ueNodes);
tcpip.Install(srvNodes);
Ipv4InterfaceContainer ueIpInfs;
Ipv4InterfaceContainer InternetIpInfs;
ueIpInfs = epcHelper->AssignUeIpv4Address(ueDevs);
Ipv4AddressHelper ipv4Helper;
ipv4Helper.SetBase("1.0.0.0", "255.0.0.0");
InternetIpInfs = ipv4Helper.Assign(p2pDevs);
Ipv4StaticRoutingHelper ipv4RoutingHelper;
Ptr<Ipv4StaticRouting> srvStaticRouting =
ipv4RoutingHelper.GetStaticRouting(srv->GetObject<Ipv4>());
srvStaticRouting->AddNetworkRouteTo (Ipv4Address("7.0.0.0"),
Ipv4Mask("255.0.0.0"), 1);
Ptr<Node> ue = ueNodes.Get(0);
Ptr<Ipv4StaticRouting> ueStaticRouting =
ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
lteHelper->Attach(ueDevs.Get(0), enbDevs.Get(0));
ApplicationContainer serverApps = echoServer.Install(srv);
UdpEchoClientHelper echoClient (InternetIpInfs.GetAddress(1), 9);
ApplicationContainer clientApps = echoClient.Install(ue);
LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
Simulator::Stop(Seconds(10.0));
Simulator::Run();
Simulator::Destroy();
 return 0;
}
