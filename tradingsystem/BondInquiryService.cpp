/**
 * BondInquiryService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */

#include "BondInquiryService.hpp"
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
Inquiry<T>::Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state) :
    product(_product)
{
    inquiryId = _inquiryId;
    side = _side;
    quantity = _quantity;
    price = _price;
    state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
    return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
    return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
    return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
    return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
    return state;
}




template<typename T>
InquiryService<T>::InquiryService()
{
    inquiries = map<string, Inquiry<T>>();
    listeners = vector<ServiceListener<Inquiry<T>>*>();
    connector = new InquiryConnector<T>(this);
}

template<typename T>
Inquiry<T>& GetData(string key)
{
	return inquiries[key];
}

template<typename T>
void OnMessage(Inquiry<T>& data)
{
    InquiryState state = data.GetState();
	if (state == RECEIVED) 
	{
		string inquiryId = data.GetInquiryId();
		inquiries[inquiryId] = data;
		connector->Publish(data);
	}
	if (state == QUOTED)
	{
		data.SetState(DONE);
		string inquiryId = data.GetInquiryId();
		inquiries[inquiryId] = data;
		for (vector<ServiceListener<Inquiry<T>>*>::iterator inq_it = listeners.begin(); inq_it != listeners.end(); inq_it++)
			(*inq_it)->ProcessAdd(data);
	}
}

template<typename T>
void AddListener(ServiceListener<Inquiry<T>>* listener)
{
	listeners.push_back(listener);
}

template<typename T>
vector<ServiceListener<Inquiry<T>>*> GetListeners()
{
	return listeners;
}

template<typename T>
void SendQuote(const string& inquiryId, double price)
{
    Inquiry<T> inquiry = inquiries[inquiryId];
    inquiry.SetPrice(price);
    for (vector<ServiceListener<Inquiry<T>>*>::iterator inq_it = listeners.begin(); inq_it != listeners.end(); inq_it++)
        (*inq_it)->ProcessAdd(inquiry);
}

template<typename T>
void RejectInquiry(const string &inquiryId)
{
    Inquiry<T> inquiry = inquiries[inquiryId];
    inquiry.SetState(REJECTED);
}

template<typename T>
InquiryConnector<T>* GetConnector()
{
	return connector;
}




template<typename T>
InquiryConnector<T>::InquiryConnector(InquiryService<T>* _service)
{ 
	service = _service; 
}

template<typename T>
void InquiryConnector<T>::Publish(Inquiry<T>& _data)
{
    InquiryState state = _data.GetState();
    if (state == RECEIVED)
    {
        data.SetState(QUOTED);
		service->OnMessage(data);
    }
}

template<typename T>
void InquiryConnector<T>::Subscribe(fstream& data_stream)
{
	// Read inquiry data from an input stream
	vector<vector<string>> inquiry_data = ReadDataStream(data_stream);
	for (vector<vector<string>>::iterator it = inquiry_data.begin(); it != inquiry_data.end(); it++)
	{
		string inquiryId = (*it)[0];
		string productId = (*it)[1];
		Side side = ((*it)[2] == "BUY") ? BUY : SELL;
		long quantity = stol((*it)[3]);
        double price = GetPrice_d2s(stod((*it)[4]));
		
        InquiryState state;
        if ((*it)[5] == "RECEIVED") 			state = RECEIVED;
        if ((*it)[5] == "QUOTED") 				state = QUOTED;
        if ((*it)[5] == "DONE") 				state = DONE;
        if ((*it)[5] == "REJECTED")			 	state = REJECTED;
        if ((*it)[5] == "CUSTOMER_REJECTED") 	state = CUSTOMER_REJECTED;
		
		// Bond
		if (T == BOND)
		{
			T curr_product = GetBond(productId);
			Inquiry<T> inquiry(inquiryId, curr_product, side, quantity, price, state);
			service->OnMessage(inquiry);
		}
	}
}


