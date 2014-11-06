#ifndef __PXER_HPP
#define __PXER_HPP
class PXer;
#include <cstdio>
#include <stdint.h>

class PXer {
	private:
		char*	dData;	// Decompressed data
		size_t	dSize;	// Decompressed size
		bool	dIsOK;	// TRUE if decompressed stuff is valid
		char*	cData;	// Compressed data
		size_t	cSize;	// Compressed size
		bool	cIsOK;	// TRUE if compressed is valid
		uint8_t	cFlag[9];//Control nibs
		void	reset();
	public:
				 PXer		();
				 PXer		(const uint8_t[9]);
				 PXer		(const PXer&);
				~PXer		();
		PXer&		operator=	(PXer);

		const char*	getDecompressed	(size_t&) const;
		const char*	getDecompressed	(size_t&);
		const char*	getCompressed	(size_t&) const;
		const char*	getCompressed	(size_t&,bool=false);

		PXer&		setDecompressed	(const char*,size_t);
		PXer&		setCompressed	(const char*,size_t);
		PXer&		setControlNib	(int,uint8_t);
		PXer&		setControlNibs	(const uint8_t[9]);
};

#endif
