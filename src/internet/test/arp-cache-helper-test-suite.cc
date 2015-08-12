/*
 * arp-cache-helper-test-suite.cc
 *
 *  Created on: 12-Aug-2015
 *      Author: saswat
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/node.h"
#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/neighbor-table-helper.h"
#include "ns3/v4ping-helper.h"
#include "ns3/v4ping.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ArpCacheHelperTestSuite");

int main (int argc, char *argv[])
{
  LogComponentEnable ("NeighborTableHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
  LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);

  NodeContainer nodes;
  nodes.Create (10);
  Ptr<Node> host = nodes.Get (0);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (5000)));
  NetDeviceContainer devices = csma.Install (nodes);
  csma.EnablePcapAll ("arp-cache-test", BooleanValue (true));
  //Get device associated with host node
  Ptr<NetDevice> hostDevice = devices.Get (0);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  //Get the Ipv4 object associated with the first node.
  Ptr<Ipv4> hostIp = interfaces.Get (0).first;
  //Get the interface object associated with the CSMA devices
  uint32_t index = hostIp->GetInterfaceForDevice (hostDevice);
  Ptr<Ipv4Interface> hostInterface = host->GetObject<Ipv4L3Protocol> ()->GetInterface (index);

  ArpCacheHelper arp;
  Ptr<OutputStreamWrapper> output = Create<OutputStreamWrapper> (&std::cout);
  arp.PrintEntries (hostInterface, output);

  arp.PopulateArpCache (hostInterface);
  arp.PrintEntries (hostInterface, output);

  ArpCache::Entry *entry = arp.GetEntry (hostInterface, "10.1.1.9");
  arp.RemoveEntry (hostInterface, entry);
  arp.PrintEntries (hostInterface, output);

  V4PingHelper ping ("10.1.1.9");
  ping.SetAttribute ("Interval", TimeValue (Seconds (5.0)));
  ApplicationContainer pingApp = ping.Install (host);
  pingApp.Start (Seconds (2.0));
  pingApp.Stop (Seconds (20.0));


  Ptr<NetDevice> remoteDevice = devices.Get (9);
  ArpCache::Entry *newEntry = arp.MakeEntry ("10.1.1.9", remoteDevice->GetAddress ());
  Simulator::Schedule (Seconds (5.0), &ArpCacheHelper::AddEntry, arp, hostInterface, newEntry);


  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}


