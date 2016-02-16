#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tchar.h"

#include "AudioDecodcEncodc.h"

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
extern "C"
{
#include "SDL2/SDL.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};   
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <SDL2/SDL.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif
#endif


#define MAX_AUDIO_FRAME_SIZE 192000//1 second of 48kHz 32bit audio

//Output PCM 
#define OUTPUT_PCM 1;
//Use SDL
#define USE_SDL 1;

//Buffer
//|------------|-------------|
//chunk--------pos--------len|
static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;

/*
*Audio Callback
*The audio funtion callback takes the following parameters
*stream:A pointer to the audio buffer to be filled
*len: The lenght (in bytes)of the audio buffer
*/
void fill_audio(void *udata ,Uint8 *stream ,int len)
{
	//SDL 2.0
	SDL_memset(stream,0,len);
	if (audio_len == 0)
	{
		return;
	}
	len = len > audio_len ? audio_len : len;

	SDL_MixAudio(stream, audio_pos, len,SDL_MIX_MAXVOLUME);
	audio_len -= len;
	audio_pos += len;


}




AudioDecodcEncodc::AudioDecodcEncodc()
{
}


AudioDecodcEncodc::~AudioDecodcEncodc()
{
}

void AudioDecodcEncodc::decodec()
{
	AVFormatContext *pFormatCtx;
	int              i, audioStream;
	AVCodecContext   *pCodecCtx;
	AVCodec          *pCodec;
	AVPacket         *packet;
	AVFrame          *pFrame;
	SDL_AudioSpec    wanted_spec;
	uint8_t          *out_buffer;
	int ret;
	uint32_t         len = 0;
	int              got_picture;
	int              index=0;
	int64_t          in_channel_layout;
	struct           SwrContext *au_convert_ctx;

	FILE *pFile = NULL;
	char url[] = "Titanic.ts";


	av_register_all();
	avformat_network_init();

	pFormatCtx = avformat_alloc_context();

	//Open 
	if (avformat_open_input(&pFormatCtx,url,NULL,NULL)<0)
	{
		printf("Count't open input stream");
		return ;
	}
	//Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx ,NULL)<0)
	{
		printf("Count't find stream information.\n");
		return;
	}
	//Dump vaild information onto standard error
	av_dump_format(pFormatCtx,0,url,0);

	//Find the frist audio stream 

	audioStream = -1;

	for (i = 0; i < pFormatCtx->nb_streams;i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			break;
		}
	}

	if (audioStream == -1)
	{
		printf("Didn't find a audio stream.\n");
		return;
	}
	pCodecCtx = pFormatCtx->streams[audioStream]->codec;
	
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return;
	}
	//Open Codec
	if (avcodec_open2(pCodecCtx ,pCodec,NULL)<0)
	{
		printf("Cound not open codec. \n");
		return;
	}

#if OUTPUT_PCM
	pFile = fopen("output.pcm","wb+");
#endif


	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);

	//Out Audio Param
	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//nb_samples:AAC-1024 MP#-1152
	int out_nb_sample = pCodecCtx->frame_size;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = 48000;
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer size
	int out_buffer_size = av_samples_get_buffer_size(NULL,out_channels,out_nb_sample,out_sample_fmt,1);

	out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE *2);
	pFrame = av_frame_alloc();

	//SDL------------------------------
#if USE_SDL
	//Init
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER))
	{
		printf("Cound not initialize SDL - %s \n",SDL_GetError());
		return;
	}
	//SDL_AudioSpes
	wanted_spec.freq = out_sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = out_channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = out_nb_sample;
	wanted_spec.callback = fill_audio;
	wanted_spec.userdata = pCodecCtx;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
	{
		printf("can't open audio.\n");
		return;
	}
#endif
	//FIX:Some Codec's Context Information is missing
	in_channel_layout = av_get_default_channel_layout(pCodecCtx->channels);
	//Swr
	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,out_channel_layout,out_sample_fmt,out_sample_rate,in_channel_layout,pCodecCtx->sample_fmt,pCodecCtx->sample_rate,0,NULL);
	swr_init(au_convert_ctx);

	while (av_read_frame(pFormatCtx,packet)>=0)
	{
		if (packet ->stream_index == audioStream)
		{
			ret = avcodec_decode_audio4(pCodecCtx,pFrame,&got_picture,packet);
			if (ret <0)
			{
				printf("error in decoding audio frame.\n");
				return;
			}
			if (got_picture)
			{
				swr_convert(au_convert_ctx,&out_buffer,MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data,pFrame->nb_samples);
#if 1
				printf("index:%5d\t pts:%lld\t packet size:%d\n", index, packet->pts, packet->size);
#endif
#if OUTPUT_PCM
				//Write PCM
				fwrite(out_buffer,1,out_buffer_size,pFile);
#endif
				index++;
			}
#if USE_SDL
			while (audio_len >0)
			{
				SDL_Delay(1);
			}
				//Set audio buffer
				audio_chunk = (Uint8 *)out_buffer;
				//Audio buffer lenght
				audio_len = out_buffer_size;
				audio_pos = audio_chunk;
				//Play
				SDL_PauseAudio(0);
			
#endif
		}
		av_free_packet(packet);
	}
	swr_free(&au_convert_ctx);
#if USE_SDL
	SDL_CloseAudio();
	SDL_Quit();
#endif
#if OUTPUT_PCM 
    fclose(pFile);
#endif
	av_free(out_buffer);
	//Close the codec
	avcodec_close(pCodecCtx);
	//Close the video file
	avformat_close_input(&pFormatCtx);
















}

void AudioDecodcEncodc::encodec()
{
	
}

