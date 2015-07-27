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


namespace ns3 {


class ArpCacheHelper
{
public:

  /**
   * \brief Default Constructor
   */
  ArpCacheHelper (void);

  /**
   * \brief Constructor
   *
   * \param node pointer to the node for which ArpCacheHelper will be constructed.
   */
  ArpCacheHelper (Ptr<Node> node);

  /**
   * \brief Destroy the ArpCacheHelper
   */
  virtual ~ArpCacheHelper (void);

  /**
   * \brief Constructs the Helper Object for the node.
   *
   * \param pointer to the node for which helper object will be created.
   */

  void Setup (Ptr<Node> node);

  /**
   * \brief Update the database of ArpCacheHelper. Use this function if you've added
   * 		new Ipv4Interface to the Node or modified ArpCache associated with existing Ipv4Interface.
   */
  void Update (void);

  /**
   * \brief Print all the ArpCache Entries associated with the Ipv4Interface
   *
   * \param ipv4Interface pointer to the Ipv4Interface of which entries will be printed.
   * \param the ostream to which entries will be printed
   */
  void PrintEntries (Ptr<Ipv4Interface> ipv4interface, Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Print all the ArpCache Entries associated with the Ipv4Interface
   * 		corresponding to the index value
   *
   * \param index index of Ipv4Interface on the Node
   * \param stream the ostream to which entries will be printed
   */
  void PrintEntries (uint32_t index, Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Print all the ArpCache entries associated with the Node.
   */
  void PrintEntriesAll (Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Add an Entry to the ArpCache of Ipv4Interface
   *
   * \param ipv4Interface pointer to the Ipv4Interface to which Entry is to be added.
   * \param entry pointer to a Entry object. Can be obtained using MakeEntry();
   */
  void AddEntry (Ptr<Ipv4Interface> ipv4interface, Ptr<ArpCache::Entry> entry) const;

  /**
   * \brief Add an Entry to the ArpCache of Ipv4Interface corresponding to the index value
   *
   * \param index index of the Ipv4Interface on the node
   * \param entry pointer to a Entry object. Can be obtained using MakeEntry()
   */
  void AddEntry (uint32_t index, Ptr<ArpCache::Entry> entry) const;

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
  Ptr<ArpCache::Entry> MakeEntry (Ipv4Address ipv4Address, Address macAddress, std::string status = "ALIVE") const;

  /**
   * \brief Get an Entry object associated with the Ipv4Address of the Ipv4Interface
   *
   * \param ipv4Interface the Ipv4Interface where the Entry will be looked upon
   * \param ipv4Address the Ipv4Address of the ArpCache Entry
   *
   * \returns pointer to the Entry object if found, returns 0 otherwise
   */
  Ptr<ArpCache::Entry> GetEntry (Ptr<Ipv4Interface> ipv4Interface, Ipv4Address ipv4Address) const;

  /**
   * \brief Get an Entry object associated with the Ipv4Address of the Ipv4Interface corresponding to the index value
   *
   * \param index index of the Ipv4Interface on the node.
   * \param	ipv4Address the Ipv4Address of the ArpCache Entry
   */
  Ptr<ArpCache::Entry> GetEntry (uint32_t index, Ipv4Address ipv4Address) const;

  /**
   * \brief Remove the Entry from the Ipv4Interface
   *
   * \param ipv4Interface the Ipv4Interface from which corresponding entry will be removed
   * \param entry the ArpCache Entry to be removed, the pointer to the entry can
   * 			  be obtained using GetEntry() method
   *
   */
  void RemoveEntry (Ptr<Ipv4Interface> ipv4Interface, Ptr<ArpCache::Entry> entry) const;

  /**
   * \brief Remove the Entry from the Ipv4Interface corresponding to the index value
   *
   * \param index index of the Ipv4Interface on the Node
   * \param entry the ArpCache Entry to be remvoed, the pointer to the entry can be
   * 			  obtianed using GetEntry() method
   */
  void RemoveEntry (uint32_t index, Ptr<ArpCache::Entry> entry) const;

  /**
   * \brief Change the status of a Entry.
   *
   * \param entry pointer to the Entry of which status is to be changed. Can be obtained using the GetEntry() method
   * \param status a string representing the status. Possible values:  "ALIVE", "WAIT_REPLY", "DEAD", "PERMANENT"
   *
   */
  void ChangeEntryStatus (Ptr<ArpCache::Entry> entry, std::string status) const;

  /**
   * \brief Change the MAC Address of the Entry.
   *
   * \param entry pointer to the Entry of which MAC address is to be changed. Can be obtained using the GetEntry() method
   * \param macAddress the new value of MAC Address which the Entry will point to
   */
  void ChangeEntryAddress (Ptr<ArpCache::Entry> entry, Address macAddress) const;


private:
  typedef std::map<Ptr<Ipv4Interface>, Ptr<ArpCache>> ArpCacheList;

  Ptr<Node> m_node;
  ArpCacheList m_arpCacheList;
};

class ArpStackHelper
{
public:
  ArpStackHelper (void);

  virtual ~ArpStackHelper (void);

  /**
   * \brief Method will send Arp Request for all the addresses in
   * 	    its sub network and for all received responses it will
   * 	    create PERMANENT ArpCache entries associating the
   * 	    sub network device's IP and MAC address. It can be used
   * 	    to pre-populate ArpCache prior sending any application
   * 	    packets to neglect the delay arising from Arp Request/Reply.
   *
   * \param ipv4Interface the Ipv4Interface pointer for which ArpCache will
   * 			 be populated.
   */
  void PopulateArpCache (Ptr<Ipv4Interface> interface);



};

}

#endif /* NEIGHBOR_TABLE_HELPER_H_ */
