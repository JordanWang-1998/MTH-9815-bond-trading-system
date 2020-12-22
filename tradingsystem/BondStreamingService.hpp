/**
 * BondStreamingService.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Jordan Wang
 */
#ifndef BondStreamingService_hpp
#define BondStreamingService_hpp
#include "soa.hpp"
#include "my functions.hpp"
#include "BondPricingService.hpp"
#include "BondMarketDataService.hpp"
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




/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder
{
public:
	// default ctor
    PriceStreamOrder() = default;
	
    // ctor for an order
    PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side);

    // The side on this order
    PricingSide GetSide() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity on this order
    long GetHiddenQuantity() const;
	
	// Get the bid / offer side
    PricingSide GetSide() const;
	
	// Convert price stream order data -> vector<string> format
	vector<string> GetPriceStreamOrder_pso2s() const;
	
private:
    double price;
    long visibleQuantity;
    long hiddenQuantity;
    PricingSide side;
};




/**
 * Price Stream with a two-way market.
 * Type T is the product type.
 */
template<typename T>
class PriceStream
{

public:
	// default ctor
	PriceStream() = default;
	
    // ctor
    PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);

    // Get the product
    const T& GetProduct() const;

    // Get the bid order
    const PriceStreamOrder& GetBidOrder() const;

    // Get the offer order
    const PriceStreamOrder& GetOfferOrder() const;
	
	// Convert price stream data -> vector<string> format
	vector<string> GetPriceStream_ps2s() const;
	
private:
    T product;
    PriceStreamOrder bidOrder; 
    PriceStreamOrder offerOrder;
};




/* AlgoStream */
template<typename T>
class AlgoStream
{
public:
	// default ctor;
    AlgoStream() = default;
	
	// ctor
    AlgoStream(const T& _product, const PriceStreamOrder& _bidOrder, const PriceStreamOrder& _offerOrder);
    
	// Get the pointer to PriceStream
	PriceStream<T>* GetPriceStream() const;
	
private:
    PriceStream<T>* priceStream;				// a pointer to PriceStream
};




/* Listener of AlgoStreamingService to PricingService */
template<typename T>
class AlgoStreamingToPricingListener;




/* AloStreamingService */

template<typename T>
class AlgoStreamingService : public Service<string, AlgoStream<T> >
{
public:
	// default ctor
    AlgoStreamingService();
	
    // Get data on our service given a key
    AlgoStream<T>& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoStream<T>& data);
    
    // Add a listener to AlgoStreamingService for callbacks on add, remove, and update events for data to AlgoStreamingService
	void AddListener(ServiceListener<AlgoStream<T>>* listener);
		
    // Get all listeners on AlgoStreamingService
	const vector<ServiceListener<AlgoStream<T>>*>& GetListeners() const;
	
	// Get the listener to PricingService
	AlgoStreamingToPricingListener<T>* GetListener();
	
	// Send bid/offer prices
	// Alternate visible sizes between 1000000 and 2000000 on subsequent updates for both sizes
    // Hidden size should be twice the visible size at all times
	void PublishPrice(Price<T>& price);
	
private:
    map<string, AlgoStream<T>> algoStreams;					// a map of {}
    vector<ServiceListener<AlgoStream<T>>*> listeners;		// all listeners on AlgoStreamingService
    AlgoStreamingToPricingListener<T>* listener;			// a pointer to a listener to PricingService
    long count;												// stream count
};




/* Listener of AlgoStreamingService to PricingService */
template<typename T>
class AlgoStreamingToPricingListener : public ServiceListener<Price<T>>
{
public:
    // ctor
	AlgoStreamingToPricingListener(AlgoStreamingService<T>* _service);
	
    // Listener callback to process an add event to AlgoStreamingService
	void ProcessAdd(Price<T>& _data);
	
    // Listener callback to process a remove event to AlgoStreamingService
    void ProcessRemove(Price<T>& _data);
	
    // Listener callback to process an update event to AlgoStreamingService
    void ProcessUpdate(Price<T>& _data);
	
private:
    AlgoStreamingService<T>* service;				// a pointer to AlgoStreamingService
};




/* Listener of StreamingService to AlgoStreamingService */
template<typename T>
class StreamingToAlgoStreamingListener;




/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingService : public Service<string,PriceStream<T>>
{

public:
	// default ctor
    StreamingService();
	
    // Get data on our service given a key
    PriceStream<T>& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PriceStream<T>& data);
	
    // Add a listener to StreamingService for callbacks on add, remove, and update events for data to StreamingService
    void AddListener(ServiceListener<PriceStream<T>>* listener);
   
    // Get all listeners on StreamingService
	const vector<ServiceListener<PriceStream<T>>*>& GetListeners() const;
    
	// Get the listener to AlgoStreamingService
	ServiceListener<AlgoStream<T>>* GetListener(); 
	
    // Publish two-way prices
    void PublishPrice(const PriceStream<T>& priceStream);

private:
    map<string, PriceStream<T> > priceStreams;				// a map of {product identifier -> price stream}
    vector<ServiceListener<PriceStream<T>>*> listeners;		// all listeners on StreamingService
    StreamingToAlgoStreamingListener<T>* listener;			// a pointer to a listener to 
};




/* Listener of StreamingService to AlgoStreamingService */
template<typename T>
class StreamingToAlgoStreamingListener : public ServiceListener<AlgoStream<T>>
{
public:
	// ctor
    StreamingToAlgoStreamingListener(StreamingService<T>* _service);
	
    // Listener callback to process an add event to StreamingService
    void ProcessAdd(AlgoStream<T>& _data);
	
    // Listener callback to process an add event to StreamingService
    void ProcessRemove(AlgoStream<T>& _data);
	
    // Listener callback to process an update event to StreamingService
    void ProcessUpdate(AlgoStream<T>& _data);
	
private:
    StreamingService<T>* service;					// a pointer to StreamingService
};




#endif
