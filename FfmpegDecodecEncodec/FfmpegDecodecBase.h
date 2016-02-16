#pragma once
class FfmpegDecodecBase
{
public:
	FfmpegDecodecBase();
	~FfmpegDecodecBase();

	virtual void decodec();
	virtual void encodec();
};

