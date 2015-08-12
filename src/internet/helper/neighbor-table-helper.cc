/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author: Saswat K Mishra <clicksaswat@gmail.com>
 */

#include "neighbor-table-helper.h"


#include "ns3/node.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/log.h"
#include "ns3/log-macros-disabled.h"
#include "ns3/net-device.h"
#include "ns3/channel.h"
#include "ns3/arp-cache.h"
#include "ns3/ipv4-interface.h"





NS_LOG_COMPONENT_DEFINE ("NeighborTableHelper");

namespace ns3 {


ArpCacheHelper::ArpCacheHelper (void)
{
  NS_LOG_FUNCTION (this);
}

ArpCacheHelper::~ArpCacheHelper (void)
{
  NS_LOG_FUNCTION (this);
}

void
ArpCacheHelper::PrintEntries (Ptr<Ipv4Interface> ipv4Interface, Ptr<OutputStreamWrapper> stream) const
{
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  arpCache->PrintArpCache (stream);
}

void
ArpCacheHelper::PrintEntries (Ptr<Node> node, uint32_t index, Ptr<OutputStreamWrapper> stream) const
{
  Ptr<Ipv4Interface> ipv4Interface = node->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  arpCache->PrintArpCache (stream);
}

void
ArpCacheHelper::PrintEntriesAll (Ptr<Node> node, Ptr<OutputStreamWrapper> stream) const
{
  uint32_t numInterface = node->GetObject<Ipv4L3Protocol> ()->GetNInterfaces ();
  for (uint32_t i = 0; i < numInterface; i++)
    this->PrintEntries (node, i, stream);
}

void
ArpCacheHelper::AddEntry (Ptr<Ipv4Interface> ipv4Interface, ArpCache::Entry *entry) const
{
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  entry->SetArpCache (PeekPointer (arpCache));
}

void
ArpCacheHelper::AddEntry (Ptr<Node> node, uint32_t index, ArpCache::Entry *entry) const
{
  Ptr<Ipv4Interface> ipv4Interface = node->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  entry->SetArpCache (PeekPointer (arpCache));
}

ArpCache::Entry *
ArpCacheHelper::MakeEntry (Ipv4Address ipv4Address, Address macAddress, std::string status) const
{
  ArpCache::Entry *entry = new ArpCache::Entry ();
  entry->SetIpv4Address (ipv4Address);
  entry->SetMacAddress (macAddress);
  this->ChangeEntryStatus (entry, status);
  return entry;
}

ArpCache::Entry *
ArpCacheHelper::GetEntry (Ptr<Ipv4Interface> ipv4Interface, Ipv4Address ipv4Address) const
{
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  return arpCache->Lookup (ipv4Address);
}

ArpCache::Entry *
ArpCacheHelper::GetEntry (Ptr<Node> node, uint32_t index, Ipv4Address ipv4Address) const
{
  Ptr<Ipv4Interface> ipv4Interface = node-> GetObject<Ipv4L3Protocol> ()-> GetInterface (index);
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  return arpCache->Lookup (ipv4Address);
}

void
ArpCacheHelper::RemoveEntry (Ptr<Ipv4Interface> ipv4Interface, ArpCache::Entry *entry) const
{
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  arpCache->Remove (entry);
}

void
ArpCacheHelper::RemoveEntry (Ptr<Node> node, uint32_t index, ArpCache::Entry *entry) const
{
  Ptr<Ipv4Interface> ipv4Interface = node-> GetObject<Ipv4L3Protocol> ()-> GetInterface (index);
  Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
  arpCache->Remove (entry);
}

void
ArpCacheHelper::ChangeEntryStatus (ArpCache::Entry *entry, std::string status) const
{
  if (status == "ALIVE")
    entry->MarkAlive (entry->GetMacAddress ());
  else if (status == "DEAD")
    entry->MarkDead ();
  else if (status == "WAIT_REPLY")
    entry->MarkWaitReply ();
  else if (status == "PERMANENT")
    entry->MarkPermanent ();
  else NS_LOG_WARN ("Invalid Status value. Possible values \"ALIVE\", \"DEAD\", \"WAIT_REPLY\", \"PERMANENT\"");
}

void
ArpCacheHelper::ChangeEntryAddress (ArpCache::Entry *entry, Address macAddress) const
{
  entry->SetMacAddress (macAddress);
}

void
ArpCacheHelper::PopulateArpCache (Ptr<Ipv4Interface> interface)
{
  Ptr<NetDevice> nd = interface->GetDevice ();
  Ptr<Node> node = nd->GetNode ();
  Ptr<Channel> channel = nd->GetChannel ();
  Ptr<ArpCache> arpCache = interface->GetArpCache ();


  //find out no. of devices connected to the same channel
  uint32_t numDevices = channel->GetNDevices ();
  for (uint32_t i = 0; i < numDevices; i++)
    {
      Ptr<NetDevice> remote = channel->GetDevice (i);
      Ptr<Ipv4L3Protocol> remoteIp = remote->GetNode ()->GetObject<Ipv4L3Protocol> ();
      uint32_t index = remoteIp->GetInterfaceForDevice (remote);
      Ptr<Ipv4Interface> remoteInterface = remoteIp->GetInterface (index);

      //for each IP address in remote interface check if it belongs in
      //the subnet of any of the IP address of host interface

      uint32_t numRemoteIp = remoteInterface->GetNAddresses ();
      uint32_t numHostIp = interface->GetNAddresses ();

      for (uint32_t j = 0; j < numRemoteIp; j++)
	{
	  Ipv4Address remoteAddress = remoteInterface->GetAddress (j).GetLocal ();
	  Ipv4Mask remoteMask = remoteInterface->GetAddress (j).GetMask ();

	  //find the network or base address of host address
	  Ipv4Address remoteNetwork = remoteAddress.CombineMask (remoteMask);

	  for (uint32_t k = 0; k < numHostIp; k++)
	    {
	      Ipv4Address hostAddress = interface->GetAddress (k).GetLocal ();
	      Ipv4Mask hostMask = interface->GetAddress (k).GetMask ();

	      //find the network or base address of host address
	      Ipv4Address hostNetwork = hostAddress.CombineMask (hostMask);

	      //find out if remote IP Address is in the subnet of host IP Address
	      if (remoteNetwork == hostNetwork)
		{
		  ArpCache::Entry* entry = new ArpCache::Entry (PeekPointer (arpCache));
		  entry->SetIpv4Address (remoteAddress);
		  entry->SetMacAddress (remote->GetAddress ());
		  entry->MarkPermanent ();
		}
	    }
	}
    }
}

}

