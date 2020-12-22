/**
 * BondPricingService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "BondPricingService.hpp"
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
Price<T>::Price(const T &_product, double _mid, double _bidOfferSpread) :
    product(_product)
{
    mid = _mid;
    bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const
{
    return product;
}

template<typename T>
double Price<T>::GetMid() const
{
    return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
    return bidOfferSpread;
}

template<typename T>
vector<string> Price<T>::print()
{
	string _product = product.GetProductId();
	string _mid = GetPrice_d2s(mid);
	string _bidOfferSpread = GetPrice_d2s(bidOfferSpread);
	
	return vector<string>(_product, _mid, _bidOfferSpread);
}




template<typename T>
PricingService<T>::PricingService()
{
	prices = map<string, Price<T>>();
	listeners = vector<ServiceListener<Price<T>>*>();	
	connector = new PricingConnector<T>(this);
}


template<typename T>
Price<T>& PricingService<T>::GetData(string key)
{
	return prices[key];
}
	
template<typename T>
void PricingService<T>::OnMessage(Price<T>& data)
{
	string _productId = prices[data.GetProduct().GetProductId()];
	for(vector<ServiceListener<Price<T>>*>::iterator p_it = listeners.begin(); p_it != listeners.end(); p_it++)
		(*p_it)->ProcessAdd(_productId);
}

template<typename T>
void PricingService<T>::AddListener(ServiceListener<Price<T>>* listener)
{
	listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Price<T>>*>& PricingService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
PricingConnector<T>* PricingService<T>::GetConnector()
{
	return connector;
}




template<typename T>
PricingConnector<T>::PricingConnector(PricingService<T>* _service)
{
	service = _service;
}

template<typename T>
void PricingConnector<T>::Subscribe(fstream& data_stream)
{
	// Read price data from an input stream
	vector<vector<string>> price_data = ReadDataStream(data_stream);
	for (vector<vector<string>>::iterator it = price_data.begin; it != price_data.end(); it++)
	{
		string productId = (*it)[0];
		double bidPrice = GetPrice_d2s((*it)[1]);
		double offerPrice = GetPrice_d2s((*it)[2]);
		double midPrice = (bidPrice + offerPrice) / 2.0;
		double spread = offerPrice - bidPrice;
		
		// Bond
		if (T == BOND)
		{
			T curr_product = GetBond(productId);
			Price<T> curr_price(curr_product, midPrice, spread);
			service->OnMessage(curr_price);
		}
	}
}



