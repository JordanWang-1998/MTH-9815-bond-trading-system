/**
 * BondMarketDataService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
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




Order::Order(double _price, long _quantity, PricingSide _side)
{
    price = _price;
    quantity = _quantity;
    side = _side;
}

double Order::GetPrice() const
{
    return price;
}
 
long Order::GetQuantity() const
{
    return quantity;
}
 
PricingSide Order::GetSide() const
{
    return side;
}




BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
    bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
    return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
    return offerOrder;
}




template<typename T>
OrderBook<T>::OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack) :
    product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template<typename T>
const T& OrderBook<T>::GetProduct() const
{
    return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const
{
    return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const
{
    return offerStack;
}

template<typename T>
const BidOffer& OrderBook<T>::GetBestBidOffer() const
{
	double bestBidPrice = -99999.99, bestOfferPrice = 99999.99;
	Order bestBidOrder, bestOfferOrder;
	
	// Traverse bidStack to find the best bid order
	for (vector<Order>::iterator it = bidStack.begin(); it != bidStack.end(); it++)
	{
		if (it->GetPrice() > bestBidPrice)
		{
			bestBidPrice = it->GetPrice();
			bestBidOrder = (*it);
		}
	}
	
	// Traverse offerStack to find the best offer order
	for (vector<Order>::iterator it = offerStack.begin(); it != offerStack.end(); it++)
	{
		if (it->GetPrice() < bestOfferPrice)
		{
			bestOfferPrice = it->GetPrice();
			bestOfferOrder = (*it);
		}
	}
	
	return BidOffer(bestBidOrder, bestOfferOrder);
}




template<typename T>
MarketDataService<T>::MarketDataService()
{
    orderBooks = map<string, OrderBook<T> >();
    listeners = vector<ServiceListener<OrderBook<T> >*>();
    connector = new MarketDataConnector<T>(this);
    orderBookLevels = 5;
}

template<typename T>
OrderBook<T>& MarketDataService<T>::GetData(string key)
{
	return orderBooks[key];
}

template<typename T>
void MarketDataService<T>::OnMessage(OrderBook<T>& data)
{
	T curr_product = data.GetProduct();
	string productId = curr_product.GetProductId();
	orderBooks[productId] = data;
	
	for (vector<ServiceListener<OrderBook<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
		(*it)->ProcessAdd(data);
}

template<typename T>
void MarketDataService<T>::AddListener(ServiceListener<OrderBook<T>>* listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<OrderBook<T>>*>& MarketDataService<T>::GetListeners() const
{
	return listeners; 
}

template<typename T>
MarketDataConnector<T>* MarketDataService<T>::GetConnector()
{
	return connector;
}

template<typename T>
int MarketDataService<T>::GetOrderBookLevels() const
{
	return orderBookLevels;
}

template<typename T>
const BidOffer& MarketDataService<T>::GetBestBidOffer(const string &productId)
{
	return orderBooks[productId].GetBestBidOffer();
}

template<typename T>
const OrderBook<T>& MarketDataService<T>::AggregateDepth(const string &productId)
{
	OrderBook<T> orderBook = orderBooks[productId];
	T curr_product = orderBook.GetProduct();
	
	
	vector<Order> oldBidStack = orderBook.GetBidStack();
	vector<Order> newBidStack;
	// a map of {bid price -> bid quantity}
	map<double, long> aggregatedBids;
	for (vector<Order>::iterator it = oldBidStack.begin(); it != oldBidStack.end(); it++)
	{
		double price = it->GetPrice();
		long quantity = it->GetQuantity();
		aggregatedBids[price] += quantity;
	}
	// Aggregate all the bids
	for (map<double, long>::iterator it = aggregatedBids.begin(); it != aggregatedBids.end(); it++)
		newBidStack.push_back(Order(it->first, it->second, BID));

	
	vector<Order> oldOfferStack = orderBook.GetOfferStack();
	vector<Order> newOfferStack;
	// a map of {offer price -> offer quantity}
	map<double, long> aggregatedOffers;
	for (vector<Order>::iterator it = oldOfferStack.begin(); it != oldOfferStack.end(); it++)
	{
		double price = it->GetPrice();
		long quantity = it->GetQuantity();
		aggregatedOffers[price] += quantity;
	}
	// Aggregate all the offers
	for (map<double, long>::iterator it = aggregatedOffers.begin(); it != aggregatedOffers.end(); it++)
		newOfferStack.push_back(Order(it->first, it->second, OFFER));

	return OrderBook<T>(curr_product, newBidStack, newOfferStack);
}




template<typename T>
MarketDataConnector<T>::MarketDataConnector()
{
	service = _service;
}

template<typename T>
void MarketDataConnector<T>::Subscribe(fstream& data_stream)
{
	int orderBookLevels = service->GetOrderBookLevels();
	int orderCount = 0;
	vector<Order> bidStack;
	vector<Order> offerStack;
	
	// Read market data from an input stream
	vector<vector<string>> market_data = ReadDataStream(data_stream);
	
	for (vector<vector<string>>::iterator it = market_data.begin(); it != market_data.end(); it++)
	{
		string productId = (*it)[0];
		double price = GetPrice_d2s((*it)[1]);
		long quantity = stol((*it)[2]);
		PricingSide side = ((*it)[3] == "BID") ? BID : OFFER;
		
		Order order(price, quantity, side);
		if (side == BID) bidStack.push_back(order);
		if (side == OFFER) offerStack.push_back(order);
		
		orderCount++;
		if (orderCount % (2 * orderBookLevels) == 0)
		{
			// Bond
			if (T == BOND)
			{
				T curr_product = GetBond(productId);
				OrderBook<T> orderBook(curr_product, bidStack, offerStack);
				service->OnMessage(orderBook);

				// Clear bidStack and offerStack
				bidStack.clear();
				offerStack.clear();
			}
		}
	}
}


