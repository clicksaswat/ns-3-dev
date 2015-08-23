/*
 * arp-cache-helper-test-suite.cc
 *
 *  Created on: 12-Aug-2015
 *      Author: saswat
 */

#include "ns3/simulator.h"
#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/node.h"

#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/neighbor-table-helper.h"
#include "ns3/v4ping-helper.h"
#include "ns3/v4ping.h"

using namespace ns3;

class ArpCacheHelperTestCase : public TestCase
{
public:

  ArpCacheHelperTestCase ();

  virtual
  ~ArpCacheHelperTestCase ();

  virtual void
  DoRun (void);
};

ArpCacheHelperTestCase::ArpCacheHelperTestCase () :
    TestCase ("Verify the ArpCacheHelper class functionalities")
{

}

ArpCacheHelperTestCase::~ArpCacheHelperTestCase ()
{

}

void
ArpCacheHelperTestCase::DoRun (void)
{
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
  //check for arp entries present for all IPs of the devices associated
  //with the channel should return false
  uint32_t remoteAddressValue = 0x0A010101;
  for (int i = 0; i < 9; i++)
    {
      Ipv4Address remoteAddress (remoteAddressValue);
      ArpCache::Entry* entry = arp.GetEntry (hostInterface, remoteAddress);
      NS_TEST_EXPECT_MSG_EQ (entry, 0, "No Entry corresponding to IP" + remoteAddress + "shoud exist" );
      remoteAddressValue++;
    }

  remoteAddressValue = 0x0A010101;
  //this will populate arp cache entries associated with all devices on channel
  arp.PopulateArpCache (hostInterface);
  for (int i = 0; i < 9; i++)
    {
      Ipv4Address remoteAddress (remoteAddressValue);
      ArpCache::Entry *entry = arp.GetEntry (hostInterface, remoteAddress);
      NS_TEST_EXPECT_MSG_EQ (!entry, 0, "Entry Corresponding to IP" + remoteAddress + "should exist");
    }


  ArpCache::Entry *entry = arp.GetEntry (hostInterface, "10.1.1.9");
  arp.RemoveEntry (hostInterface, entry);



  V4PingHelper ping ("10.1.1.9");
  ping.SetAttribute ("Interval", TimeValue (Seconds (5.0)));
  ApplicationContainer pingApp = ping.Install (host);

  //get the ping app for the host node
  V4Ping *hostApp = dynamic_cast<V4Ping*>(GetPointer(pingApp.Get (0)));
  //send an ping to the node 10.1.1.9
  hostApp->Send();
  //after pinging host arpcache should be having an entry for 10.1.1.9



  Simulator::Destroy ();

}
