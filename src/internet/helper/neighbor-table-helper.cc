/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author: Saswat K Mishra <clicksaswat@gmail.com>
 */

#include <stdint.h>
#include "ns3/node.h"
#include "ns3/ipv4-interface.h"
#include "ns3/log.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-l3-protocol.h"
#include "neighbor-table-helper.h"




NS_LOG_COMPONENT_DEFINE ("NeighborTableHelper");

namespace ns3 {

ArpCacheHelper::ArpCacheHelper (void)
  : m_node (0),
    m_arpCacheList (0)
{

}

ArpCacheHelper::ArpCacheHelper (Ptr<Node> node)
{
	this->Setup (node);
}

ArpCacheHelper::~ArpCacheHelper (void)
{
  delete m_arpCacheList;
}

void
ArpCacheHelper::Setup (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  //clear the previous database
  if (!m_arpCacheList.empty())
    {
      NS_LOG_DEBUG ("Clearing Previous Database");
      m_arpCacheList.clear();
    }

  //set the node of the helper object
  m_node = node;

  //extract all the Ipv4Interfaces associated with the node and add them to the database.
  Ptr<Ipv4L3Protocol> ipv4 = node->GetObject<Ipv4L3Protocol> ();
  uint32_t numOfInterfaces =  ipv4->GetNInterfaces ();

  for (int i = 0; i < numOfInterfaces; i++)
    {
      Ptr<Ipv4Interface> ipv4Interface = ipv4->GetInterface (i);
      Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
      m_arpCacheList[ipv4Interface] = arpCache;
    }
}

void
ArpCacheHelper::Update (void)
{
	this->Setup (m_node);
}

void
ArpCacheHelper::PrintEntries (Ptr<Ipv4Interface> ipv4Interface, Ptr<OutputStreamWrapper> stream) const
{
  Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
  arpCache->PrintArpCache (stream);
}

void
ArpCacheHelper::PrintEntries (uint32_t index, Ptr<OutputStreamWrapper> stream) const
{
	Ptr<Ipv4Interface> ipv4Interface = m_node-> GetObject<Ipv4L3Protocol> ()-> GetInterface (index);
	Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
	arpCache->PrintArpCache (stream);
}


void
ArpCacheHelper::PrintEntriesAll (Ptr<OutputStreamWrapper> stream) const
{
  for (ArpCacheList::const_iterator it = m_arpCacheList.begin(); it != m_arpCacheList.end(); it++)
    {
      Ptr<ArpCache> arpCache = it->second;
      arpCache->PrintArpCache (stream);
    }
}

void
ArpCacheHelper::AddEntry (Ptr<Ipv4Interface> ipv4Interface, Ptr<ArpCache::Entry> entry) const
{
  Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
  entry->SetArpCache (arpCache);
}

void
ArpCacheHelper::AddEntry (uint32_t index, Ptr<ArpCache::Entry> entry) const
{
  Ptr<Ipv4Interface> ipv4Interface = m_node->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
  entry->SetArpCache (arpCache);
}

Ptr<ArpCache::Entry>
ArpCacheHelper::MakeEntry (Ipv4Address ipv4Address, Address macAddress, std::string status = "ALIVE") const
{
  ArpCache::Entry *entry = new ArpCache::Entry ();
  entry->SetIpv4Address (ipv4Address);
  entry->SetMacAddress (macAddress);
  this->ChangeEntryStatus (entry, status);
  return entry;
}

Ptr<ArpCache::Entry>
ArpCacheHelper::GetEntry (Ptr<Ipv4Interface> ipv4Interface, Ipv4Address ipv4Address) const
{
  Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
  return arpCache->Lookup (ipv4Address);
}

Ptr<ArpCache::Entry>
ArpCacheHelper::GetEntry (uint32_t index, Ipv4Address ipv4Address) const
{
  Ptr<Ipv4Interface> ipv4Interface = m_node-> GetObject<Ipv4L3Protocol> ()-> GetInterface (index);
  Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
  return arpCache->Lookup (ipv4Address);
}

void
ArpCacheHelper::RemoveEntry (Ptr<Ipv4Interface> ipv4Interface, Ptr<ArpCache::Entry> entry) const
{
	Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
	arpCache->Remove (entry);
}

void
ArpCacheHelper::RemoveEntry (uint32_t index, Ptr<ArpCache::Entry> entry) const
{
	Ptr<Ipv4Interface> ipv4Interface = m_node-> GetObject<Ipv4L3Protocol> ()-> GetInterface (index);
	Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
	arpCache->Remove (entry);
}
void
ArpCacheHelper::ChangeEntryStatus (Ptr<ArpCache::Entry> entry, std::string status) const
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
ArpCacheHelper::ChangeEntryAddress (Ptr<ArpCache::Entry> entry, Address macAddress) const
{
  entry->SetMacAddress (macAddress);
}

void
ArpStackHelper::PopulateArpCache(Ptr<Ipv4Interface> interface)
{
  Ptr<ArpL3Protocol> arp = interface->GetObject<ArpL3Protocol> ();
  Ptr<ArpCache> arpCache = interface->GetArpCache();

  //flush the ArpCache before populating
  arpCache->Flush ();

  //Get No. of IpAddress associated with this interface
  uint32_t nAddress = interface->GetNAddresses ();

  for (int i = 0; i < nAddress; i++)
    {
      Ipv4InterfaceAddress ipInterface = interface->GetAddress (i);
      Ipv4Address local = ipInterface.GetLocal ();
      Ipv4Mask mask = ipInterface.GetMask ();

      //Calculate subnet base address from local & mask
      Ipv4Address network = local.CombineMask (mask);

      //Generate an Ipv4AddressHelper which will generate all IPs in the subnet
      Ipv4AddressHelper addressHelper = new Ipv4AddressHelper (network, mask);

      //figure out max possible no. of subnet IPs for this mask
      uint32_t maskValue = mask.Get ();
      uint32_t maxValue = 1;

      while (true)
	{
	  if (maskValue & 1)
	    break;
	  maskValue >>= 1;
	  maxValue <<= 1;
	}

      maxValue -= 2; //Don't use network & broadcast address (all 0's & all 1's)

      //send arp request for all subnet IPs
      for (int i = 0; i < maxValue; i++)
	{

	  Ipv4Address to = addressHelper.NewAddress ();
	  arp->SendArpRequest (arpCache, to);

	  //sending Arp request will not create an entry in this cache
	  //So, we manually add one & mark it WAIT_REPLY

	  ArpCache::Entry *entry = arpCache->Add (to);
	  entry->MarkWaitReply ();

	  //TO-DO
	  //Some devices or channels may be introducing latency during sending or receiving
	  //Arp Request/Reply. Find a way to make sure sent packet is gone and all received
	  //packet(if there is any) has been processed by ArpL3Protocol

	  //ensured that we've got the Arp Reply (if there is any)
	  //if Mac Address is not null we've got the reply
	  if (entry->GetMacAddress ())
	    entry->MarkPermanent ();
	  else arpCache->Remove (entry);

	 }
       }

}

}
