#pragma once
#include "FfmpegDecodecBase.h"
class VideoDecodcEncodc :public FfmpegDecodecBase
{
public:
	VideoDecodcEncodc();
	~VideoDecodcEncodc();
	void decodec();
	void encodec();
};

