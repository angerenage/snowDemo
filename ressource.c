#include "ressource.h"

static void lzmaError(lzma_ret ret) {
	switch (ret) {
		case LZMA_MEM_ERROR:
			fprintf(stderr, "Memory allocation failed\n");
			break;
		case LZMA_FORMAT_ERROR:
			fprintf(stderr, "Input is not in the xz format\n");
			break;
		case LZMA_OPTIONS_ERROR:
			fprintf(stderr, "Unsupported compression options\n");
			break;
		case LZMA_DATA_ERROR:
			fprintf(stderr, "Corrupted input data\n");
			break;
		case LZMA_BUF_ERROR:
			fprintf(stderr, "No progress is possible, buffer too small?\n");
			break;
		default:
			fprintf(stderr, "Unknown error: %d\n", ret);
	}
}

int loadRessource(const char *sourceFile, void **data, size_t *data_size) {
	FILE *source = fopen(sourceFile, "rb");
	if (source == NULL) {
		perror("File open error");
		return -1;
	}

	lzma_stream strm = LZMA_STREAM_INIT;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];
	unsigned char *result = NULL, *temp_ptr;
	size_t result_size = 0;
	lzma_ret ret;

	ret = lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED);
	if (ret != LZMA_OK) {
		fprintf(stderr, "Failed to initialize lzma decoder: %d\n", ret);
		lzmaError(ret);
		fclose(source);
		return -1;
	}

	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			perror("File read error");
			lzma_end(&strm);
			fclose(source);
			free(result);
			return -1;
		}
		strm.next_in = in;

		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;

			ret = lzma_code(&strm, strm.avail_in == 0 ? LZMA_FINISH : LZMA_RUN);

			if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
				fprintf(stderr, "Decompression error: %d\n", ret);
				lzmaError(ret);
				lzma_end(&strm);
				fclose(source);
				free(result);
				return -1;
			}

			size_t have = CHUNK - strm.avail_out;
			temp_ptr = realloc(result, result_size + have);
			if (!temp_ptr) {
				perror("Memory allocation error");
				lzma_end(&strm);
				fclose(source);
				free(result);
				return -1;
			}
			result = temp_ptr;
			memcpy(result + result_size, out, have);
			result_size += have;

		} while (strm.avail_out == 0);

	} while (ret != LZMA_STREAM_END);

	lzma_end(&strm);
	fclose(source);

	if (ret != LZMA_STREAM_END) {
		fprintf(stderr, "File decompression did not end properly\n");
		lzmaError(ret);
		free(result);
		return -1;
	}

	*data = result;
	*data_size = result_size;

	return 0;
}