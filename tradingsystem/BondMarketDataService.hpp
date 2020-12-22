/**
 * BondMarketDataService.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Jordan Wang
 */
#ifndef BondMarketDataService_hpp
#define BondMarketDataService_hpp
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
 * A market data order with price, quantity, and side.
 */
class Order
{

public:
	// default ctor
	Order() = default;
	
    // ctor for an order
    Order(double _price, long _quantity, PricingSide _side);

    // Get the price on the order
    double GetPrice() const;

    // Get the quantity on the order
    long GetQuantity() const;

    // Get the side on the order
    PricingSide GetSide() const;

private:
    double price;
    long quantity;
    PricingSide side;
};




/**
 * Class representing a bid and offer order
 */
class BidOffer
{

public:
	// default ctor
	BidOffer() = default;
	
    // ctor for bid/offer
    BidOffer(const Order &_bidOrder, const Order &_offerOrder);

    // Get the bid order
    const Order& GetBidOrder() const;

    // Get the offer order
    const Order& GetOfferOrder() const;

private:
    Order bidOrder;
    Order offerOrder;
};




/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template<typename T>
class OrderBook
{

public:
	// default ctor
	OrderBook() = default;
	
    // ctor for the order book
    OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack);

    // Get the product
    const T& GetProduct() const;

    // Get the bid stack
    const vector<Order>& GetBidStack() const;

    // Get the offer stack
    const vector<Order>& GetOfferStack() const;
	
	// Get the best bid/offer order
	const BidOffer& GetBestBidOffer() const;
	
private:
    T product;
    vector<Order> bidStack;
    vector<Order> offerStack;
};




/* Subscribe-only Connector to BondMarketDataService */
template<typename T>
class MarketDataConnector;




/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class MarketDataService : public Service<string,OrderBook<T>>
{
public:
	// default ctor
	MarketDataService();
	
    // Get data on our service given a key
    OrderBook<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(OrderBook<T>& data);

    // Add a listener to MarketDataService for callbacks on add, remove, and update events for data to MarketDataService
    void AddListener(ServiceListener<OrderBook<T>>* listener);

    // Get all listeners on MarketDataService
	const vector<ServiceListener<OrderBook<T>>*>& GetListeners() const;
	
	// Get the pointer to the connector on MarketDataService
	MarketDataConnector<T>* GetConnector();
	
	// Get the # of bid/offer levels in the order book
	int GetOrderBookLevels() const;
	
    // Get the best bid/offer order
    const BidOffer& GetBestBidOffer(const string &productId);

    // Aggregate the order book
    const OrderBook<T>& AggregateDepth(const string &productId);

private:
	map<string, OrderBook<T>> orderBooks;				// a map of {order identifier -> order book}
    vector<ServiceListener<OrderBook<T>>*> listeners;	// all listeners on BondMarketDataService
    MarketDataConnector<T>* connector;					// a pointer to a MarketDataConnector
    int orderBookLevels;								// # of bid/offer levels in the order book
};




/* Subscribe-only Connector to BondMarketDataService */
template<typename T>
class MarketDataConnector
{
public:
	// ctor
    MarketDataConnector(MarketDataService<T>* _service) { service = _service; }

    // Publish data to the Connector
    void Publish(OrderBook<T>& _data); 					// Empty
	
    // Subscribe data from the Connector
    void Subscribe(fstream& data_stream);
	
private:
    MarketDataService<T>* service;						// a pointer to MarketDataService
}




#endif
