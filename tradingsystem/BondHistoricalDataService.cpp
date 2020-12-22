/**
 * BondHistoricalDataService.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "BondHistoricalDataService.hpp"
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




template<typename V>
HistoricalDataService<V>::HistoricalDataService(HistoricalDataType _type):
	type(_type)
{
    historicalDatas = map<string, V>();
    listeners = vector<ServiceListener<V>*>();
    connector = new HistoricalDataConnector<V>(this);
    listener = new HistoricalDataListener<V>(this);
}

template<typename V>
V& HistoricalDataService<V>::GetData(string key) 
{ 
	return historicalDatas[key]; 
}

template<typename V>
void HistoricalDataService<V>::OnMessage(V& data) 
{ 
	string productId = data.GetProduct().GetProductId();
	historicalDatas[productId] = data; 
}

template<typename V>
void HistoricalDataService<V>::AddListener(ServiceListener<V>* listener) 
{ 
	listeners.push_back(listener); 
}

template<typename V>
const vector<ServiceListener<V>*>& HistoricalDataService<V>::GetListeners() const 
{ 
	return listeners; 
}

template<typename V>
HistoricalDataConnector<V>* HistoricalDataService<V>::GetConnector() 
{
	return connector; 
}

template<typename V>
ServiceListener<V>* HistoricalDataService<V>::GetListener() 
{ 
	return listener; 
}

template<typename V>
HistoricalDataType HistoricalDataService<V>::GetHistoricalDataType() const 
{ 
	return type;
}

template<typename V>
void HistoricalDataService<V>::PersistData(string persistKey, const V& data)
{
	connector->Publish(data);
}




template<typename V>
HistoricalDataConnector<V>::HistoricalDataConnector(HistoricalDataService<V>* _service) 
{ 
	service = _service; 
}

template<typename V>
void HistoricalDataConnector<V>::Publish(V& _data)
{
    HistoricalDataType type = service->GetHistoricalDataType();
	if (type == POSITION) 
		OutputDataStream("positions.txt", data.GetPosition_p2s());			// output to positions.txt
	if (type == RISK) 
		OutputDataStream("risk.txt", data.GetPV01_pv2s());					// output to risk.txt
	if (type == EXECUTION) 
		OutputDataStream("executions.txt", data.GetExecutionOrder_eo2s());	// output to executions.txt
	if (type == STREAMING) 
		OutputDataStream("streaming.txt", data.GetPriceStream_ps2s());		// output to streaming.txt
	if (type == INQUIRY) 
		OutputDataStream("allinquiries.txt", data.GetPosition_p2s());		// output to allinquiries.txt
}

    
	

template<typename V>
HistoricalDataListener<V>::HistoricalDataListener(HistoricalDataService<V>* _service) 
{ 
	service = _service; 
}
    
template<typename V>
void HistoricalDataListener<V>::ProcessAdd(V& _data)
{
    string productId = _data.GetProduct().GetProductId();
    service->PersistData(productId, _data);
}

	
	
	