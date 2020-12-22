/**
 * BondInquiryService.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Jordan Wang
 */
#ifndef BondInquiryService_hpp
#define BondInquiryService_hpp
#include "soa.hpp"
#include "BondTradeBookingService.hpp"
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
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{

public:
	// default ctor
	Inquiry() = default;
	
    // ctor for an inquiry
    Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state);

    // Get the inquiry ID
    const string& GetInquiryId() const;

    // Get the product
    const T& GetProduct() const;

    // Get the side on the inquiry
    Side GetSide() const;

    // Get the quantity that the client is inquiring for
    long GetQuantity() const;

    // Get the price that we have responded back with
    double GetPrice() const;

    // Get the current state on the inquiry
    InquiryState GetState() const;

	// Set the current state on the inquiry
	void SetState(InquiryState _state);
	
	// Convert inquiry data -> vector<string> format
	vector<string> GetInquiry_i2s();
	
private:
    string inquiryId;
    T product;
    Side side;
    long quantity;
    double price;
    InquiryState state;
};




/* Connector to InquiryService */
template<typename T>
class InquiryConnector;




/**
 * Service for customer inquirry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since each inquiry must be unique).
 * Type T is the product type.
 */
template<typename T>
class InquiryService : public Service<string,Inquiry<T>>
{
public:
	// default ctor
	InquiryService();
	
    // Get data on our service given a key
    Inquiry<T>& GetData(string key);
	
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Inquiry<T>& data);
    
    // Add a listener to InquiryService for callbacks on add, remove, and update events for data to InquiryService
	void AddListener(ServiceListener<Inquiry<T>>* listener);
	
    // Get all listeners on InquiryService
	vector<ServiceListener<Inquiry<T>>*> GetListeners();
	
    // Send a quote back to the client
    void SendQuote(const string& inquiryId, double price);

    // Reject an inquiry from the client
    void RejectInquiry(const string &inquiryId);
	
	// Get the connector to InquiryService
	InquiryConnector<T>* GetConnector();

private:
    map<string, Inquiry<T>> inquiries;					// a map of {inquiry identifier -> inquiry}
    vector<ServiceListener<Inquiry<T>>*> listeners;		// all listeners on InquiryService
    InquiryConnector<T>* connector;						// a pointer to InquiryConnector
};




/* Connector to InquiryService */
template<typename T>
class InquiryConnector : public Connector<Inquiry<T>>
{
public:
	// ctor
    InquiryConnector(InquiryService<T>* _service);

    // Publish data to the Connector
    void Publish(Inquiry<T>& _data);
	
	// Subscribe data from the Connector
    void Subscribe(fstream& data_stream);
	
private:
    InquiryService<T>* service;
};




#endif
