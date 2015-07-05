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



NS_LOG_COMPONENT_DEFINE (NeighborTableHelper);

namespace ns3 {

ArpCacheHelper::ArpCacheHelper (void)
  : m_node (0),
    m_arpCacheList (0)
{

}

ArpCacheHelper::~ArpCacheHelper (void)
{
  delete m_arpCacheList;
}

void
ArpCacheHelper::Initiate (Ptr<Node> node)
{
  //clear the previous database
  if (!m_arpCacheList.empty())
    m_arpCacheList.clear();

  //set the node of the helper object
  m_node = node;

  //extract all the Ipv4Interfaces associated with the node and add them to the database.
  uint32_t noOfDevices =  node->GetNDevices();


  Ptr<Ipv4L3Protocol> ipv4 = node->GetObject<Ipv4L3Protocol>();

  for (int i = 0; i < noOfDevices; i++)
    {
      Ptr<Ipv4Interface> ipv4Interface = ipv4->GetInterfaceForDevice (node->GetDevice(i));
      Ptr<ArpCache> arpCache = ipv4Interface->GetArpCache ();
      m_arpCacheList[ipv4Interface] = arpCache;
    }

}

Ptr<Ipv4Interface>
ArpCacheHelper::GetInterface (uint32_t index) const
{
  return m_node->GetObject<Ipv4L3Protocol> ()->GetInterfaceForDevice (m_node->GetDevice (index));
}

uint32_t
ArpCacheHelper::GetNInterfaces (void) const
{
  return m_node->GetNDevices ();
}

void
ArpCacheHelper::PrintEntries (Ptr<Ipv4Interface> ipv4Interface) const
{
  Ptr<ArpCache> arpCache = m_arpCacheList[ipv4Interface];
  arpCache->PrintArpCache(); //TO-DO find out how to pass standard output.
}

void
ArpCacheHelper::PrintEntriesAll (void) const
{
  for (ArpCacheList::const_iterator it = m_arpCacheList.begin(); it != m_arpCacheList.end(); it++)
    {
      Ptr<ArpCache> arpCache = it->second;
      arpCache->PrintArpCache(); //TO-DO find out how to pass standard output.
    }
}

void
ArpCacheHelper::AddEntry (Ptr<Ipv4Interface> ipv4Interface, Ptr<ArpCache::Entry> entry) const
{
  //TO-DO have to modify the ArpCache class to provide this function
}

Ptr<ArpCache::Entry>
ArpCacheHelper::MakeEntry (Ipv4Address ipv4Address, Address macAddress = null, char *state = "ALIVE" ) const;
{
  //TO-DO have to provide a constructor for Entry without any parameter.
}







}
