#pragma once
#include "FfmpegDecodecBase.h"
class PictureDecodcEncodec :public FfmpegDecodecBase
{
public:
	PictureDecodcEncodec();
	~PictureDecodcEncodec();
	void decodec();
	void encodec();
};

