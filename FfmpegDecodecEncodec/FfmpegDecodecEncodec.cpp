// FfmpegDecodecEncodec.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FfmpegDecodecBase.h"
#include "AudioDecodcEncodc.h"
#include "VideoDecodcEncodc.h"
#include "PictureDecodcEncodec.h"


#define AUDIO_DECODC 0
#define VIDEO_DECODC 1
#define PICTURE_DECODC 0

int _tmain(int argc, _TCHAR* argv[])
{
#if AUDIO_DECODC
	FfmpegDecodecBase *pAudioFfmpegBase = new AudioDecodcEncodc();
	pAudioFfmpegBase->decodec();
	pAudioFfmpegBase->encodec();
#endif

#if VIDEO_DECODC
	FfmpegDecodecBase *pVideoFfmpegBase = new VideoDecodcEncodc();
	pVideoFfmpegBase->decodec();
	pVideoFfmpegBase->encodec();
#endif
#if PICTURE_DECODC
	FfmpegDecodecBase *pPictureFfmpegBase = new PictureDecodcEncodec();
	pPictureFfmpegBase->decodec();
	pPictureFfmpegBase->encodec();
#endif

	return 0;
}

