#include "pxer.hpp"
#include <cstring>
#include <deque>
#include <stdexcept>
#include <stdint.h>
using namespace std;

void PXer::reset() {
	delete[] dData;
	dData=0;
	dSize=0;
	dIsOK=false;
	delete[] cData;
	cData=0;
	cSize=0;
	cIsOK=false;
}

PXer::PXer() : dData(0),dSize(0),dIsOK(false),cData(0),cSize(0),cIsOK(false) {
	for(int i=0;i<sizeof(cFlag);i++)
		cFlag[i]=0;
}

PXer::PXer(const uint8_t fInit[9]) : dData(0),dSize(0),dIsOK(false),cData(0),cSize(0),cIsOK(false) {
	for(int i=0;i<sizeof(cFlag);i++)
		cFlag[i]=fInit[i];
}

PXer::PXer(const PXer&other) {
	*this = other;
}

PXer::~PXer() {
	delete[] dData;
	delete[] cData;
}
PXer& PXer::operator=(PXer other) {
	dSize=other.dSize;
	dIsOK=other.dIsOK;
	cSize=other.cSize;
	cIsOK=other.cIsOK;
	if(other.dData) {
		dData = new char[dSize];
		memmove(dData,other.dData,dSize);
	} else
		dData=0;
	if(other.cData) {
		cData = new char[cSize];
		memmove(cData,other.cData,cSize);
	} else
		cData=0;
	for(int i=0;i<sizeof(cFlag);i++)
		cFlag[i]=other.cFlag[i];
	return *this;
}

const char* PXer::getDecompressed(size_t& n) const {
	if(dIsOK) {
		n=dSize;
		return dData;
	} else {
		n=0;
		return NULL;
	}
}

const char* PXer::getDecompressed(size_t& n) {
	if(dIsOK) {
		n=dSize;
		return dData;
	} else
		throw logic_error("PXer::getDecompressed() unimplemented!");
}

const char* PXer::getCompressed(size_t& n) const {
	if(cIsOK) {
		n=cSize;
		return cData;
	} else {
		n=0;
		return NULL;
	}
}

const char* PXer::getCompressed(size_t& n) {
	if(cIsOK) {
		n=cSize;
		return cData;
	} else
		throw logic_error("PXer::getCompressed() unimplemented!");
}

PXer& PXer::setDecompressed(const char*p,size_t n) {
	reset();
	dData=new char[n];
	dSize=n;
	memmove(dData,p,n);
	dIsOK=true;
	return *this;
}

PXer& PXer::setCompressed(const char*p,size_t n) {
	reset();
	cData=new char[n];
	cSize=n;
	memmove(cData,p,n);
	cIsOK=true;
	return *this;
}

PXer& PXer::setControlNib(int i, uint8_t v) {
	cFlag[i]=v&0xF;
	return *this;
}

PXer& PXer::setControlNibs(const uint8_t v[9]) {
	for(int i=0;i<9;i++)
		cFlag[i]=v[i];
	return *this;
}
