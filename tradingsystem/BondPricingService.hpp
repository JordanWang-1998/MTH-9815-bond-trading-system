/**
 * BondPricingService.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#ifndef BondPricingService_hpp
#define BondPricingService_hpp
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




/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price
{

public:
	// default ctor
	Price() = default;
	
    // ctor for a price
    Price(const T &_product, double _mid, double _bidOfferSpread);

    // Get the product
    const T& GetProduct() const;

    // Get the mid price
    double GetMid() const;

    // Get the bid/offer spread around the mid
    double GetBidOfferSpread() const;
	
	// Print the price info into a vector
	vector<string> print();
	
private:
    const T& product;
    double mid;
    double bidOfferSpread;

};




/* Connector to PricingService */
template<typename T>
class PricingConnector;




/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string,Price<T>>
{
public:
	// default ctor
	PricingService();
	
    // Get data on PricingService given a key
    Price<T>& GetData(string key);
		
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Price<T>& data);

    // Add a listener to PricingService for callbacks on add, remove, and update events for data to PricingService
    void AddListener(ServiceListener<Price<T>>* listener);

    // Get all listeners on PricingService
    const vector<ServiceListener<Price<T>>*>& GetListeners() const;
	
	// Get the pointer to the connector on PricingService
	PricingConnector<T>* GetConnector();
	
private:
	map<string, Price<T>> prices;					// a map of {product identifier -> price}
	vector<ServiceListener<Price<T>>*> listeners;	// all listeners on PricingService
	PricingConnector<T>* connector;					// a pointer to a PricingConnector
};




/* Connector to PricingService */
template<typename T>
class PricingConnector : public Connector<Price<T>>
{
public:
	// ctor
	PricingConnector(PricingService<T>* _service);
	
    // Publish data to the Connector
    void Publish(Price<T>& data);				// Empty
	
	// Subscribe data from the Connector
	void Subscribe(fstream& data_stream);
	
private:
	PricingService<T>* service;					// a pointer to a PricingService
};





#endif
