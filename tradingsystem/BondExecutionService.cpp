/**
 * BondExecutionService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "BondExecutionService.hpp"
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




template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
    product(_product)
{
    side = _side;
    orderId = _orderId;
    orderType = _orderType;
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    parentOrderId = _parentOrderId;
    isChildOrder = _isChildOrder;
}

template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
    return product;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
    return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
    return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
    return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
    return visibleQuantity;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
    return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
    return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
    return isChildOrder;
}

template<typename T>
vector<string> ExecutionOrder<T>::GetExecutionOrder_eo2s() const
{
	return GetExecutionOrder_eo2s<T>(product, side, orderId, orderType, price, visibleQuantity, hiddenQuantity, parentOrderId, isChildOrder); 
}




template<typename T>
AlgoExecution<T>::AlgoExecution(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder)
{
    executionOrder = new ExecutionOrder<T>(_product, _side, _orderId, _orderType, _price, _visibleQuantity, _hiddenQuantity, _parentOrderId, _isChildOrder);
}

template<typename T>
ExecutionOrder<T>* AlgoExecution<T>::GetExecutionOrder() const
{
    return executionOrder;
}




template<typename T>
AlgoExecutionService<T>::AlgoExecutionService()
{
    algoExecutions = map<string, AlgoExecution<T>>();
    listeners = vector<ServiceListener<AlgoExecution<T>>*>();
    listener = new AlgoExecutionToMarketDataListener<T>(this);
    minSpread = 1.0 / 128.0;
    count = 0;
}

template<typename T>
AlgoExecution<T>& AlgoExecutionService<T>::GetData(string key)
{ 
	return algoExecutions[key]; 
}

template<typename T>
void AlgoExecutionService<T>::OnMessage(AlgoExecution<T>& data)
{
	ExecutionOrder<T>* executionOrder = data.GetExecutionOrder();
	T curr_product = executionOrder->GetProduct();
	string productId = curr_product.GetParentOrderId();
	algoExecutions[productId] = data;
}

template<typename T>
void AlgoExecutionService<T>::AddListener(ServiceListener<AlgoExecution<T>>* listener)
{
	listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<AlgoExecution<T>>*>& AlgoExecutionService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
AlgoExecutionToMarketDataListener<T>* AlgoExecutionService<T>::GetListener()
{
	return listener;
}

template<typename T>
void AlgoExecutionService<T>::ExecuteOrder(OrderBook<T>& orderBook)
{
	T curr_product = orderBook.GetProduct();
	string productId = product.GetProductId();
	string orderId = GetTime();		// Use current timestamp as order ID
	
	BidOffer bidOffer = orderBook.GetBidOffer();
	Order bidOrder = bidOffer.GetBidOrder();
	double bidPrice = bidOrder.GetPrice();
	long bidQuantity = bidOrder.GetQuantity();
	Order offerOrder = bidOffer.GetOfferOrder();
	double offerPrice = offerOrder.GetPrice();
	long offerQuantity = offerOrder.GetQuantity();
	
	// Only aggress at the tightest spread (i.e. 1/128th)
	if (offerPrice - bidPrice == spread)
	{
		// Alternate between bid and offer
		if (count % 2 == 0)
		{
			double price = bidPrice;
			long quantity = bidQuantity;
			PricingSide side = BID;
		}
		else
		{
			double price = offerPrice;
			long quantity = offerQuantity;
			PricingSide side = OFFER;
		}
		count++;
		
        AlgoExecution<T> algoExecution(product, side, orderId, MARKET, price, quantity, 0, "", false);
        algoExecutions[productId] = algoExecution;
		
        for (vector<ServiceListener<AlgoExecution<T>>*>::iterator ae_it = listeners.begin(); ae_it != listeners.end(); ae_it++)
            (*ae_it)->ProcessAdd(_algoExecution);
	}
}




template<typename T>
AlgoExecutionToMarketDataListener<T>::AlgoExecutionToMarketDataListener(AlgoExecutionService<T>* _service)
{
	service = _service;
}

template<typename T>
void AlgoExecutionToMarketDataListener<T>::ProcessAdd(OrderBook<T>& _data)
{
	service->ExecuteOrder(_data);
}




template<typename T>
ExecutionService<T>::ExecutionService()
{
    executionOrders = map<string, ExecutionOrder<T>>();
    listeners = vector<ServiceListener<ExecutionOrder<T>>*>();
    listener = new ExecutionToAlgoExecutionListener<T>(this);
}

template<typename T>
ExecutionOrder<T>& ExecutionService<T>::GetData(string key) 
{ 
	return executionOrders[key]; 
}

template<typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T>& data) 
{ 
	T curr_product = data.GetProduct();
	string productId = curr_product.GetProductId();
	executionOrders[productId] = data;
}

template<typename T>
void ExecutionService<T>:: AddListener(ServiceListener<ExecutionOrder<T>>* listener) 
{ 
	listeners.push_back(listener); 
}

template<typename T>
const vector<ServiceListener<ExecutionOrder<T>>*>& ExecutionService<T>::GetListeners() const 
{ 
	return listeners; 
}

template<typename T>
ExecutionToAlgoExecutionListener<T>* ExecutionService<T>::GetListener() 
{ 
	return listener; 
}

template<typename T>
void ExecutionService<T>::ExecuteOrder(ExecutionOrder<T>& executionOrder)
{
	T curr_product = executionOrder.GetProduct();
    string productId = curr_product.GetProductId();
    executionOrders[productId] = executionOrder;
    
    for (vector<ServiceListener<ExecutionOrder<T>>*>::iterator eo_it = listeners.begin(); eo_it != listeners.end(); eo_it++)
        (*eo_it)->ProcessAdd(executionOrder);
}




template<typename T>
ExecutionToAlgoExecutionListener<T>::ExecutionToAlgoExecutionListener(ExecutionService<T>* _service) 
{ 
	service = _service; 
}    

template<typename T>
void ExecutionToAlgoExecutionListener<T>::ProcessAdd(AlgoExecution<T>& _data)
{
    ExecutionOrder<T>* executionOrder = _data.GetExecutionOrder();
    service->OnMessage(*executionOrder);
    service->ExecuteOrder(*executionOrder);
}

