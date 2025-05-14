#include "audio.h"

#include <mikmod.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#include <resources.h>

#define PCM_DEVICE "default"

extern pthread_t audioThread;

typedef struct memReader_s {
	MREADER core;
	const void *buffer;
	long len;
	long pos;
} MemReader;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int keep_playing = 1;

pthread_t audioThread;

static BOOL memReader_Eof(MREADER* reader) {
	MemReader* mr = (MemReader*) reader;
	if (!mr) return 1;
	if (mr->pos >= mr->len) return 1;
	return 0;
}

static BOOL memReader_Read(MREADER* reader,void* ptr,size_t size) {
	unsigned char *d;
	const unsigned char *s;
	MemReader* mr;
	long siz;
	BOOL ret;

	if (!reader || !size || (size > (size_t) LONG_MAX))
		return 0;

	mr = (MemReader*) reader;
	siz = (long) size;
	if (mr->pos >= mr->len) return 0;	/* @ eof */
	if (mr->pos + siz > mr->len) {
		siz = mr->len - mr->pos;
		ret = 0; /* not enough remaining bytes */
	}
	else {
		ret = 1;
	}

	s = (const unsigned char *) mr->buffer;
	s += mr->pos;
	mr->pos += siz;
	d = (unsigned char *) ptr;

	while (siz) {
		*d++ = *s++;
		siz--;
	}

	return ret;
}

static int memReader_Get(MREADER* reader) {
	MemReader* mr;
	int c;

	mr = (MemReader*) reader;
	if (mr->pos >= mr->len) return EOF;
	c = ((const unsigned char*) mr->buffer)[mr->pos];
	mr->pos++;

	return c;
}

static int memReader_Seek(MREADER* reader,long offset,int whence) {
	MemReader* mr;

	if (!reader) return -1;
	mr = (MemReader*) reader;
	switch(whence)
	{
	case SEEK_CUR:
		mr->pos += offset;
		break;
	case SEEK_SET:
		mr->pos = reader->iobase + offset;
		break;
	case SEEK_END:
		mr->pos = mr->len + offset;
		break;
	default: /* invalid */
		return -1;
	}
	if (mr->pos < reader->iobase) {
		mr->pos = mr->core.iobase;
		return -1;
	}
	if (mr->pos > mr->len) {
		mr->pos = mr->len;
	}
	return 0;
}

static long memReader_Tell(MREADER* reader) {
	if (reader) {
		return ((MemReader*)reader)->pos - reader->iobase;
	}
	return 0;
}

MREADER *createMemReader(const void *buffer, long len) {
	MemReader* reader = (MemReader*)calloc(1, sizeof(MemReader));
	if (reader) {
		reader->core.Eof = &memReader_Eof;
		reader->core.Read= &memReader_Read;
		reader->core.Get = &memReader_Get;
		reader->core.Seek= &memReader_Seek;
		reader->core.Tell= &memReader_Tell;
		reader->buffer = buffer;
		reader->len = len;
		reader->pos = 0;
	}
	return (MREADER*)reader;
}

static void* audioThreadRoutine(void *arg) {
	const Ressource* music = (Ressource*)arg;

	MREADER *mem_reader = createMemReader(music->data, music->size);

	MODULE *module = Player_LoadGeneric(mem_reader, 64, 0);
#ifdef DEBUG
	if (module) {
#endif
		Player_Start(module);
		pthread_mutex_lock(&mutex);
		while (Player_Active() && keep_playing) {
			pthread_mutex_unlock(&mutex);
			MikMod_Update();
			usleep(10000);
			pthread_mutex_lock(&mutex);
		}
		pthread_mutex_unlock(&mutex);
		Player_Stop();
		Player_Free(module);
#ifdef DEBUG
	}
	else {
		fprintf(stderr, "Could not load module, reason: %s\n", MikMod_strerror(MikMod_errno));
	}
#endif

	return NULL;
}

void initAudio() {
	MikMod_RegisterAllDrivers();
	MikMod_RegisterAllLoaders();

	md_device = 0;
	md_mixfreq = 44100;
	md_mode |= DMODE_STEREO | DMODE_16BITS;
	md_reverb = 0;
#ifdef DEBUG
	if (MikMod_Init("")) {
		fprintf(stderr, "Could not initialize sound, reason: %s\n", MikMod_strerror(MikMod_errno));
		exit(EXIT_FAILURE);
	}
#else
	MikMod_Init("");
#endif
}

void playSound() {
#ifdef DEBUG
	if (pthread_create(&audioThread, NULL, audioThreadRoutine, (void*)&res_music_it) != 0) {
		fprintf(stderr, "Failed to create thread\n");
		exit(EXIT_FAILURE);
	}
#else
	pthread_create(&audioThread, NULL, audioThreadRoutine, (void*)&res_music_it);
#endif
}

void stopSound() {
	pthread_mutex_lock(&mutex);
	keep_playing = 0;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

void cleanupAudio() {
	pthread_join(audioThread, NULL);
	MikMod_Exit();
}
