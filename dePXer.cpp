#include "pxer.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
using namespace std;

int main(int argc, char *argv[]) {
	if(argc!=4) {
		cout << "Usage: " << argv[0] << " <c|a|x> <infile> <outfile>\n";
		cout << "  c - Compress PKDPX\n  a - Compress AT4PX\n  x - Extract either\n";
		return 0;
	}
	ifstream fin(argv[2]);
	ofstream fout(argv[3]);
	if(!strcmp(argv[1],"c"))
		throw runtime_error("Compression not implemented!");
	else if(!strcmp(argv[1],"a"))
		throw runtime_error("Compression not implemented!");
	else if(!strcmp(argv[1],"x")) {
		char type[5];
		fin.read(type,5);
		if(strncmp(type,"PKDPX",5) && strncmp(type,"AT4PX",5)) {
			cout << "Not a valid compressed file!\n";
			return 1;
		}
		uint16_t cLength;
		fin.read((char*)&cLength,2);
		uint8_t flags[9];
		fin.read((char*)flags,9);
		uint32_t dLength=0;
		if(type[0]=='P') {
			fin.read((char*)&dLength,4); cLength-=0x14;
		} else {
			fin.read((char*)&dLength,2); cLength-=0x12;
		}
		// Load the compressed data
		char* inData = new char[cLength];
		fin.read(inData,cLength);
		fin.close();
		// Init PXer
		PXer myPX(flags);
		myPX.setCompressed(inData,cLength);
		delete[] inData;

		// Get the decompressed data
		size_t realSize;
		const char* outData = myPX.getDecompressed(realSize);
		if(realSize!=dLength) {
			cout << "Expected and actual lengths differ: Wanted " << dLength << ", got " << realSize << endl;
			return 1;
		}
		fout.write(outData,dLength);
		fout.close();
	} else {
		cout << "Invalid operation: " << argv[1] << endl;
		return 1;
	}
	return 0;
}
