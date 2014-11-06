#include "pxer.hpp"
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <vector>
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
	}
	// Decompression algorithm
	const char* readPos = cData;		// Position we read from
	vector< char > dBuffer;			// Decompressed data buffer
	char curCmdByte;			// Current command byte
	int cmdBitsLeft=0;			// Number of bits remaining in the command byte
	while(readPos<(cData+cSize)) {		// Main loop
		// Handle the possible lack of command bits
		if(!cmdBitsLeft) {
			curCmdByte = *(readPos++);
			cmdBitsLeft=8;
		}
		bool cmdBit = curCmdByte&0x80;
		curCmdByte<<=1;cmdBitsLeft--;
		if(cmdBit)
			// Copy 1 byte to output
			dBuffer.push_back(*(readPos++));
		else {
			// Use the next byte as instruction
			uint8_t nbHigh = (*readPos & 0xF0) >> 4;
			uint8_t nbLow = (*(readPos++) & 0xF);
			// Check if nbHigh is a control flag
			int ctlFlagID = -1;
			for(int i=0;i<9;i++)
				if(nbHigh==cFlag[i])
					ctlFlagID=i;
			if(ctlFlagID!=-1) {
				// It is a control flag
				// Add 4 nibs to output
				uint8_t n[4];
				n[0]=nbLow;
				n[1]=nbLow;
				n[2]=nbLow;
				n[3]=nbLow;
				switch(ctlFlagID) {
					case 0:
						break;
					case 1:
						for(int i=0;i<4;i++)
							n[i]++;
					case 2:
					case 3:
					case 4:
						n[ctlFlagID-1]--;
						break;
					case 5:
						for(int i=0;i<4;i++)
							n[i]--;
					case 6:
					case 7:
					case 8:
						n[ctlFlagID-5]++;
						break;
				}
				dBuffer.push_back((n[0]<<4) | n[1]);
				dBuffer.push_back((n[2]<<4) | n[3]);
			} else {
				// It isn't a control flag
				int16_t offset = -0x1000 + ((nbLow<<8) | (uint8_t)(*(readPos++)));
				for(int i=0;i<(nbHigh+3);i++)
					dBuffer.push_back(dBuffer[dBuffer.size()+offset]);
			}
		}
	}
	dSize = dBuffer.size();
	dData = new char[dSize];
	for(int i=0;i<dSize;i++)
		dData[i]=dBuffer[i];
	dIsOK=true;
	n=dSize;
	return dData;
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

const char* PXer::getCompressed(size_t& n, bool computeFlags) {
	if(cIsOK) {
		n=cSize;
		return cData;
	} else {
		if(computeFlags)
			// FIXME Right now we are just using hardcoded control nibs
			// Set up computation sometime
			for(int i=0;i<9;i++)
				cFlag[i]=i+0xF-8;
		// Our compressed data is at MOST 12.5% longer than the decompressed
		// (this is for every command bit being 1)
		cSize=dSize*9/8 + 1;
		cData=new char[cSize];
		char *writePtr = cData;		// Where we are writing data
		char *wCmdBit;			// Where we are writing the cmd bit
		int nBitsFree=0;		// Number of bits left in the cmd
		const char*readPtr = dData;	// Where we are reading data
		while(readPtr<(dData+dSize)) {
			// Move the command pointer, if needed
			if(!nBitsFree) {
				wCmdBit=writePtr++;
				nBitsFree=8;
			}
			// Check if we can use the block copy
			// TODO Code this up
			
			// Check if we can use the 2nib copy
			// TODO Code this up

			// Use the 1-byte copy as a fallback
			*(writePtr++) = *(readPtr++);
			*wCmdBit |= 1 << --nBitsFree;
		}
		// Finish up
		n = writePtr-cData;
		cSize=n;
		return cData;
	}
}

const uint8_t* PXer::getControlNibs() const {
	return cFlag;
}

uint8_t* PXer::getControlNibs() {
	return cFlag;
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
