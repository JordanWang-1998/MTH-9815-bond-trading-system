/**
 * my functions.hpp
 *
 * @author Jordan Wang
 */

#ifndef my_functions_hpp
#define my_functions_hpp
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/algorithm/string.hpp"
#include "soa.hpp"
#include "products.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <time.h>
#include <windows.h>
#include <map>
#include <unordered_map>
using namespace std;
using namespace std::chrono;
using namespace boost::gregorian;
using namespace boost::algorithm;
using Clock = system_clock;
using Ms = milliseconds;
using Sec = seconds;
template<class Duration> using TimePoint = time_point<Clock, Duration>;




enum Side 				{ BUY, SELL };
enum PricingSide 		{ BID, OFFER };
enum OrderType 			{ FOK, IOC, MARKET, LIMIT, STOP };
enum Market 			{ BROKERTEC, ESPEED, CME };
enum InquiryState 		{ RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };
enum HistoricalDataType { POSITION, RISK, EXECUTION, STREAMING, INQUIRY };




// Get current time in string format
string GetTime()
{
	SYSTEMTIME sys;
    GetLocalTime( &sys );
 
    char curr_time[100];
	sprintf(curr_time, "%4d%02d%02d-%02d:%02d:%02d:%03d",
			sys.wYear, sys.wMonth, sys.wDay, sys.wHour,
			sys.wMinute, sys.wSecond, sys.wMilliseconds);
    return curr_time;
}




// Get current time in millisecond
int GetMillisecond()
{
    system_clock::time_point now = system_clock::now();
	TimePoint<Sec> now_s = time_point_cast<Sec>(now);
	Ms now_ms = duration_cast<Ms>(now - now_s);
    int ms = now_ms.count();
    return ms;
}




// Convert price data from double -> string format
string GetPrice_d2s(double _doublePrice)
{
	int _doublePrice100 = floor(_doublePrice);
	_doublePrice = 32 * (_doublePrice - _doublePrice100);
	int _doublePrice32 = floor(_doublePrice);
	_doublePrice -= _doublePrice32;
	int _doublePrice256 = floor(_doublePrice * 8.0);
	
    string _stringPrice100 = to_string(_doublePrice100);
    string _stringPrice32 = to_string(_doublePrice32);
    string _stringPrice256 = to_string(_doublePrice256);
    
    if (_doublePrice32 < 10) _stringPrice32 = "0" + _stringPrice32;
    if (_doublePrice256 == 4) _stringPrice256 = "+";
    
    return _stringPrice100 + "-" + _stringPrice32 + _stringPrice256;
}




// Convert position data -> vector<string> format
template<typename T>
vector<string> GetPositions_p2s(T product, map<string, long> positions)
{
	vector<string> res;
	
	// Append productId
    string productId = product.GetProductId();
	res.push_back(productId);
	
	for(map<string, long>::iterator it = positions.begin(); it != positions.end(); it++)
	{
		// Append book name
		string book = it->first;
		res.push_back(book);						
		
		// Append position quantity
		string position = to_string(it->second);
		res.push_back(position);					
	}	
	
	return res;
}
 
 
 
 
// Convert pv01 data -> vector<string> format
template<typename T>
vector<string> GetPV01_pv2s(T product, double pv01, long quantity)
{
	vector<string> res;
	res.push_back(product.GetProductId());		// Append productId
	res.push_back(to_string(pv01));				// Append pv01 value
	res.push_back(to_string(quantity));			// Append quantity
	return res;	
}




// Convert execution order data -> vector<string> format
template<typename T>
vector<string> GetExecutionOrder_eo2s(T product, PricingSide side, string orderId, OrderType orderType, double price, long visibleQuantity, long hiddenQuantity, string parentOrderId, bool isChildOrder)
{
	string orderTypeName;
	if (orderType == FOK) orderTypeName = "FOK";
	if (orderType == IOC) orderTypeName = "IOC";
	if (orderType == MARKET) orderTypeName = "MARKET";
	if (orderType == LIMIT) orderTypeName = "LIMIT";
	if (orderType == STOP) orderTypeName = "STOP";
		
	vector<string> res;
	res.push_back(product.GetProductId());						// Append productId
	res.push_back((side == BID) ? "BID" : "OFFER");				// Append side
	res.push_back(orderId);										// Append orderId
	res.push_back(orderTypeName);								// Append orderType
	res.push_back(GetPrice_d2s(price));							// Append price
	res.push_back(to_string(visibleQuantity));					// Append visibleQuantity
	res.push_back(to_string(hiddenQuantity));					// Append hiddenQuantity
	res.push_back(parentOrderId);								// Append parentOrderId
	res.push_back((isChildOrder == true) ? "TRUE" : "FALSE");	// Append isChildOrder
	return res;	
}




