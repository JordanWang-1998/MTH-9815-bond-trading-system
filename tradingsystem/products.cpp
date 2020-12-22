/**
 * products.cpp
 * Defines the data types and Service for internal prices.
 *
 * @author Jordan Wang
 */
 
#include "soa.hpp"
#include "products.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
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
using namespace boost::gregorian;




Product::Product(string _productId, ProductType _productType)
{
    productId = _productId;
    productType = _productType;
}

const string& Product::GetProductId() const
{
    return productId;
}

ProductType Product::GetProductType() const
{
    return productType;
}




Bond::Bond(string _productId, BondIdType _bondIdType, string _ticker, float _coupon, date _maturityDate) : Product(_productId, BOND)
{
    bondIdType = _bondIdType;
    ticker = _ticker;
    coupon = _coupon;
    maturityDate =_maturityDate;
}

//Bond::Bond() : Product(0, BOND)
//{
//}

const string& Bond::GetTicker() const
{
    return ticker;
}

float Bond::GetCoupon() const
{
    return coupon;
}

const date& Bond::GetMaturityDate() const
{
    return maturityDate;
}

BondIdType Bond::GetBondIdType() const
{
    return bondIdType;
}

ostream& operator<<(ostream &output, const Bond &bond)
{
    output << bond.ticker << " " << bond.coupon << " " << bond.GetMaturityDate();
    return output;
}




IRSwap::IRSwap(string _productId, DayCountConvention _fixedLegDayCountConvention, DayCountConvention _floatingLegDayCountConvention, PaymentFrequency _fixedLegPaymentFrequency, FloatingIndex _floatingIndex, FloatingIndexTenor _floatingIndexTenor, date _effectiveDate, date _terminationDate, Currency _currency, int _termYears, SwapType _swapType, SwapLegType _swapLegType) :
    Product(_productId, IRSWAP)
{
    fixedLegDayCountConvention =_fixedLegDayCountConvention;
    floatingLegDayCountConvention =_floatingLegDayCountConvention;
    fixedLegPaymentFrequency =_fixedLegPaymentFrequency;
    floatingIndex =_floatingIndex;
    floatingIndexTenor =_floatingIndexTenor;
    effectiveDate =_effectiveDate;
    terminationDate =_terminationDate;
    currency =_currency;
    termYears = _termYears;
    swapType =_swapType;
    swapLegType = _swapLegType;
    effectiveDate =_effectiveDate;
    terminationDate =_terminationDate;
}

//IRSwap::IRSwap() : Product(0, IRSWAP)
//{
//}

DayCountConvention IRSwap::GetFixedLegDayCountConvention() const
{
    return fixedLegDayCountConvention;
}

DayCountConvention IRSwap::GetFloatingLegDayCountConvention() const
{
    return floatingLegDayCountConvention;
}

PaymentFrequency IRSwap::GetFixedLegPaymentFrequency() const
{
    return fixedLegPaymentFrequency;
}

FloatingIndex IRSwap::GetFloatingIndex() const
{
    return floatingIndex;
}

FloatingIndexTenor IRSwap::GetFloatingIndexTenor() const
{
    return floatingIndexTenor;
}

const date& IRSwap::GetEffectiveDate() const
{
    return effectiveDate;
}

const date& IRSwap::GetTerminationDate() const
{
    return terminationDate;
}

Currency IRSwap::GetCurrency() const
{
    return currency;
}

int IRSwap::GetTermYears() const
{
    return termYears;
}

SwapType IRSwap::GetSwapType() const
{
    return swapType;
}

SwapLegType IRSwap::GetSwapLegType() const
{
    return swapLegType;
}


ostream& operator<<(ostream &output, const IRSwap &swap)
{
    output << "fixedDayCount:" << swap.ToString(swap.GetFixedLegDayCountConvention()) << " floatingDayCount:" << swap.ToString(swap.GetFloatingLegDayCountConvention()) << " paymentFreq:" << swap.ToString(swap.GetFixedLegPaymentFrequency()) << " " << swap.ToString(swap.GetFloatingIndexTenor()) << swap.ToString(swap.GetFloatingIndex()) << " effective:" << swap.GetEffectiveDate() << " termination:" << swap.GetTerminationDate() << " " << swap.ToString(swap.GetCurrency()) << " " << swap.GetTermYears() << "yrs " << swap.ToString(swap.GetSwapType()) << " " << swap.ToString(swap.GetSwapLegType());
    return output;
}

string IRSwap::ToString(DayCountConvention dayCountConvention) const
{
    switch (dayCountConvention) {
    case THIRTY_THREE_SIXTY: return "30/360";
    case ACT_THREE_SIXTY: return "Act/360";
    default: return "";
    }
}

string IRSwap::ToString(PaymentFrequency paymentFrequency) const
{
    switch (paymentFrequency) {
    case QUARTERLY: return "Quarterly";
    case SEMI_ANNUAL: return "Semi-Annual";
    case ANNUAL: return "Annual";
    default: return "";
    }
}

string IRSwap::ToString(FloatingIndex floatingIndex) const
{
    switch (floatingIndex) {
    case LIBOR: return "LIBOR";
    case EURIBOR: return "EURIBOR";
    default: return "";
    }
}

string IRSwap::ToString(FloatingIndexTenor floatingIndexTenor) const
{ 
    switch(floatingIndexTenor) {
    case TENOR_1M: return "1m";
    case TENOR_3M: return "3m";
    case TENOR_6M: return "6m";
    case TENOR_12M: return "12m";
    default: return "";
    }
}

string IRSwap::ToString(Currency currency) const
{ 
    switch(currency) {
    case USD: return "USD";
    case EUR: return "EUR";
    case GBP: return "GBP";
    default: return "";
    }
}

string IRSwap::ToString(SwapType swapType) const
{ 
    switch(swapType) {
    case STANDARD: return "Standard";
    case FORWARD: return "Forward";
    case IMM: return "IMM";
    case MAC: return "MAC";
    case BASIS: return "Basis";
    default: return "";
    }
}

string IRSwap::ToString(SwapLegType swapLegType) const
{ 
    switch(swapLegType) {
    case OUTRIGHT: return "Outright";
    case CURVE: return "Curve";
    case FLY: return "Fly";
    default: return "";
    }
}



