/**
 * This file contains declares the simple BitcoinNode class.
 */

#ifndef BITCOIN_NODE_H
#define BITCOIN_NODE_H

#include <algorithm>
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "bitcoin.h"
#include "ns3/boolean.h"
#include "../../rapidjson/document.h"
#include "../../rapidjson/writer.h"
#include "../../rapidjson/stringbuffer.h"

namespace ns3 {

class Address;
class Socket;
class Packet;


class BitcoinNode : public Application
{
public:

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  BitcoinNode (void);

  virtual ~BitcoinNode (void);

  /**
   * \return pointer to listening socket
   */
  Ptr<Socket> GetListeningSocket (void) const;


  /**
   * \return a vector containing the addresses of peers
   */
  std::vector<Ipv4Address> GetPeersAddresses (void) const;


  /**
   * \brief Set the addresses of peers
   * \param peers the reference of a vector containing the Ipv4 addresses of peers
   */
  void SetPeersAddresses (const std::vector<Ipv4Address> &peers);

  /**
   * \brief set the download speeds of peers
   * \param peersDownloadSpeeds the reference of a map containing the Ipv4 addresses of peers and their corresponding download speed
   */
  void SetPeersDownloadSpeeds (const std::map<Ipv4Address, double> &peersDownloadSpeeds);

  /**
   * \brief Set the upload speeds of peers
   * \param peersUploadSpeeds the reference of a map containing the Ipv4 addresses of peers and their corresponding upload speed
  */
  void SetPeersUploadSpeeds (const std::map<Ipv4Address, double> &peersUploadSpeeds);

  /**
   * \brief Set the internet speeds of the node
   * \param internetSpeeds a struct containing the download and upload speed of the node
   */
  void SetNodeInternetSpeeds (const nodeInternetSpeeds &internetSpeeds);

  /**
   * \brief Set the node statistics
   * \param nodeStats a reference to a nodeStatistics struct
   */
  void SetNodeStats(nodeStatistics *nodeStats);
  void SetProperties(uint64_t timeToRun, enum ModeType mode,
    int systemId, std::vector<Ipv4Address> outPeers, ProtocolSettings protocolSettings);

protected:
  virtual void DoDispose (void);           // inherited from Application base class.

  virtual void StartApplication (void);    // Called at time specified by Start
  virtual void StopApplication (void);     // Called at time specified by Stop

  /**
   * \brief Handle a packet received by the application
   * \param socket the receiving socket
   */
  void HandleRead (Ptr<Socket> socket);

  /**
   * \brief Handle an incoming connection
   * \param socket the incoming connection socket
   * \param from the address the connection is from
   */
  void HandleAccept (Ptr<Socket> socket, const Address& from);

  /**
   * \brief Handle an connection close
   * \param socket the connected socket
   */
  void HandlePeerClose (Ptr<Socket> socket);

  /**
   * \brief Handle an connection error
   * \param socket the connected socket
   */
  void HandlePeerError (Ptr<Socket> socket);


  void LogTime(void);

  int PoissonNextSend(int averageIntervalSeconds);
  int PoissonNextSendIncoming(int averageIntervalSeconds);

  void ReconcileWithPeer(void);
  Ipv4Address ChooseFromPeers(std::vector<Ipv4Address> peers);
  void UpdatePreferredPeersList(void);
  void ValidateNodeFilters(void);
  void UpdateFilterBegin(Ipv4Address& peer, uint32_t newVal);
  void UpdateFilterEnd(Ipv4Address& peer, uint32_t newVal);
  void AnnounceMode(void);

  void ScheduleNextTransactionEvent(void);
  void EmitTransaction(void);

  void SaveTxData(int txId, Ipv4Address from, int hopNumber);
  void AddToReconciliationSets(int txId, Ipv4Address from);
  void RemoveFromReconciliationSets(int txId, Ipv4Address peer);

  void AdvertiseTransactionInvWrapper (Address from, const int transactionHash, int hopNumber);
  void AdvertiseNewTransactionInvStandard (Ipv4Address from, const int transactionHash, int hopNumber);
  void AdvertiseNewTransactionInv (Ipv4Address from, const int transactionHash, int hopNumber, std::vector<Ipv4Address> peers, int order);

  void SendInvToNode(Ipv4Address receiver, const int transactionHash, int hopNumber);

  void RespondToReconciliationRequest(Ipv4Address from);

  void RotateDandelionDestinations();
  /**
   * \brief Sends a message to a peer
   * \param receivedMessage the type of the received message
   * \param responseMessage the type of the response message
   * \param d the rapidjson document containing the info of the outgoing message
   * \param outgoingSocket the socket of the peer
   */
  void SendMessage(enum Messages receivedMessage,  enum Messages responseMessage, rapidjson::Document &d, Ptr<Socket> outgoingSocket);

  /**
   * \brief Sends a message to a peer
   * \param receivedMessage the type of the received message
   * \param responseMessage the type of the response message
   * \param d the rapidjson document containing the info of the outgoing message
   * \param outgoingAddress the Address of the peer
   */
  void SendMessage(enum Messages receivedMessage,  enum Messages responseMessage, rapidjson::Document &d, Address &outgoingAddress);

