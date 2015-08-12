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

using namespace ns3;

int main (int argc, char *argv[])
{
  NodeContainer nodes;
  nodes.Create (10);
  Ptr<Node> host = nodes.Get (0);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (5000)));
  NetDeviceContainer devices = csma.Install (nodes);
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
  Ptr<OutputStreamWrapper> output = Create<OutputStreamWrapper>(&std::cout);
  arp.PrintEntries (hostInterface, output);
  arp.PopulateArpCache (hostInterface);
  arp.PrintEntries (hostInterface, output);

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}


