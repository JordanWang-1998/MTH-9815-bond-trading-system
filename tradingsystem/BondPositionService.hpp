/**
 * BondPositionService.hpp
 * Defines the data types and Service for positions.
 *
 * @author Jordan Wang
 */
#ifndef BondPositionService_hpp
#define BondPositionService_hpp
#include "soa.hpp"
#include "my functions.hpp"
#include "BondTradeBookingService.hpp"
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
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:
	// default ctor
	Position() = default;
	
    // ctor for a position
    Position(const T &_product);

    // Get the product
    const T& GetProduct() const;

    // Get the position quantity
    long GetPosition(string &book);

    // Get the aggregate position
    long GetAggregatePosition();
	
	// Get a map of {product identifier -> position quantity}
	map<string, long> GetPositions();
	
	// Convert position data -> vector<string> format
	vector<string> GetPositions_p2s();
	
	// Add a position to a book
	void AddPosition(string& book, long position);
	
private:
    T product;
    map<string,long> positions;
};




/* Listener of BondPositionService to BondTradeBookingService */
template<typename T>
class PositionToTradeBookingListener;




/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string,Position<T>>
{
public:
	// default ctor
	PositionService();
	
    // Get data on our service given a key
    Position<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Position<T>& data);

    // Add a listener to BondPositionService for callbacks on add, remove, and update events for data to BondPositionService
    void AddListener(ServiceListener<Position<T>>* listener);

    // Get all listeners on BondPositionService
	const vector<ServiceListener<Position<T>>*>& GetListeners() const;
	
	// Get the listener to BondTradeBookingService
	PositionToTradeBookingListener<T>* GetListener();
	
    // Add a trade to the service
    void AddTrade(const Trade<T>& trade);

private:
	map<string, Position<T>> positions;
	vector<ServiceListener<Position<T>>*> listeners;
	PositionToTradeBookingListener<T>* listener;
};




/* Listener of BondPositionService to BondTradeBookingService */
template<typename T>
class PositionToTradeBookingListener : public ServiceListener<Trade<T>>
{
public:
	// ctor
    PositionToTradeBookingListener(PositionService<T>* _service);
	
    // Listener callback to process an add event to BondPositionService
    void ProcessAdd(Trade<T>& _data);
	
    // Listener callback to process a remove event to BondPositionService
    void ProcessRemove(Trade<T>& _data);
	
    // Listener callback to process an update event to BondPositionService
    void ProcessUpdate(Trade<T>& _data);
	
private:
    PositionService<T>* service;
};




#endif
