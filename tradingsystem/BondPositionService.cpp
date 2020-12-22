/**
 * BondPositionService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "BondTradeBookingService.hpp"
#include "BondPositionService.hpp"
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
Position<T>::Position(const T &_product) :
    product(_product)
{
	positions = map<string, long>();
}

template<typename T>
const T& Position<T>::GetProduct() const
{
    return product;
}

template<typename T>
long Position<T>::GetPosition(string &book)
{
    return positions[book];
}

template<typename T>
long Position<T>::GetAggregatePosition()
{
    return accumulate(begin(positions), end(positions), 0,
		[](long previous, pair<string, long>& p){ return previous + p.second; });
}

template<typename T>
map<string, long> Position<T>::GetPositions() 
{
	return positions; 
}

template<typename T>
vector<string> Position<T>::GetPositions_p2s()
{
	return GetPositions_p2s<T>(product, positions);
}

template<typename T>
void Position<T>::AddPosition(string& book, long position)
{
	positions[book] += position;
}




template<typename T>
PositionService<T>::PositionService()
{
    positions = map<string, Position<T>>();
    listeners = vector<ServiceListener<Position<T>>*>();
    listener = new PositionToTradeBookingListener<T>(this);
}

template<typename T>
Position<T>& PositionService<T>::GetData(string key)
{
	return positions[key];
}

template<typename T>
void PositionService<T>::OnMessage(Position<T>& data)
{
	string curr_product = data.GetProduct();
	string productId = curr_product.GetProductId();
	positions[productId] = data;
}

template<typename T>
void PositionService<T>::AddListener(ServiceListener<Position<T>>* listener)
{
	listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Position<T>>*>& PositionService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
PositionToTradeBookingListener<T>* PositionService<T>::GetListener()
{
	return listener; 
}

template<typename T>
void PositionService<T>::AddTrade(const Trade<T>& trade)
{
	T curr_product = trade.GetProduct();
	string productId = curr_product.GetProductId();
    double price = trade.GetPrice();
    string book = trade.GetBook();
    long quantity = trade.GetQuantity();
    Side side = trade.GetSide();
	
    Position<T> newPosition(curr_product);
	Position<T> oldPosition = positions[productId];
	map<string, long> oldPositions = oldPosition.GetPositions();
	
	if (side == BUY) newPosition.AddPosition(book, quantity);
	if (side == SELL) newPosition.AddPosition(book, -quantity);
	for (map<string, long>::iterator it = oldPositions.begin(); it != oldPositions.end(); it++)
		newPosition.AddPosition(it->first, it->second);
	positions[productId] = newPosition;
	
	for (vector<ServiceListener<Position<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
		(*it)->ProcessAdd(newPosition);
}




template<typename T>
PositionToTradeBookingListener<T>::PositionToTradeBookingListener(PositionService<T>* _service) 
{ 
	service = _service; 
}

template<typename T>
void PositionToTradeBookingListener<T>::ProcessAdd(Trade<T> &data)
{
	service->AddTrade(data);
}


