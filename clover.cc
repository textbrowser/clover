/*
** Copyright (c) Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from clover without specific prior written permission.
**
** CLOVER IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** CLOVER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

extern "C"
{
#include <string.h>
}

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>

static size_t OUTPUT_SIZE_IN_BYTES = 128;

int main(int argc, char *argv[])
{
  if(argc <= 0 || !argv || !argv[1])
    {
      std::cerr << "clover: --filename file --output-size bytes" << std::endl;
      return EXIT_FAILURE;
    }

  int fileNameIdx = -1;

  for(int i = 0; i < argc; i++)
    if(argv[i] && strcmp(argv[i], "--filename") == 0)
      {
	i += 1;

	if(i < argc && argv[i])
	  fileNameIdx = i;
      }
    else if(argv[i] && strcmp(argv[i], "--output-size") == 0)
      {
	i += 1;

	if(i < argc && argv[i])
	  {
	    auto const value = std::strtol(argv[i], 0, 10);

	    if(value < 32 || value > 128)
	      return EXIT_FAILURE;
	    else
	      OUTPUT_SIZE_IN_BYTES = value;
	  }
      }

  if(fileNameIdx < 0)
    return EXIT_FAILURE;

  std::ifstream file(argv[fileNameIdx]);

  if(file.is_open())
    {
      char H[OUTPUT_SIZE_IN_BYTES];
      char PI[OUTPUT_SIZE_IN_BYTES];
      char buffer[OUTPUT_SIZE_IN_BYTES];

      memcpy(PI,
	     "141592653589793238462643383279502884197169399375"
	     "105820974944592307816406286208998628034825342117"
	     "06798214808651328230664709384460",
	     std::min(sizeof(PI), static_cast<size_t> (128)));
      memcpy(H, PI, std::min(sizeof(H), sizeof(PI)));

      while(true)
	{
	  file.read(buffer, sizeof(buffer));

	  auto const gcount = static_cast<size_t> (file.gcount());

	  if(gcount <= 0)
	    break;

	  if(gcount < sizeof(PI))
	    memcpy(&buffer[gcount], PI, sizeof(PI) - gcount);

	  char R[OUTPUT_SIZE_IN_BYTES];
	  uint64_t a[OUTPUT_SIZE_IN_BYTES / 8];
	  uint64_t p[OUTPUT_SIZE_IN_BYTES / 8];

	  memset(R, 0, sizeof(R));
	  memset(a, 0, sizeof(a));
	  memset(p, 0, sizeof(p));

	  for(size_t i = 0, j = 0, k = 0; i < OUTPUT_SIZE_IN_BYTES; i++)
	    {
	      a[j] |= static_cast<uint64_t> (buffer[i] & 0xff) << 8 * k;

	      if((i + 1) % 8 == 0)
		{
		  j += 1;
		  k = 0;
		}
	      else
		k += 1;

	      if(j >= sizeof(a) / sizeof(a[0]))
		break;
	    }

	  for(size_t h = 13; h <= 75; h += 13)
	    {
	      for(size_t i = 0; i < OUTPUT_SIZE_IN_BYTES / 8; i++)
		{
		  auto const b =
		    (static_cast<long double> (a[i]) /
		     std::numeric_limits<uint64_t>::max()) * h * M_PI;
		  auto const x0 = std::numeric_limits<uint64_t>::max() / 2 *
		    cosl(static_cast<double> (h) * b) * cosl(b);
		  auto const y0 = std::numeric_limits<uint64_t>::max() / 2 *
		    cosl(static_cast<double> (h) * b) * sinl(b);
		  auto const x = std::llround(std::ceil(x0));
		  auto const y = std::llround(std::ceil(y0));

		  p[i] = x ^ y;
		  a[i] = p[i];
		}

	      for(size_t i = 0; i < OUTPUT_SIZE_IN_BYTES; i++)
		for(size_t j = 0; j < OUTPUT_SIZE_IN_BYTES / 8; j++)
		  {
		    H[i] ^= static_cast<char> (a[j]);
		    H[i] ^= static_cast<char> (a[j] << 11);
		    H[i] ^= static_cast<char> (a[j] << 13);
		    H[i] ^= static_cast<char> (a[j] << 17);
		  }
	    }

	  for(size_t i = 0; i < OUTPUT_SIZE_IN_BYTES / 8; i++)
	    {
	      R[i * 8 + 0] = static_cast<char> (p[i]);
	      R[i * 8 + 1] = static_cast<char> ((p[i] >> 8) & 0xff);
	      R[i * 8 + 2] = static_cast<char> ((p[i] >> 16) & 0xff);
	      R[i * 8 + 3] = static_cast<char> ((p[i] >> 24) & 0xff);
	      R[i * 8 + 4] = static_cast<char> ((p[i] >> 32) & 0xff);
	      R[i * 8 + 5] = static_cast<char> ((p[i] >> 40) & 0xff);
	      R[i * 8 + 6] = static_cast<char> ((p[i] >> 48) & 0xff);
	      R[i * 8 + 7] = static_cast<char> ((p[i] >> 56) & 0xff);
	    }

	  for(size_t i = 0; i < OUTPUT_SIZE_IN_BYTES; i++)
	    H[i] ^= R[i];
	}

      for(size_t i = 0; i < OUTPUT_SIZE_IN_BYTES; i++)
	printf("%02x", H[i] & 0xff);

      std::cout << std::endl;
    }

  file.close();
  return EXIT_SUCCESS;
}
