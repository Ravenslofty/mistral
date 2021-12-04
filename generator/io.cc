#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lzma.h>

int lookup_int(const uint8_t *&p)
{
  if(*p < '0' || *p > '9')
    return -1;
  int res = *p++ - '0';
  while(*p >= '0' && *p <= '9')
    res = 10*res + *p++ - '0';
  return res;  
}

float lookup_float(const uint8_t *&p)
{
  char buf[64];
  char *bp = buf;
  for(;;) {
    char c = *p;
    if((c < '0' || c > '9') && c != 'e' && c != '.' && c != '-')
      break;
    *bp++ = c;
    assert(bp-buf != sizeof(buf));
    p++;
  }
  *bp = 0;
  return strtod(buf, nullptr);
}

void skipsp(const uint8_t *&p)
{
  while(*p == ' ')
    p++;
}

std::vector<uint8_t> file_load(std::string fname)
{
  std::vector<uint8_t> data;
  std::string msg = "Open " + fname;

  FILE *fd = fopen(fname.c_str(), "rb");
  if(!fd) {
    perror(msg.c_str());
    exit(2);
  }

  fseek(fd, 0, SEEK_END);
  size_t size = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  data.resize(size);
  fread(data.data(), size, 1, fd);
  fclose(fd);

  if(!memcmp(data.data(), "\xfd" "7zXZ", 6)) {
    const uint8_t *fptr = data.data() + data.size() - 12;
    lzma_stream_flags stream_flags;
    lzma_ret ret;

    ret = lzma_stream_footer_decode(&stream_flags, fptr);
    const uint8_t *iptr = fptr - stream_flags.backward_size;
    lzma_index *index = nullptr;
    uint64_t memlimit = UINT64_MAX;
    size_t pos = 0;
    lzma_index_buffer_decode(&index, &memlimit, nullptr, iptr, &pos, fptr - iptr);
    size_t size = lzma_index_uncompressed_size(index);
    lzma_index_end(index, nullptr);
    std::vector<uint8_t> ndata(size);

    lzma_stream strm = LZMA_STREAM_INIT;
    if ((ret = lzma_stream_decoder(&strm, UINT64_MAX, 0)) != LZMA_OK) {
      fprintf(stderr, "failed to initialise liblzma: %d\n", ret);
      exit(1);
    }

    strm.next_in = data.data();
    strm.avail_in = data.size();
    strm.next_out = ndata.data();
    strm.avail_out = ndata.size();
  
    if((ret = lzma_code(&strm, LZMA_RUN)) != LZMA_STREAM_END) {
      fprintf(stderr, "rmux data decompression failure: %d\n", ret);
      exit(1);
    }

    data = std::move(ndata);
  }

  return data;
}

void file_save(std::string fname, const std::vector<uint8_t> &output, int level)
{
  std::vector<uint8_t> compressed(output.size());
  size_t compressed_size = 0;

  if(level) {
    auto ret = lzma_easy_buffer_encode(level, LZMA_CHECK_NONE, nullptr, output.data(), output.size(), compressed.data(), &compressed_size, compressed.size());
    if(ret != LZMA_OK) {
      fprintf(stderr, "LZMA compression failure %d\n", ret);
      exit(1);
    }
  }

  std::string msg = "Open " + fname;
  
  FILE *fd = fopen(fname.c_str(), "wb");
  if(!fd) {
    perror(msg.c_str());
    exit(2);
  }

  if(level)
    fwrite(compressed.data(), compressed_size, 1, fd);
  else
    fwrite(output.data(), output.size(), 1, fd);

  fclose(fd);
}

