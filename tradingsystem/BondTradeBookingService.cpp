/**
 * BondTradeBookingService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "BondTradeBookingService.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
using namespace std;




template<typename T>
Trade::Trade()
{
}

template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side) :
    product(_product)
{
    tradeId = _tradeId;
    price = _price;
    book = _book;
    quantity = _quantity;
    side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
    return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
    return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const
{
    return price;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
    return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
    return side;
}




template<typename T>
TradeBookingService<T>::TradeBookingService()
{
	trades = map<string, Trade<T>>();
	listeners = vector<ServiceListener<Trade<T>>*>();
	connector = new TradeBookingConnector<T>(this);
	listener = new TradeBookingToExecutionListener<T>(this);
}

template<typename T>
Trade<T>& TradeBookingService<T>::GetData(string key)
{
	return trades[key];
}

template<typename T>
void TradeBookingService<T>::OnMessage(Trade<T>& data)
{
	string _tradeId = trades[data.GetTradeId()];
	for (vector<ServiceListener<Trade<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
		(*it)->ProcessAdd(_tradeId);
}

template<typename T>
void TradeBookingService<T>::AddListener(ServiceListener<Trade<T>>* listener)
{
	listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Trade<T>>*>& TradeBookingService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
TradeBookingConnector<T>* TradeBookingService<T>::GetConnector()
{
	return connector;
}

template<typename T>
TradeBookingToExecutionListener<T>* TradeBookingService<T>::GetListener()
{
	return listener;
}

template<typename T>
void TradeBookingService<T>::BookTrade(const Trade<T>& trade)
{
	for (vector<ServiceListener<Trade<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
		(*it)->ProcessAdd(trade);
}




template<typename T>
TradeBookingConnector<T>::TradeBookingConnector(TradeBookingService<T>* _service)
{
	service = _service;
}

template<typename T>
void TradeBookingConnector<T>::Subscribe(fstream& data_stream)
{
	// Read trade data from an input stream
	vector<vector<string>> trade_data = ReadDataStream(data_stream);
	for (vector<vector<string>>::iterator it = trade_data.begin(); it != trade_data.end(); it++)
	{
		string productId = (*it)[0];
		string tradeId = (*it)[1];
		double price = GetPrice_d2s((*it)[2]);
		string book = (*it)[3];
		long quantity = stol((*it)[4]);
		string tradeId = (*it)[1];
		Side side = ((*it)[5] == "BUY") ? BUY : SELL;
		
		// Bond
		if (T == BOND)
		{
			T curr_product = GetBond(productId);
			Trade<T> curr_trade(curr_product, tradeId, price, book, quantity, side);
			service->OnMessage(curr_trade);
		}
	}
	
}




template<typename T>
TradeBookingToExecutionListener<T>::TradeBookingToExecutionListener(TradeBookingService<T>* _service)
{
	service = _service;
	count = 0;
}

template<typename T>
void TradeBookingToExecutionListener<T>::ProcessAdd(ExecutionOrder<T>& _data) 
{
	count++;
	
	T curr_product = data.GetProduct();
	PricingSide pricingSide = data.GetPricingSide();
	string orderId = data.GetOrderId();
	double price = data.GetPrice();
	long visibleQuantity = data.GetVisibleQuantity();
	long hiddenQuantity = data.GetHiddenQuantity();
	long quantity = visibleQuantity + hiddenQuantity;
	
	string book;
	if (count % 3 == 0) book = "TRSY1";
	if (count % 3 == 1) book = "TRSY2";
	if (count % 3 == 2) book = "TRSY3";
	
	Side side;
	if (pricingSide == BID) side = SELL;
	if (pricingSide == OFFER) side = BUY;
			
	Trade<T> curr_trade(curr_product, orderId, price, book, quantity, side);
	service->BookTrade(curr_trade);
	service->OnMessage(curr_trade);
}

