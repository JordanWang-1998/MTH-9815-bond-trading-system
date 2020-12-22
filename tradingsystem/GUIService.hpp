/**
 * GUIService.hpp
 *
 * @author Jordan Wang
 */

#ifndef GUIService_hpp
#define GUIService_hpp
#include "soa.hpp"
#include "my functions.hpp"
#include "products.hpp"
#include "BondPricingService.hpp"
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




/* Publish-only Connector to GUIService */
template<typename T>
class GUIConnector;

/* Listener of GUIService to PricingService */
template<typename T>
class GUIToPricingListener; 




/* GUIService */

template<typename T>
class GUIService : Service<string, Price<T>>
{
public:
	// default ctor
    GUIService();
	
    // Get data on our service given a key
    Price<T>& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Price<T>& data);
	
    // Add a listener to GUIService for callbacks on add, remove, and update events for data to GUIService
    void AddListener(ServiceListener<Price<T>>* listener);
	
    // Get all listeners on GUIService
    const vector<ServiceListener<Price<T>>*>& GetListeners() const;

	// Get the connector to GUIService
	GUIConnector<T>* GetConnector();
	
	// Get the listener to PricingSerivice
    GUIToPricingListener<T>* GetListener();
	
	// Get throttle
    int GetThrottle();
	
	// Get millisec
    int GetMillisecond();
	
	// Set millisec
    void SetMillisecond(int _millisecond);
	
private:
    map<string, Price<T>> guis;						// a map of {product identifier -> price value}
    vector<ServiceListener<Price<T>>*> listeners;	// all listeners on GUIService
    GUIConnector<T>* connector;						// a pointer to GUIConnector
    GUIToPricingListener<T>* listener;				// a pointer to a listener to PricingService
    int throttle;									// 300ms throttle
    int millisecond;								// millisecond (0~300)
};




/* Publish-only Connector to GUIService */
template<typename T>
class GUIConnector : public Connector<Price<T>>
{
public:
	// ctor
    GUIConnector(GUIService<T>* _service);
	
    // Publish data to the Connector
    void Publish(Price<T>& _data);
	
	// Subscribe data from the Connector
    void Subscribe(fstream& data_stream);		// Empty
	
private:
    GUIService<T>* service;						// a pointer to GUIService
};




/* Listener of GUIService to PricingService */
template<typename T>
class GUIToPricingListener : public ServiceListener<Price<T>>
{
public:
	// ctor
    GUIToPricingListener(GUIService<T>* _service);
	
    // Listener callback to process an add event to GUIService
    void ProcessAdd(Price<T>& _data);
	
    // Listener callback to process a remove event to GUIService
    void ProcessRemove(Price<T>& _data);
	
    // Listener callback to process an update event to GUIService
    void ProcessUpdate(Price<T>& _data);
	
private:
    GUIService<T>* service;				// a pointer to GUIService
};




#endif