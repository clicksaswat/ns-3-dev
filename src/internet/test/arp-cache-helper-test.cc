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

ArpCacheHelperTestCase::ArpCacheHelperTestCase ()
  : TestCase ("Verify the ArpCacheHelper class functionalities")
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

  Ipv4AddressHelper address ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  //Get the Ipv4 object associated with the first node.
  Ptr<Ipv4> hostIp = hostDevice->GetObject<Ipv4> ();
  //Get the interface object associated with the CSMA devices
  uint32_t index = hostIp->GetInterfaceForDevice (hostDevice);
  Ptr<Ipv4Interface> hostInterface = host->GetObject<Ipv4L3Protocol> ()->GetInterface (index);

  ArpCacheHelper arp;

  //Currently, there doesn't exist any way to know the size of Arp Cache Table
  //So, we'll check individual IP entry for all the devices and confirm its existence

  //check for arp entries present for all IPs of the devices associated
  //with the channel should return false

  address.SetBase ("10.1.1.0", "255.255.255.0");
  for (int i = 0; i < 9; i++)
    {
      Ipv4Address remoteAddress = address.NewAddress ();
      ArpCache::Entry* entry = arp.GetEntry (hostInterface, remoteAddress);
      NS_TEST_EXPECT_MSG_EQ (entry, false, "No Entry corresponding to IP" + remoteAddress + "should exist" );
    }


  //this will populate arp cache entries associated with all devices on channel
  arp.PopulateArpCache (hostInterface);

  //Now that all entry must have been populated
  //check if they exist in the Neighbor Table

  address.SetBase ("10.1.1.0", "255.255.255.0");
  for (int i = 0; i < 9; i++)
    {
      Ipv4Address remoteAddress = address.NewAddress ();
      ArpCache::Entry *entry = arp.GetEntry (hostInterface, remoteAddress);
      NS_TEST_EXPECT_MSG_EQ (!entry, 0, "Entry Corresponding to IP" + remoteAddress + "should exist");
    }

  //Remove an entry
  ArpCache::Entry *entry = arp.GetEntry (hostInterface, "10.1.1.9");
  arp.RemoveEntry (hostInterface, entry);
  entry = arp.GetEntry (hostInterface, "10.1.1.9");
  NS_TEST_EXPECT_MSG_EQ (entry, 0, "Entry corresponding to IP 10.1.1.9 removed successfully");

  //We'll ping to 10.1.1.9
  //which will generate an Arp Entry for corresponding IP
  V4PingHelper ping ("10.1.1.9");
  ApplicationContainer pingApps = ping.Install (host);
  Ptr<Application> hostPingApp = pingApps.Get (0);

  //Start and stop the ping app on the host node
  //instead of setting start and stop time we may
  //directly call the pingApps send method. But, that
  //will be little clumsy
  hostPingApp->SetStartTime (Seconds (1));;
  hostPingApp->SetStopTime (Seconds (2));


  //Now, we must have an neighbor table entry corresponding
  //to 10.1.1.9
  entry = arp.GetEntry (hostInterface, "10.1.1.9");
  NS_TEST_EXPECT_MSG_EQ (entry, false, "Entry corresponding to 10.1.1.9 exists");

  //TODO: Find out ways to test ArpCacheHelper::ChangeEntryStatus as ArpCacheHelper::ChangeEntryAdress




  Simulator::Destroy ();

}
