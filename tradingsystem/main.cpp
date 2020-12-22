/**
 * main.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "soa.hpp"
#include "products.hpp"
#include "my functions.hpp"
#include "BondPricingService.hpp"
#include "BondTradeBookingService.hpp"
#include "BondPositionService.hpp"
#include "BondRiskService.hpp"
#include "BondMarketDataService.hpp"
#include "BondExecutionService.hpp"
#include "BondStreamingService.hpp"
#include "GUIService.hpp"
#include "BondInquiryService.hpp"
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

int main()
{
	// Initialize all services
	cout << "*******************************" << endl;
	cout << "*** Initialize all services ***" << endl;
	cout << "*******************************" << endl;
	PricingService<Bond> pricingService;
    TradeBookingService<Bond> tradeBookingService;
    PositionService<Bond> positionService;
    RiskService<Bond> riskService;
    MarketDataService<Bond> marketDataService;
    AlgoExecutionService<Bond> algoExecutionService;
    AlgoStreamingService<Bond> algoStreamingService;	
    GUIService<Bond> guiService;
    ExecutionService<Bond> executionService;
    StreamingService<Bond> streamingService;
    InquiryService<Bond> inquiryService;
    HistoricalDataService<PriceStream<Bond>> historicalStreamingService(STREAMING);
    HistoricalDataService<ExecutionOrder<Bond>> historicalExecutionService(EXECUTION);
    HistoricalDataService<Position<Bond>> historicalPositionService(POSITION);
    HistoricalDataService<PV01<Bond>> historicalRiskService(RISK);
    HistoricalDataService<Inquiry<Bond>> historicalInquiryService(INQUIRY);
	cout << "*******************************" << endl;
	cout << "*** Initialization complete ***" << endl; 
	cout << "*******************************" << endl;
	cout << endl << endl << endl;
	
	// Initialize all listeners
	cout << "********************************" << endl;
	cout << "*** Initialize all listeners ***" << endl;
	cout << "********************************" << endl;
    pricingService.AddListener(algoStreamingService.GetListener());
    pricingService.AddListener(guiService.GetListener());
    tradeBookingService.AddListener(positionService.GetListener());
    positionService.AddListener(riskService.GetListener());
    marketDataService.AddListener(algoExecutionService.GetListener());
    algoExecutionService.AddListener(executionService.GetListener());
    algoStreamingService.AddListener(streamingService.GetListener());
    executionService.AddListener(tradeBookingService.GetListener()); 
	positionService.AddListener(historicalPositionService.GetListener());
    riskService.AddListener(historicalRiskService.GetListener());
    executionService.AddListener(historicalExecutionService.GetListener());
    streamingService.AddListener(historicalStreamingService.GetListener());
    inquiryService.AddListener(historicalInquiryService.GetListener());
	cout << "*******************************" << endl;
	cout << "*** Initialization complete ***" << endl; 
	cout << "*******************************" << endl;
	cout << endl << endl << endl;
    
	// Test the trading system
	cout << "*******************************" << endl;
	cout << "*** Test the trading system ***" << endl;
	cout << "*******************************" << endl;
	// Load 4 input data files
	fstream prices_txt("./input/prices.txt");
	fstream marketdata_txt("./input/marketdata.txt");
	fstream trades_txt("./input/trades.txt");
	fstream inquiries_txt("./input/inquiries.txt");
	// Read 4 input data files
    pricingService.GetConnector()->Subscribe(prices_txt);
    marketDataService.GetConnector()->Subscribe(marketdata_txt);
    tradeBookingService.GetConnector()->Subscribe(trades_txt);
    inquiryService.GetConnector()->Subscribe(inquiries_txt);
	cout << "*********************" << endl;
	cout << "*** Test complete ***" << endl; 
	cout << "*********************" << endl;
	cout << endl << endl << endl;
	
	return 0;
}
