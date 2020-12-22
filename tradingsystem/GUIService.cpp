/**
 * GUIService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "GUIService.hpp"
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
GUIService<T>::GUIService()
{
    guis = map<string, Price<T> >();
    listeners = vector<ServiceListener<Price<T>>*>();
    connector = new GUIConnector<T>(this);
    listener = new GUIToPricingListener<T>(this);
    throttle = 300;
    millisecond = GetMillisecond();
}

template<typename T>
Price<T>& GUIService<T>::GetData(string key) 
{	
	return guis[key];
}

template<typename T>
void GUIService<T>::OnMessage(Price<T>& data)
{
	T curr_product = data.GetProduct();
	string productId = curr_product.GetProductId();
    guis[productId] = data;
	
    connector->Publish(data);	// output to gui.txt
}

template<typename T>
void GUIService<T>::AddListener(ServiceListener<Price<T>>* listener) 
{
	listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Price<T>>*>& GUIService<T>::GetListeners() const 
{
	return listeners;
}

template<typename T>
GUIConnector<T>* GUIService<T>::GetConnector() 
{
	return connector;
}

template<typename T>
GUIToPricingListener<T>* GUIService<T>::GetListener() 
{
	return listener;
}

template<typename T>
int GUIService<T>::GetThrottle() 
{
	return throttle;
}

template<typename T>
int GUIService<T>::GetMillisecond() 
{
	return millisecond;
}

template<typename T>
void GUIService<T>::SetMillisecond(int _millisecond) 
{
	millisecond = _millisecond;
}




template<typename T>
GUIConnector<T>::GUIConnector(GUIService<T>* _service)
{ 
	service = _service;
}

template<typename T>
void GUIConnector<T>::Publish(Price<T>& _data)
{
	int throttle = service->GetThrottle();
	int millisecondStart = service->GetMillisecond();
	int millisecondNow = GetMillisecond();
	
	if (millisecondNow - millisecondStart >= throttle) 
	{
		service->SetMillisec(millisecondNow);			// reset millisecond
		OutputDataStream("gui.txt", _data.print());		// output to gui.txt
	}
}
	
	


template<typename T>
GUIToPricingListener<T>::GUIToPricingListener(GUIService<T>* _service) 
{
	service = _service;
}

template<typename T>
void GUIToPricingListener<T>::ProcessAdd(Price<T>& _data) 
{
	service->OnMessage(_data);
}



