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
  //TO-DO have to modify the ArpCache class to provide this function
}

void
ArpCacheHelper::AddEntry (uint32_t index, Ptr<ArpCache::Entry> entry) const
{
	//TO-DO have to modify the ArpCache class to provide this function
}

Ptr<ArpCache::Entry>
ArpCacheHelper::MakeEntry (Ipv4Address ipv4Address, Address macAddress, std::string status = "ALIVE") const
{
  //TO-DO have to provide a constructor for Entry without any parameter.
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
		entry->MarkWaitReply (entry->GetPendingPackets ()); //TO-DO Implement GetPendingPackets() in the ArpCache::Entry class
	else if (status == "PERMANENT")
		entry->MarkPermanent ();
	else NS_LOG_WARN ("Invalid Status value. Possible values \"ALIVE\", \"DEAD\", \"WAIT_REPLY\", \"PERMANENT\"");
}

void
ArpCacheHelper::ChangeEntryAddress (Ptr<ArpCache::Entry> entry, Address macAddress) const
{
	//TO-DO implement SetMacAddress() in ArpCache::Entry
}


}
