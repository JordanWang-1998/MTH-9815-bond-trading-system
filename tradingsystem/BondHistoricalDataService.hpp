/**
 * BondHistoricalDataService.hpp
 * BondHistoricalDataService.hpp
 *
 * @author Jordan Wang
 * Defines the data types and Service for historical data.
 *
 * @author Jordan Wang
 */

#ifndef BondHistoricalDataService_hpp
#define BondHistoricalDataService_hpp
#include "soa.hpp"
#include "my functions.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>
using namespace std;





/* Publish-only Connector to HistoricalDataService */
template<typename V>
class HistoricalDataConnector;

/* Listener of HistoricalDataService in general */
template<typename V>
class HistoricalDataListener;




/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename V>
class HistoricalDataService : Service<string, V>
{
public:	
	// ctor
    HistoricalDataService(HistoricalDataType _type);
	
    // Get data on our service given a key
    V& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(V& data);
	
    // Add a listener to HistoricalDataService for callbacks on add, remove, and update events for data to HistoricalDataService
    void AddListener(ServiceListener<V>* listener);
	
    // Get all listeners on HistoricalDataService
    const vector<ServiceListener<V>*>& GetListeners() const;
	
	// Get the connector to HistoricalDataService
    HistoricalDataConnector<V>* GetConnector();
	
	// Get the listener in general
    ServiceListener<V>* GetListener();
	
	// Get the historical data type to persiste
    HistoricalDataType GetHistoricalDataType() const;
	
    // Persist data to a store
    void PersistData(string persistKey, const V& data);

private:
    map<string, V> historicalDatas;			// a map of {historical data identifier -> historical data}
    vector<ServiceListener<V>*> listeners;	// all listeners on HistoricalDataService
    HistoricalDataConnector<V>* connector;	// a pointer to HistoricalDataConnector
    HistoricalDataListener<V>* listener;	// a pointer to a listener in general
    HistoricalDataType type;				// historical data type to persist
};




/* Publish-only Connector to HistoricalDataService */
template<typename V>
class HistoricalDataConnector : public Connector<V>
{
public:
	// ctor
    HistoricalDataConnector(HistoricalDataService<V>* _service);
	
    // Publish data to the Connector
    void Publish(V& _data);
	
	// Subscribe data from the Connector
    void Subscribe(fstream& data_stream);		// Empty
	
private:
    HistoricalDataService<V>* service;			// a pointer to HistoricalDataService
};



/* Listener of HistoricalDataService in general */
template<typename V>
class HistoricalDataListener : public ServiceListener<V>
{
public:
	// ctor
    HistoricalDataListener(HistoricalDataService<V>* _service);
	
    // Listener callback to process an add event to HistoricalDataService
    void ProcessAdd(V& _data);
	
    // Listener callback to process a remove event to HistoricalDataService
    void ProcessRemove(V& _data);
	
    // Listener callback to process an update event to HistoricalDataService
    void ProcessUpdate(V& _data);
	
private:
    HistoricalDataService<V>* service;			// a pointer to HistoricalDataService
};




#endif