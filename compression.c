#include <stdio.h>
#include <string.h>

#include "zlib.h"
#include "pdb.h"

#define CHUNK 16384

int def(const char *filename_in, const char *filename_out, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    FILE *source;
    FILE *dest;

    fprintf(stdout, "Compressing database: %s\n", filename_out);
    if((source = fopen(filename_in, "rb")) == NULL){
        perror("fopen");
        return -1;
    }
    if((dest = fopen(filename_out, "ab")) == NULL){
        perror("fopen");
        return -1;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if(ret != Z_OK)
        return ret;

    do{
        strm.avail_in = fread(in, 1, CHUNK, source);
        if(ferror(source)){
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        do{
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    
            have = CHUNK - strm.avail_out;
            if(fwrite(out, 1, have, dest) != have || ferror(dest)){
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        }while (strm.avail_out == 0);

    }while (flush != Z_FINISH);

    (void)deflateEnd(&strm);
    fclose(source);
    fflush(dest);
    fclose(dest);
    return Z_OK;
}

int inf(const char *filename_in, const char *filename_out)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    FILE *source;
    FILE *dest;

    fprintf(stdout, "Decompressing database %s\n", filename_in);
    if((source = fopen(filename_in, "rb")) == NULL){
        perror("fopen");
        return -1;
    }
    if((dest = fopen(filename_out, "ab")) == NULL){
        perror("fopen");
        return -1;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if(ret != Z_OK)
        return ret;

    do{
        strm.avail_in = fread(in, 1, CHUNK, source);
        if(ferror(source)){
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if(strm.avail_in == 0)
            break;
        strm.next_in = in;

        do{
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret){
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if(fwrite(out, 1, have, dest) != have || ferror(dest)){
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        }while (strm.avail_out == 0);

    }while (ret != Z_STREAM_END);

    (void)inflateEnd(&strm);
    fclose(source);
    fflush(dest);
    fclose(dest);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int zerr(int ret)
{
    if(ret != 0)
        fprintf(stderr, "pdb: ");
    switch (ret) {
        case Z_DATA_ERROR:
            fprintf(stdout, "invalid or incomplete deflate data\n");
            break;
        case Z_MEM_ERROR:
            fprintf(stderr, "out of memory\n");
            break;
        case Z_VERSION_ERROR:
            fprintf(stderr, "zlib version mismatch!\n");
    }
    return ret;
}

