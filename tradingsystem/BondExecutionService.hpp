/**
 * BondExecutionService.hpp
 * Defines the data types and Service for executions.
 *
 * @author Jordan Wang
 */
#ifndef BondExecutionService_hpp
#define BondExecutionService_hpp
#include "soa.hpp"
#include "my functions.hpp"
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
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{
public:
	// default ctor
    ExecutionOrder() = default;
	
    // ctor for an order
    ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

    // Get the product
    const T& GetProduct() const;

    // Get the order ID
    const string& GetOrderId() const;

    // Get the order type on this order
    OrderType GetOrderType() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity
    long GetHiddenQuantity() const;

    // Get the parent order ID
    const string& GetParentOrderId() const;

    // Is child order?
    bool IsChildOrder() const;
	
	// Convert execution order data -> vector<string> format
	vector<string> GetExecutionOrder_eo2s() const;
	
private:
    T product;
    PricingSide side;
    string orderId;
    OrderType orderType;
    double price;
    double visibleQuantity;
    double hiddenQuantity;
    string parentOrderId;
    bool isChildOrder;
};




/* An algo execution object */
template<typename T>
class AlgoExecution
{
public:
	// default ctor
    AlgoExecution() = default;
	
    // ctor for an order
    AlgoExecution(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder);
    
    // Get the pointer to the ExecutionOrder
    ExecutionOrder<T>* GetExecutionOrder() const;
    
private:
    ExecutionOrder<T>* executionOrder;				// a pointer to an ExecutionOrder
};




/* Listener of AlgoExecution on MarketDataService */
template<typename T>
class AlgoExecutionToMarketDataListener;



/* AlgoExecutionService*/
template<typename T>
class AlgoExecutionService : public Service<string, AlgoExecution<T>>
{
public:
	// default ctor
    AlgoExecutionService();
	
    // Get data on our service given a key
    AlgoExecution<T>& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoExecution<T>& data);
	
    // Add a listener to AlgoExecutionService for callbacks on add, remove, and update events for data to AlgoExecutionService
    void AddListener(ServiceListener<AlgoExecution<T>>* listener);
	
    // Get all listeners on AlgoExecutionService
    const vector<ServiceListener<AlgoExecution<T>>*>& GetListeners() const;
	
	// Get the listener on MarketDataService
    AlgoExecutionToMarketDataListener<T>* GetListener();
	
	// Aggress the top of the book, alternating between bid and offer, 
	// and only aggressing when the spread is at its tightest (i.e. 1/128th)
	// to reduce the cost of crossing the spread
    void ExecuteOrder(OrderBook<T>& orderBook);
	
private:
    map<string, AlgoExecution<T>> algoExecutions;			// a map of {order identifier -> algo execution}
    vector<ServiceListener<AlgoExecution<T>>*> listeners;	// all listeners on AlgoExecutionService
    AlgoExecutionToMarketDataListener<T>* listener;			// a pointer to a listener on MarketDataService
    double minSpread;										// tightest spread (i.e. 1/128th)
    int count;												// algo execution count
};




/* Listener of AlgoExecution on MarketDataService */
template<typename T>
class AlgoExecutionToMarketDataListener : public ServiceListener<OrderBook<T>>
{
public:
    // ctor
	AlgoExecutionToMarketDataListener(AlgoExecutionService<T>* _service);
	
    // Listener callback to process an add event to AlgoExecutionService
    void ProcessAdd(OrderBook<T>& _data);
	
	// Listener callback to process a remove event to AlgoExecutionService
    void ProcessRemove(OrderBook<T>& _data);
    
	// Listener callback to process an update event to AlgoExecutionService
	void ProcessUpdate(OrderBook<T>& _data);
	
private:
    AlgoExecutionService<T>* service;						// a pointer to AlgoExecutionService
};




/* Listener of ExecutionService To AlgoExecutionService */
template<typename T>
class ExecutionToAlgoExecutionListener;




/* ExecutionService */
template<typename T>
class ExecutionService : public Service<string, ExecutionOrder<T>>
{
public:
	// default ctor
	ExecutionService();
	
    // Get data on our service given a key
    ExecutionOrder<T>& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(ExecutionOrder<T>& data);
	
    // Add a listener to ExecutionService for callbacks on add, remove, and update events for data to ExecutionService
	void AddListener(ServiceListener<ExecutionOrder<T>>* listener);
	
    // Get all listeners on ExecutionService
	const vector<ServiceListener<ExecutionOrder<T>>*>& GetListeners() const;
	
	// Get the listener to AlgoExecutionService
    ExecutionToAlgoExecutionListener<T>* GetListener();
	
	// Execute order on the entire size on the market data 
	// for the right side you are executing against 
    void ExecuteOrder(ExecutionOrder<T>& executionOrder);
	
private:
    map<string, ExecutionOrder<T> > executionOrders;			// a map of {execution order identifier -> execution order}
    vector<ServiceListener<ExecutionOrder<T>>*> listeners;		// all listeners on ExecutionService
    ExecutionToAlgoExecutionListener<T>* listener;				// a pointer to a listener to AlgoExecutionService
};




/* Listener of ExecutionService To AlgoExecutionService */
template<typename T>
class ExecutionToAlgoExecutionListener : public ServiceListener<AlgoExecution<T>>
{
public:
	// ctor
    ExecutionToAlgoExecutionListener(ExecutionService<T>* _service);
	
    // Listener callback to process an add event to ExecutionService
    void ProcessAdd(AlgoExecution<T>& _data);
	
    // Listener callback to process a remove event to ExecutionService
    void ProcessRemove(AlgoExecution<T>& _data);
	
    // Listener callback to process an update event to ExecutionService
    void ProcessUpdate(AlgoExecution<T>& _data);
	
private:
    ExecutionService<T>* service;					// a pointer to ExecutionService
};




#endif
