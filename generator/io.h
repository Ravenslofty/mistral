#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <vector>
#include <string>

extern int lookup_int(const uint8_t *&p);
extern float lookup_float(const uint8_t *&p);
extern void skipsp(const uint8_t *&p);

extern std::vector<uint8_t> file_load(std::string fname);
extern void file_save(std::string fname, const std::vector<uint8_t> &output, int level);

#endif
