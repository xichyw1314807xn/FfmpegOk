#include "stdafx.h"
#include "VideoDecodcEncodc.h"

extern "C"
{
#include "SDL2/SDL.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}


VideoDecodcEncodc::VideoDecodcEncodc()
{
}


VideoDecodcEncodc::~VideoDecodcEncodc()
{
}

void VideoDecodcEncodc::decodec()
{
	AVFormatContext *pFormatCtx;
    printf("********* Hello video decodec **********\n");
	printf("%s", avcodec_configuration());
}

void VideoDecodcEncodc::encodec()
{
	printf("********* Hello video encodec **********\n");
	//printf("%s", avcodec_configuration());
}