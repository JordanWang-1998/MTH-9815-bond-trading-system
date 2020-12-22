/**
 * BondStreamingService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "BondStreamingService.hpp"
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




PriceStreamOrder::PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side)
{
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    side = _side;
}

double PriceStreamOrder::GetPrice() const
{
    return price;
}

long PriceStreamOrder::GetVisibleQuantity() const
{
    return visibleQuantity;
}

long PriceStreamOrder::GetHiddenQuantity() const
{
    return hiddenQuantity;
}

PricingSide PriceStreamOrder::GetSide() const 
{
	return side;
}

vector<string> PriceStreamOrder::GetPriceStreamOrder_pso2s() const
{
	return GetPriceStreamOrder_pso2s<T>(price, visibleQuantity, hiddenQuantity, side);
}




template<typename T>
PriceStream<T>::PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder) :
    product(_product), bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

template<typename T>
const T& PriceStream<T>::GetProduct() const
{
    return product;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const
{
    return bidOrder;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const
{
    return offerOrder;
}

template<typename T>
vector<string> PriceStream<T>::GetPriceStream_ps2s() const
{
	return GetPriceStream_ps2s<T>(product, bidOrder.GetPriceStreamOrder_pso2s(), offerOrder.GetPriceStreamOrder_pso2s());
}




template<typename T>
AlgoStream<T>::AlgoStream(const T& _product, const PriceStreamOrder& _bidOrder, const PriceStreamOrder& _offerOrder)
{
    priceStream = new PriceStream<T>(_product, _bidOrder, _offerOrder);
}

template<typename T>
PriceStream<T>* AlgoStream<T>::GetPriceStream() const
{
	return priceStream;
}




template<typename T>
AlgoStreamingService<T>::AlgoStreamingService()
{
    algoStreams = map<string, AlgoStream<T> >();
    listeners = vector<ServiceListener<AlgoStream<T> >*>();
    listener = new AlgoStreamingToPricingListener<T>(this);
    count = 0;
}

template<typename T>
AlgoStream<T>&  AlgoStreamingService<T>::GetData(string key)
{
	return algoStreams[key];
}

template<typename T>
void AlgoStreamingService<T>::OnMessage(AlgoStream<T>& data)
{
	PriceStream<T> priceStream = data.GetPriceStream();
	T curr_product = priceStream.GetProduct();
	string productId = curr_product.GetProductId();
    algoStreams[productId] = data;
}

template<typename T>
void AlgoStreamingService<T>::AddListener(ServiceListener<AlgoStream<T>>* listener)
{
	listeners.push_back(listener);
}
	
template<typename T>
const vector<ServiceListener<AlgoStream<T>>*>& AlgoStreamingService<T>::GetListeners() const 
{ 
	return listeners; 
}

template<typename T>
AlgoStreamingToPricingListener<T>* AlgoStreamingService<T>::GetListener()
{
	return listener;
}

template<typename T>
void AlgoStreamingService<T>::PublishPrice(Price<T>& price)
{
    T curr_product = price.GetProduct();
    string productId = curr_product.GetProductId();
    
    double mid = price.GetMid();
    double bidOfferSpread = price.GetBidOfferSpread();
    double bidPrice = mid - bidOfferSpread / 2.0;
    double offerPrice = mid + bidOfferSpread / 2.0;
	long visibleQuantity = (count % 2 == 0) ? 10000000 : 2000000;
    long hiddenQuantity = visibleQuantity * 2;
	
	count++;
	
    PriceStreamOrder bidOrder(bidPrice, visibleQuantity, hiddenQuantity, BID);
    PriceStreamOrder offerOrder(offerPrice, visibleQuantity, hiddenQuantity, OFFER);
    AlgoStream<T> algoStream(curr_product, bidOrder, offerOrder);
    algoStreams[productId] = algoStream;
    
    for (vector<ServiceListener<AlgoStream<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
        (*it)->ProcessAdd(_algoStream);
}




template<typename T>
AlgoStreamingToPricingListener<T>::AlgoStreamingToPricingListener(AlgoStreamingService<T>* _service)
{ 
	service = _service; 
}

template<typename T>
void AlgoStreamingToPricingListener<T>::ProcessAdd(Price<T>& _data) 
{ 
	service->PublishPrice(_data); 
}




template<typename T>
StreamingService<T>::StreamingService()
{
    priceStreams = map<string, PriceStream<T> >();
    listeners = vector<ServiceListener<PriceStream<T> >*>();
    listener = new StreamingToAlgoStreamingListener<T>(this);
}

template<typename T>
PriceStream<T>& StreamingService<T>::GetData(string key) 
{ 
	return priceStreams[key]; 
}

template<typename T>
void StreamingService<T>::OnMessage(PriceStream<T>& data) 
{ 
	T curr_product = data.GetProduct();
	string productId = curr_product.GetProductId();
	priceStreams[productId] = data; 
}

template<typename T>
void StreamingService<T>::AddListener(ServiceListener<PriceStream<T>>* listener) 
{ 
	listeners.push_back(listener); 
}

template<typename T>
const vector<ServiceListener<PriceStream<T>>*>& StreamingService<T>::GetListeners() const 
{ 
	return listeners; 
}

template<typename T>
ServiceListener<AlgoStream<T>>* StreamingService<T>::GetListener() 
{ 
	return listener; 
}

template<typename T>
void StreamingService<T>::PublishPrice(PriceStream<T>& priceStream)
{
    for (vector<ServiceListener<PriceStream<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
        (*it)->ProcessAdd(priceStream);
}




template<typename T>
StreamingToAlgoStreamingListener<T>::StreamingToAlgoStreamingListener(StreamingService<T>* _service)
{
	service = _service;
}


template<typename T>
void StreamingToAlgoStreamingListener<T>::ProcessAdd(AlgoStream<T>& _data)
{
    PriceStream<T>* priceStream = _data.GetPriceStream();
    service->OnMessage(*priceStream);
    service->PublishPrice(*priceStream);
}



