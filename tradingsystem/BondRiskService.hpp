/**
 * BondRiskService.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Jordan Wang
 */
#ifndef BondRiskService_hpp
#define BondRiskService_hpp
#include "soa.hpp"
#include "my functions.hpp"
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




/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{

public:
	// default ctor
	PV01() = default;
	
    // ctor for a PV01 value
    PV01(const T &_product, double _pv01, long _quantity);

    // Get the product on this PV01 value
    const T& GetProduct() const;

    // Get the PV01 value
    double GetPV01() const;

    // Get the quantity that this risk value is associated with
    long GetQuantity() const;
	
	// Set the quantity that this risk value is associated with
	void SetQuantity(long _quantity);

	// Convert pv01 data -> vector<string> format
	vector<string> GetPV01_pv2s();
	
private:
    T product;
    double pv01;
    long quantity;
};




/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:
	// default ctor
	BucketedSector() = default;
	
    // ctor for a bucket sector
    BucketedSector(const vector<T> &_products, string _name);

    // Get the products associated with this bucket
    const vector<T>& GetProducts() const;

    // Get the name of the bucket
    const string& GetName() const;

private:
    vector<T> products;
    string name;

};




/* Listener of BondRiskService to BondPositionService */
template<typename T>
class RiskToPositionListener;




/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string,PV01<T>>
{
public:
	// default ctor
	RiskService();
	
    // Get data on our service given a key
    PV01<T>& GetData(string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PV01<T>& data);

    // Add a listener to BondRiskService for callbacks on add, remove, and update events for data to BondRiskService
    void AddListener(ServiceListener<PV01<T>>* listener);
		
    // Get all listeners on BondRiskService
	const vector<ServiceListener<PV01<T>>*>& GetListeners() const;

	// Get the listener to BondPositionService
	RiskToPositionListener<T>* GetListener();
	
    // Get the bucketed risk for the bucket sector
    const PV01< BucketedSector<T>>& GetBucketedRisk(const BucketedSector<T>& sector) const;

    // Add a position that the service will risk
    void AddPosition(Position<T>& position);

private:
    map<string, PV01<T>> pv01s;						// a map of {product identifier -> pv01 value}
    vector<ServiceListener<PV01<T>>*> listeners;	// all listeners on BondRiskService 
    RiskToPositionListener<T>* listener;			// listener to BondPositionService
};




/* Listener of BondRiskService to BondPositionService */
template<typename T>
class RiskToPositionListener : public ServiceListener<Position<T>>
{
public:
	// ctor
    RiskToPositionListener(RiskService<T>* _service);

    // Listener callback to process an add event to BondRiskService
    void ProcessAdd(Position<T>& _data);
	
    // Listener callback to process a remove event to BondRiskService
    void ProcessRemove(Position<T>& _data);
	
    // Listener callback to process an update event to BondRiskService
    void ProcessUpdate(Position<T>& _data);
	
private:
    RiskService<T>* service;						// a pointer to BondRiskService
};




#endif
