/**
 * BondTradeBookingService.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Jordan Wang
 */
#ifndef BondTradeBookingService_hpp
#define BondTradeBookingService_hpp
#include "soa.hpp"
#include "my functions.hpp"
#include "BondExecutionService.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
using namespace std;




/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade
{
public:
	// default ctor
	Trade() = default;
	
    // ctor for a trade
    Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side);

    // Get the product
    const T& GetProduct() const;

    // Get the trade ID
    const string& GetTradeId() const;

    // Get the mid price
    double GetPrice() const;

    // Get the book
    const string& GetBook() const;

    // Get the quantity
    long GetQuantity() const;

    // Get the side
    Side GetSide() const;

private:
    T product;
    string tradeId;
    double price;
    string book;
    long quantity;
    Side side;
};




/* Subscribe-only Connector to TradeBookingService*/
template<typename T>
class TradeBookingConnector;




/* Listener of TradeBookingService to ExecutionService */
template<typename T>
class TradeBookingToExecutionListener;




/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on trade id.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<string,Trade<T>>
{
public:
	// default ctor
	TradeBookingService();
	
    // Get data on our service given a key
    Trade<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Trade<T>& data);

    // Add a listener to TradeBookingService for callbacks on add, remove, and update events for data to TradeBookingService
    void AddListener(ServiceListener<Trade<T>>* listener);

    // Get all listeners on TradeBookingService
    const vector<ServiceListener<Trade<T>>*>& GetListeners() const;

    // Get the pointer to the connector on TradeBookingService
	TradeBookingConnector<T>* GetConnector();

    // Get the listener to ExecutionService
    TradeBookingToExecutionListener<T>* GetListener();
	
    // Book the trade
    void BookTrade(const Trade<T>& trade);
	
private:
	map<string, Trade<T>> trades;					// a map of {tradeId -> trade}
	vector<ServiceListener<Trade<T>>*> listeners;	// all listeners on TradeBookingService
	TradeBookingConnector<T>* connector;			// a pointer to a TradeBookingConnector
	TradeBookingToExecutionListener<T>* listener;	// listener to ExecutionService
};





/* Connector to TradeBookingService*/
template<typename T>
class TradeBookingConnector : public Connector<Trade<T>>
{
public:
    // ctor
    TradeBookingConnector(TradeBookingService<T>* _service);

    // Publish data to the Connector
    void Publish(Trade<T>& data);			// Empty

    // Subscribe data from the Connector
    void Subscribe(fstream& data_stream);

private:
    TradeBookingService<T>* service;
};




/* Subscribe-only Listener of TradeBookingService to ExecutionService */
template<typename T>
class TradeBookingToExecutionListener : public ServiceListener<ExecutionOrder<T>>
{
public:
	// ctor
	TradeBookingToExecutionListener(TradeBookingService<T>* _service);
	
    // Listener callback to process an add event to TradeBookingService
    void ProcessAdd(ExecutionOrder<T> &data);

    // Listener callback to process a remove event to TradeBookingService
    void ProcessRemove(ExecutionOrder<T> &data);

    // Listener callback to process an update event to TradeBookingService
    void ProcessUpdate(ExecutionOrder<T> &data);
	
private:
	TradeBookingService<T>* service;		// a pointer to TradeBookingService
	int count;								// trade count
};




#endif