  /**
   * \brief Sends a message to a peer
   * \param receivedMessage the type of the received message
   * \param responseMessage the type of the response message
   * \param packet a string containing the info of the outgoing message
   * \param outgoingAddress the Address of the peer
   */
  void SendMessage(enum Messages receivedMessage,  enum Messages responseMessage, std::string packet, Address &outgoingAddress);

  // In the case of TCP, each socket accept returns a new socket, so the
  // listening socket is stored separately from the accepted sockets
  Ptr<Socket>     m_socket;                           //!< Listening socket
  Address         m_local;                            //!< Local address to bind to
  TypeId          m_tid;                              //!< Protocol TypeId
  int             m_numberOfPeers;                    //!< Number of node's peers
  int             m_numInvsSent;                      //!< keep track of number of INVs sent to all peers - used for "Preferred Destinations" routing
  double          m_overlap;                          //!< Overlap of filters
  Time            m_invTimeoutMinutes;                //!< The block timeout in minutes
  double          m_downloadSpeed;                    //!< The download speed of the node in Bytes/s
  double          m_uploadSpeed;                      //!< The upload speed of the node in Bytes/s
  double          m_averageTransactionSize;           //!< The average transaction size. Needed for compressed blocks
  uint m_fixedTxTimeGeneration;

  int m_systemId;

  std::map<ns3::Ipv4Address, uint32_t> filterBegin;        //!< The start of the filter for each peer
  std::map<ns3::Ipv4Address, uint32_t> filterEnd;          //!< The end of the filter for each peer
  std::map<Ipv4Address, ModeType> peersMode;

  uint lastTxId;
  std::vector<int> knownTxHashes;

  uint32_t sentOriginalInvs;
  uint32_t retransmittedInvs;
  uint32_t gotGetData;


  double m_prevA;

  std::map<Ipv4Address, double>     m_prevA_per_peer;


  std::map<int, std::vector<Ipv4Address>>     peersKnowTx;

  std::map<Ipv4Address, std::vector<int>>     m_peerReconciliationSets;         //!< Set of all txs we've seen, to be reconciled with peers
  std::list<Ipv4Address>                              m_reconcilePeers;                 //!< Queue holding peers with which we will reconcile
  std::vector<Ipv4Address>                            m_peersAddresses;                 //!< The addresses of peers
  std::map<Ipv4Address, double>                       m_peersDownloadSpeeds;            //!< The peersDownloadSpeeds of channels
  std::map<Ipv4Address, double>                       m_peersUploadSpeeds;              //!< The peersUploadSpeeds of channels
  std::map<Ipv4Address, peerStatistics>               m_peerStatistics;                 //!< map holding message statistics for each of this node's peers. Used to order peers by some metric.
  std::vector<Ipv4Address>                            m_preferredPeers;                 //!< List of peers the node deems "preferred" by some metric
  std::map<Ipv4Address, Ptr<Socket>>                  m_peersSockets;                   //!< The sockets of peers
  std::map<std::string, EventId>                      m_invTimeouts;                    //!< map holding the event timeouts of inv messages
  std::map<std::string, EventId>                      m_chunkTimeouts;                  //!< map holding the event timeouts of chunk messages
  std::map<Address, std::string>                      m_bufferedData;                   //!< map holding the buffered data from previous handleRead events
  nodeStatistics                                     *m_nodeStats;                      //!< struct holding the node stats
  enum ModeType                                       m_mode;

  std::vector<int>                      loopHistory;                   // tx announcement result as a loop

  ProtocolSettings m_protocolSettings;

  uint64_t heardTotal;
  std::vector<int> firstTimeHops;
  bool txCreator;

  int voidReconciliations;

  std::vector<Ipv4Address> m_outPeers;
  std::vector<Ipv4Address> m_inPeers;
  // std::map<Ipv4Address, std::vector<Ipv4Address>> m_dandelionDestinations;
  // std::map<Ipv4Address, std::vector<int>> m_reconciliationHistory;



  Address spy;
  uint64_t       m_timeToRun;

  const int       m_bitcoinPort;               //!< 8333
  const int       m_secondsPerMin;             //!< 60
  const int       m_countBytes;                //!< The size of count variable in messages, 4 Bytes
  const int       m_bitcoinMessageHeader;      //!< The size of the bitcoin Message Header, 90 Bytes, including both the bitcoinMessageHeaders and the other protocol headers (TCP, IP, Ethernet)
  const int       m_inventorySizeBytes;        //!< The size of inventories in INV messages, 36 Bytes
  const int       m_getHeadersSizeBytes;       //!< The size of the GET_HEADERS message, 72 Bytes
  const int       m_headersSizeBytes;          //!< 81 Bytes


  int64_t lastInvScheduled;

  /// Traced Callback: received packets, source address.
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;


  // every 30 seconds
  const std::vector<double> transactionRates = {9, 2.66, 7, 2.033, 14, 14, 3.6, 2.45, 2.067, 9, 2.067};

};

} // namespace ns3

#endif /* BITCOIN_NODE_H */
