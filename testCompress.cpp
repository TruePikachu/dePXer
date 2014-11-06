#include "pxer.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
using namespace std;

int main(int argc, char *argv[]) {
	if(argc!=2) {
		cout << "Usage: " << argv[0] << " <any file>\n";
		return 0;
	}
	char buf[256];
	// Load the file
	ifstream file(argv[1]);
	file.seekg(0,file.end);
	size_t nLength = file.tellg();
	file.seekg(0);
	char *pData = new char[nLength];
	file.read(pData,nLength);
	file.close();
	sprintf(buf,"0x%X",nLength);
	clog << "Loaded '" << argv[1] << "' (" << buf << " bytes)\n";

	// Make our compressor
	PXer cPX;
	cPX.setDecompressed(pData,nLength);
	size_t cLength;
	const char *cData = cPX.getCompressed(cLength,true);

	// Make our decompressor
	PXer dPX;
	dPX.setCompressed(cData,cLength);
	dPX.setControlNibs(cPX.getControlNibs());
	size_t dLength;
	const char *dData = cPX.getDecompressed(dLength);

	// Check for equality
	if(dLength!=nLength) {
		clog << "ERROR: Decompressed data is a different length\n";
		return 1;
	}
	if(strncmp(dData,pData,nLength)) {
		clog << "ERROR: Decompressed data is different\n";
		return 1;
	}
	clog << "Decompressed data matches\n";
	return 0;
}
