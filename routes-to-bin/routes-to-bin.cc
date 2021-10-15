#include <map>
#include <vector>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "prefixtree.h"
#include "io.h"
#include "rnode.h"
#include "routes.h"

static struct {
  const char *chip;
  uint32_t width;

} widths[7] = {
  { "e50f",    4958 },
  { "gt75f",   6006 },
  { "gt150f",  7605 },
  { "gt300f", 10038 },
  { "gx25f",   3856 },
  { "sx50f",   6006 },
  { "sx120f",  7605 },
};

int main(int argc, char **argv)
{
  if(argc != 5) {
    fprintf(stderr, "Usage:\n%s data-dir chip target-dir <lzma-level>\n", argv[0]);
    exit(0);
  }

  std::string base_dir = argv[1];
  std::string chip = argv[2];
  std::string dest_dir = argv[3];

  uint32_t width = 0;
  for(int i=0; i != 7; i++)
    if(chip == widths[i].chip) {
      width = widths[i].width;
      break;
    }

  if(!width) {
    fprintf(stderr, "Chip %s unknown, no width available\n", chip.c_str());
    exit(1);
  }

  auto r_data = file_load(base_dir + '/' + chip + "-r.txt.xz");
  auto l_data = file_load(base_dir + '/' + chip + "-l.txt.xz");

  RNodeReader rnr;
  RoutesParser rparse(rnr, r_data, width);

  std::vector<uint8_t> output(700*1024*1024);
  uint8_t *opos = output.data();
  int nr = 0;
  while(rparse.rn) {
    memcpy(opos+ 0,  &rparse.rn, 4);
    memcpy(opos+ 4, &rparse.pattern, 4);
    memcpy(opos+ 8, &rparse.fw_pos,  4);
    memcpy(opos+12, rparse.sources, 44*4);
    opos += 188;
    nr++;
    rparse.next();
  }
  assert(opos-output.data() <= output.size());

  output.resize(opos-output.data());
  r_data.clear();
  l_data.clear();

  file_save(dest_dir + '/' + chip + "-r.bin", output, strtol(argv[4], nullptr, 10));

  exit(0);
}
