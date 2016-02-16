#pragma once
class FfmpegPlayBase
{
public:
	FfmpegPlayBase();
	~FfmpegPlayBase();

	virtual void playLocalFile();
	virtual void playHttpFile();
};

