/**
 * This file contains the definitions of the functions declared in bitcoin-node-helper.h
 */

#include "bitcoin-node-helper.h"
#include "ns3/string.h"
#include "ns3/inet-socket-address.h"
#include "ns3/names.h"
#include "../model/bitcoin-node.h"

namespace ns3 {

BitcoinNodeHelper::BitcoinNodeHelper (std::string netProtocol, Address address, std::vector<Ipv4Address> &peers,
                                      std::map<Ipv4Address, double> &peersDownloadSpeeds, std::map<Ipv4Address, double> &peersUploadSpeeds,
                                      nodeInternetSpeeds &internetSpeeds, nodeStatistics *stats, ProtocolSettings protocolSettings)
{
  m_factory.SetTypeId ("ns3::BitcoinNode");
  commonConstructor (netProtocol, address, peers, peersDownloadSpeeds, peersUploadSpeeds, internetSpeeds, stats,
    protocolSettings);
}

BitcoinNodeHelper::BitcoinNodeHelper (void)
{
}

void
BitcoinNodeHelper::commonConstructor(std::string netProtocol, Address address, std::vector<Ipv4Address> &peers,
                                     std::map<Ipv4Address, double> &peersDownloadSpeeds, std::map<Ipv4Address, double> &peersUploadSpeeds,
                                     nodeInternetSpeeds &internetSpeeds, nodeStatistics *stats, ProtocolSettings protocolSettings)
{

  m_netProtocol = netProtocol;
  m_address = address;
  m_peersAddresses = peers;
  m_peersDownloadSpeeds = peersDownloadSpeeds;
  m_peersUploadSpeeds = peersUploadSpeeds;
  m_internetSpeeds = internetSpeeds;
  m_nodeStats = stats;
  m_protocolSettings = protocolSettings;
  m_factory.Set ("Protocol", StringValue (m_netProtocol));
  m_factory.Set ("Local", AddressValue (m_address));

}

void
BitcoinNodeHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
BitcoinNodeHelper::Install (Ptr<Node> node)
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
BitcoinNodeHelper::Install (std::string nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
BitcoinNodeHelper::Install (NodeContainer c)
{

  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
  {
    apps.Add (InstallPriv (*i));
  }

  return apps;
}

Ptr<Application>
BitcoinNodeHelper::InstallPriv (Ptr<Node> node)
{
  Ptr<BitcoinNode> app = m_factory.Create<BitcoinNode> ();
  app->SetPeersAddresses(m_peersAddresses);
  app->SetPeersDownloadSpeeds(m_peersDownloadSpeeds);
  app->SetPeersUploadSpeeds(m_peersUploadSpeeds);
  app->SetNodeInternetSpeeds(m_internetSpeeds);
  app->SetNodeStats(m_nodeStats);
  app->SetProperties(m_timeToRun, m_mode, m_systemId, m_outPeers, m_protocolSettings);

  node->AddApplication (app);

  return app;
}

void
BitcoinNodeHelper::SetPeersAddresses (std::vector<Ipv4Address> &peersAddresses)
{
  m_peersAddresses = peersAddresses;
}

void
BitcoinNodeHelper::SetPeersDownloadSpeeds (std::map<Ipv4Address, double> &peersDownloadSpeeds)
{
  m_peersDownloadSpeeds = peersDownloadSpeeds;
}

void
BitcoinNodeHelper::SetPeersUploadSpeeds (std::map<Ipv4Address, double> &peersUploadSpeeds)
{
  m_peersUploadSpeeds = peersUploadSpeeds;
}


void
BitcoinNodeHelper::SetNodeInternetSpeeds (nodeInternetSpeeds &internetSpeeds)
{
  m_internetSpeeds = internetSpeeds;
}

void
BitcoinNodeHelper::SetNodeStats (nodeStatistics *nodeStats)
{
  m_nodeStats = nodeStats;
}

void
BitcoinNodeHelper::SetProperties (uint64_t timeToRun, enum ModeType mode, int systemId,
    std::vector<Ipv4Address> outPeers)
{
  m_timeToRun = timeToRun;
  m_mode = mode;
  m_systemId = systemId;
  m_outPeers = outPeers;
}


} // namespace ns3
