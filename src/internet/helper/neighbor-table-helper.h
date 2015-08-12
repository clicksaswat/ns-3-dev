/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef NEIGHBOR_TABLE_HELPER_H_
#define NEIGHBOR_TABLE_HELPER_H_

#include <stdint.h>
#include <string>
#include <map>
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-address.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/address.h"
#include "ns3/arp-cache.h"


namespace ns3 {


class ArpCacheHelper
{
public:

  /**
   * \brief Default Constructor
   */
  ArpCacheHelper (void);

  /**
   * \brief Destroy the ArpCacheHelper
   */
  virtual ~ArpCacheHelper (void);

  /**
   * \brief Print all the ArpCache Entries associated with the Ipv4Interface
   *
   * \param ipv4Interface pointer to the Ipv4Interface of which entries will be printed.
   * \param the ostream to which entries will be printed
   */
  void PrintEntries (Ptr<Ipv4Interface> ipv4interface,
		     Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Print all the ArpCache Entries associated with the Ipv4Interface
   * 		corresponding to the index value
   *
   * \param node pointer to the Node
   * \param index index of Ipv4Interface on the Node
   * \param stream the ostream to which entries will be printed
   */
  void PrintEntries (Ptr<Node> node,
		     uint32_t index,
		     Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Print all the ArpCache entries associated with the Node.
   *
   * \param node pointer to the Node
   * \param stream the ostream to which entries will be printed
   */
  void PrintEntriesAll (Ptr<Node> node,
			Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Add an Entry to the ArpCache of Ipv4Interface
   *
   * \param ipv4Interface pointer to the Ipv4Interface to which Entry is to be added.
   * \param entry pointer to a Entry object. Can be obtained using MakeEntry();
   */
  void AddEntry (Ptr<Ipv4Interface> ipv4interface,
		 ArpCache::Entry *entry) const;

  /**
   * \brief Add an Entry to the ArpCache of Ipv4Interface corresponding to the index value
   *
   * \param node pointer to the Node
   * \param index index of the Ipv4Interface on the node
   * \param entry pointer to a Entry object. Can be obtained using MakeEntry()
   */
  void AddEntry (Ptr<Node> node,
		 uint32_t index,
		 ArpCache::Entry *entry) const;

  /**
   * \brief Make an Entry object using the parameters passed
   *
   * \param ipv4Address the ipv4Address for the entry
   * \param macAddress the MAC Address of the entry. By default null
   * \param state a string representing the State of the Entry. Can take four values
   * 		  "ALIVE", "WAIT_REPLY", "DEAD", "PERMANENT". By default "ALIVE"
   *
   * \returns pointer to the newly created Entry.
   */
  ArpCache::Entry *MakeEntry (Ipv4Address ipv4Address, Address macAddress, std::string status = "ALIVE") const;

  /**
   * \brief Get an Entry object associated with the Ipv4Address of the Ipv4Interface
   *
   * \param ipv4Interface the Ipv4Interface where the Entry will be looked upon
   * \param ipv4Address the Ipv4Address of the ArpCache Entry
   *
   * \returns pointer to the Entry object if found, returns 0 otherwise
   */
  ArpCache::Entry *GetEntry (Ptr<Ipv4Interface> ipv4Interface,
		             Ipv4Address ipv4Address) const;

  /**
   * \brief Get an Entry object associated with the Ipv4Address of the Ipv4Interface corresponding to the index value
   *
   * \param node pointer to the Node
   * \param index index of the Ipv4Interface on the node.
   * \param ipv4Address the Ipv4Address of the ArpCache Entry
   */
  ArpCache::Entry *GetEntry (Ptr<Node> node,
			     uint32_t index,
			     Ipv4Address ipv4Address) const;

  /**
   * \brief Remove the Entry from the Ipv4Interface
   *
   * \param ipv4Interface the Ipv4Interface from which corresponding entry will be removed
   * \param entry the ArpCache Entry to be removed, the pointer to the entry can
   * 			  be obtained using GetEntry() method
   *
   */
  void RemoveEntry (Ptr<Ipv4Interface> ipv4Interface,
		    ArpCache::Entry *entry) const;

  /**
   * \brief Remove the Entry from the Ipv4Interface corresponding to the index value
   *
   * \param node pointer to the Node
   * \param index index of the Ipv4Interface on the Node
   * \param entry the ArpCache Entry to be remvoed, the pointer to the entry can be
   * 			  obtianed using GetEntry() method
   */
  void RemoveEntry (Ptr<Node> node,
		    uint32_t index,
		    ArpCache::Entry *entry) const;

  /**
   * \brief Change the status of a Entry.
   *
   * \param entry pointer to the Entry of which status is to be changed. Can be obtained using the GetEntry() method
   * \param status a string representing the status. Possible values:  "ALIVE", "WAIT_REPLY", "DEAD", "PERMANENT"
   *
   */
  void ChangeEntryStatus (ArpCache::Entry *entry,
			  std::string status) const;

  /**
   * \brief Change the MAC Address of the Entry.
   *
   * \param entry pointer to the Entry of which MAC address is to be changed. Can be obtained using the GetEntry() method
   * \param macAddress the new value of MAC Address which the Entry will point to
   */
  void ChangeEntryAddress (ArpCache::Entry *entry,
			   Address macAddress) const;

  /**
   * \brief Method will search for all the devices connected to the
   * 	    same channel as the device associated with the interface
   * 	    passed on as parameter. It will add permanent arp cache
   * 	    entry for the IP addresses associated with the remote
   * 	    devices(devices on the same channel). This method can be
   * 	    used to pre populate Arp cache to bypass the delay arising
   * 	    from Arp Request/Reply.
   *
   * \param ipv4Interface the Ipv4Interface pointer for which ArpCache will
   * 			 be populated.
   */
   void PopulateArpCache (Ptr<Ipv4Interface> interface);

};


}

#endif /* NEIGHBOR_TABLE_HELPER_H_ */
