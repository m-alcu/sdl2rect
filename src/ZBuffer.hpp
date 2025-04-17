#pragma once

#include <limits>
#include <cassert>
#include <algorithm>

class ZBuffer
{
public:
	ZBuffer( int width,int height )
		:
		width( width ),
		height( height ),
		pBuffer( new float[width*height] )
	{}
	ZBuffer( const ZBuffer& ) = delete;
	~ZBuffer()
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}
	void Clear()
	{
		std::fill_n(
			pBuffer,
			width * height,
			std::numeric_limits<float>::infinity() // Initialize zBuffer to the maximum float value
		);
	}
	bool TestAndSet( int pos,float depth )
	{
		float& depthInBuffer = pBuffer[pos];
		if( depth < depthInBuffer )
		{
			depthInBuffer = depth;
			return true;
		}
		return false;
	}
private:
	int width;
	int height;
	float* pBuffer = nullptr;
};