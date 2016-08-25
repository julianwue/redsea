/*
 * redsea - RDS decoder
 * Copyright (c) Oona Räisänen OH2EIQ (windyoona@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "redsea.h"

#include <getopt.h>
#include <iostream>

#include "bits2blocks.h"
#include "groups.h"

namespace redsea {

void printShort(Station station) {
    printf("%s 0x%04x %s\n", station.getPS().c_str(), station.getPI(),
        station.getRT().c_str());

  //printf("%04x %2d%s TP:%d PTY:%d\n", station.pi, group.type,
  //    group.type_ab == 1 ? "B" : "A",
  //    group.tp, group.pty);
}

} // namespace redsea

int main(int argc, char** argv) {

  int option_char;
  int input_type = redsea::INPUT_MPX;

  while ((option_char = getopt(argc, argv, "b")) != EOF) {
    switch (option_char) {
      case 'b':
        input_type = redsea::INPUT_ASCIIBITS;
        break;
      case '?':
        break;
    }
  }

  redsea::BlockStream block_stream(input_type);
  std::map<uint16_t, redsea::Station> stations;

  uint16_t pi=0, prev_new_pi=0, new_pi=0;

  int group_counter = 0;
  while (!block_stream.isEOF()) {
    auto blockbits = block_stream.getNextGroup();

    if (blockbits.size() == 0)
      continue;

    group_counter ++;

    prev_new_pi = new_pi;
    new_pi = blockbits[0];

    if (new_pi == prev_new_pi) {
      pi = new_pi;

    } else if (new_pi != pi) {
      continue;
    }

    redsea::Group group(blockbits);

    if (stations.find(pi) != stations.end()) {
      stations[pi].update(group);
    } else {
      stations.insert({pi, redsea::Station(pi)});
      stations[pi].update(group);
    }

    //printShort(stations[pi]);
  }
}
