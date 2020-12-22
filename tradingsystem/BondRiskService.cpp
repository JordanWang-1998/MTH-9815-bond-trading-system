/**
 * BondRiskService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */

#include "BondRiskService.hpp"
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
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
    product(_product)
{
    pv01 = _pv01;
    quantity = _quantity;
}

template<typename T>
const T& PV01<T>::GetProduct() const
{
	return product;
}

template<typename T>
double PV01<T>::GetPV01() const
{
	return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const
{
	return quantity;
}

template<typename T>
void PV01<T>::SetQuantity(long _quantity)
{
	quantity = _quantity;
}

template<typename T>
vector<string> PV01<T>::GetPV01_pv2s()
{
	return GetPV01_pv2s<T>(product, pv01, quantity);
}




template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
    products(_products)
{
    name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
    return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
    return name;
}




template<typename T>
RiskService<T>::RiskService()
{
    pv01s = map<string, PV01<T> >();
    listeners = vector<ServiceListener<PV01<T> >*>();
    listener = new RiskToPositionListener<T>(this);
}

template<typename T>
PV01<T>& RiskService<T>::GetData(string key) 
{ 
	return pv01s[key]; 
}

template<typename T>
void RiskService<T>::OnMessage(PV01<T>& data) 
{ 
	T curr_product = data.GetProduct();
	string productId = curr_product.GetProductId();
	pv01s[productId] = data; 
}

template<typename T>
void RiskService<T>::AddListener(ServiceListener<PV01<T>>* listener) 
{ 
	listeners.push_back(listener); 
}

template<typename T>
const RiskService<T>::vector<ServiceListener<PV01<T>>*>& GetListeners() const 
{ 
	return listeners; 
}

template<typename T>
RiskToPositionListener<T>* RiskService<T>::GetListener() 
{ 
	return listener; 
}
	
template<typename T>
const PV01<BucketedSector<T>>& RiskService<T>::GetBucketedRisk(const BucketedSector<T>& sector) const
{
    BucketedSector<T> curr_bs = sector;
	vector<T> curr_products = sector.GetProducts();
	double pv01Value = 0.0;
	long quantity = 0;
	
	for (vector<T>::iterator it = curr_products.begin(); it != curr_products.end(); it++)
	{
		string productId = it->GetProductId();
		pv01Value = pv01s[productId].GetPV01() * pv01s[productId].GetQuantity();
		quantity += pv01s[productId].GetQuantity();
	}
	
    return PV01<BucketedSector<T> >(curr_bs, pv01Value, quantity);
}

template<typename T>
void RiskService<T>::AddPosition(Position<T>& position)
{
    T curr_product = position.GetProduct();
    string productId = curr_product.GetProductId();
    double pv01Value = GetPV01Value(productId);
    long quantity = position.GetAggregatePosition();
	
	// Update pv01 value
    PV01<T> new_pv01(curr_product, pv01Value, quantity);
    pv01s[productId] = new_pv01;
    
    for (vector<ServiceListener<PV01<T>>*>::iterator it = listeners.begin(); it != listeners.end(); it++)
        (*it)->ProcessAdd(new_pv01);
}




template<typename T>
RiskToPositionListener<T>::RiskToPositionListener(RiskService<T>* _service)
{
	service = _service;
}

template<typename T>
void RiskToPositionListener<T>::ProcessAdd(Position<T>& _data)
{
	service->AddPosition(_data);
}