// Convert price stream order data -> vector<string> format
template<typename T>
vector<string> GetPriceStreamOrder_pso2s(double price, long visibleQuantity, long hiddenQuantity, PricingSide side)
{
	vector<string> res;
	res.push_back(GetPrice_d2s(price));					// Append price
	res.push_back(to_string(visibleQuantity));			// Append visibleQuantity
	res.push_back(to_string(hiddenQuantity));			// Append hiddenQuantity	
	res.push_back((side == BID) ? "BID" : "OFFER");		// Append side
	return res;
}




// Convert price stream data -> vector<string> format
template<typename T>
vector<string> GetPriceStream_ps2s(T product, vector<string> bidOrder, vector<string> offerOrder)
{
	string productId = product.GetProductId();

	vector<string> res;
	res.push_back(productId);											// Append productId
	res.insert(res.end(), bidOrder.begin(), bidOrder.end());			// Append bidOrder
	res.insert(res.end(), offerOrder.begin(), offerOrder.end());		// Append offerOrder
	return res;
}




// Search bonds by CUSIP
Bond GetBond(string _cusip)
{
    if (_cusip == "91282CAX9") return Bond("91282CAX9", CUSIP, "US2Y", 0.125, from_string("2022/11/30"));
    if (_cusip == "91282CBA8") return Bond("91282CBA8", CUSIP, "US3Y", 0.125, from_string("2023/12/15"));
    if (_cusip == "91282CAZ4") return Bond("91282CAZ4", CUSIP, "US5Y", 0.375, from_string("2025/11/30"));
    if (_cusip == "91282CAY7") return Bond("91282CAY7", CUSIP, "US7Y", 0.625, from_string("2027/11/30"));
    if (_cusip == "91282CAV3") return Bond("91282CAV3", CUSIP, "US10Y", 0.875, from_string("2030/12/15"));
    if (_cusip == "912810ST6") return Bond("912810ST6", CUSIP, "US20Y", 1.375, from_string("2040/11/30"));
    if (_cusip == "912810SS8") return Bond("912810SS8", CUSIP, "US30Y", 1.625, from_string("2050/12/15"));
	return Bond("*********", CUSIP, "US0Y", 0.0, from_string("0000/01/01"));
}




// Search PV01 value by CUSIP
double GetPV01(string _cusip)
{
    if (_cusip == "91282CAX9") return 1.998126079;
    if (_cusip == "91282CBA8") return 2.995311964;
    if (_cusip == "91282CAZ4") return 4.958072114;
    if (_cusip == "91282CAY7") return 6.859835619;
    if (_cusip == "91282CAV3") return 9.594924967;
    if (_cusip == "912810ST6") return 17.52797647;
    if (_cusip == "912810SS8") return 23.82649737;
	return 0.0;
}




// Read data from an input stream
vector<vector<string>> ReadDataStream(fstream& data_stream)
{
	vector<vector<string>> res;
	
	string curr_line;
	while (getline(data_stream, curr_line))				// get each line from data stream
	{
		stringstream line_stream(curr_line);
		string curr_word;
		vector<string> curr_line_of_words;
		while (getline(line_stream, curr_word, ' '))	// get each word from the line stream
		{
			trim(curr_word);							// trim off blanks
			curr_line_of_words.push_back(curr_word);
		}
		res.push_back(curr_line_of_words);
	}
	
	return res;
}




// Output data to an output stream
template<typename T>
void OutputDataStream(string fileName, vector<string> data)
{
	fstream file;
	file.open(fileName, ios::app);		// open existing file with the same fileName
	
	file << GetTime() << " ";
	for (vector<string>::iterator it = data.begin(); it != data.end(); it++)
		file << *it << " ";
	file << endl;
	
	file.close();						// close file
}




#endif