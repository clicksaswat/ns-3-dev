/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "ns3/application.h"
#include "ns3/application-container.h"
#include "ns3/arp-cache.h"
#include "ns3/boolean.h"
#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-address-generator.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/neighbor-table-helper.h"
#include "ns3/net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/test.h"
#include "ns3/v4ping-helper.h"
#include "ns3/log.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ArpCacheHelperTestSuite");

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
  csma.SetChannelAttribute ("Delay", StringValue ("2ms"));
  csma.EnablePcapAll ("arp-cache-test", BooleanValue (true));
  NetDeviceContainer devices = csma.Install (nodes);

  //Get device associated with host node
  Ptr<NetDevice> hostDevice = devices.Get (0);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  //Get the Ipv4 object associated with the first node.
  Ptr<Ipv4> hostIp = host->GetObject<Ipv4> ();
  //Get the interface object associated with the CSMA devices
  uint32_t index = hostIp->GetInterfaceForDevice (hostDevice);
  //Get the interface object associated with the host
  Ptr<Ipv4Interface> hostInterface = host->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  //Get the arpcache object associated with the host
  Ptr<ArpCache> arpCache = hostInterface->GetArpCache ();

  //Create the helper object
  ArpCacheHelper arp;


  //Currently, there doesn't exist any way to know the size of Arp Cache Table
  //So, we'll check individual IP entry for all the devices and confirm its existence

  //check for arp entries present for all IPs of the devices associated
  //with the channel should return false because no packet has been transmitted yet

  Ipv4Address remoteAddress("10.1.1.1");
  for (int i = 0; i < 9; i++)
    {
      remoteAddress.Set (remoteAddress.Get () + 1);
      ArpCache::Entry* entry = arpCache->Lookup (remoteAddress);
      NS_TEST_EXPECT_MSG_EQ (entry, (void *) NULL, "No Entry corresponding to IP " << remoteAddress << "should exist" );
    }

  std::cout << "out of first loop" << std::endl;

  //this will populate arp cache entries associated with all devices on channel
  arp.PopulateArpCache (hostInterface);

  //Now that all entry must have been populated
  //check if they exist in the Neighbor Table

  remoteAddress.Set ("10.1.1.1");
  for (int i = 0; i < 9; i++)
    {
      remoteAddress.Set (remoteAddress.Get () + 1);
      ArpCache::Entry *entry = arpCache->Lookup (remoteAddress);
      std::cout << entry << std::endl;
      NS_TEST_EXPECT_MSG_NE (entry, (void *) NULL, "Entry Corresponding to IP " << remoteAddress << "should exist");
    }



  //Use RemoveEntry and GetEntry Methods
  ArpCache::Entry *entry = arp.GetEntry (hostInterface, "10.1.1.9");
  arp.RemoveEntry (hostInterface, entry);
  entry = arp.GetEntry (hostInterface, "10.1.1.9");
  NS_TEST_EXPECT_MSG_EQ (entry, (void *) NULL, "Entry corresponding to IP 10.1.1.9 removed successfully");

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

  Simulator::Run ();

  entry = arpCache->Lookup (Ipv4Address ("10.1.1.9"));
  NS_TEST_EXPECT_MSG_NE (entry, (void *) NULL, "Entry corresponding to 10.1.1.9 exists");
  Simulator::Destroy ();
}

class ArpCacheHelperTestSuite : public TestSuite
{
public:
  ArpCacheHelperTestSuite ()
    :TestSuite ("arp-cache-helper", UNIT)
  {
    AddTestCase (new ArpCacheHelperTestCase, TestCase::QUICK);
  }
};

static ArpCacheHelperTestSuite g_arpCacheHelperTestSuite;

