extern "C"
{
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
}

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>

static const size_t OUTPUT_SIZE_IN_BYTES = 128;

int main(int argc, char *argv[])
{
  if(argc <= 0 || !argv || !argv[1])
    return EXIT_FAILURE;

  std::ifstream file(argv[1]);

  if(file.is_open())
    {
      char H[OUTPUT_SIZE_IN_BYTES];
      char PI[] = "1415926535897932384626433832795028841971693"
	"9937510582097494459230781640628620899862803482534211"
	"706798214808651328230664709384460";
      char buffer[OUTPUT_SIZE_IN_BYTES];

      memcpy(H, PI, sizeof(H));

      while(true)
	{
	  file.read(buffer, sizeof(buffer));

	  size_t gcount = static_cast<size_t> (file.gcount());

	  if(gcount <= 0)
	    break;

	  if(gcount < sizeof(PI))
	    memcpy(&buffer[gcount], PI, sizeof(PI) - gcount);

	  char R[OUTPUT_SIZE_IN_BYTES];
	  uint64_t a[16];
	  uint64_t p[16];

	  memset(R, 0, sizeof(R));
	  memset(a, 0, sizeof(a));
	  memset(p, 0, sizeof(p));

	  for(size_t i = 0, j = 0; i < OUTPUT_SIZE_IN_BYTES; i++)
	    {
	      a[j] |= static_cast<uint64_t> (buffer[i] & 0xff) << 8 * i;

	      if((i + 1) % 8 == 0)
		j += 1;
	    }

	  for(size_t h = 13; h <= 75; h += 13)
	    for(size_t i = 0; i < 16; i++)
	      {
		long double b =
		  (static_cast<long double> (a[i]) /
		   std::numeric_limits<uint64_t>::max()) * h * M_PI;
		long double x0 = std::numeric_limits<uint64_t>::max() / 2 *
		  cosl(static_cast<double> (h) * b) * cosl(b);
		long double y0 = std::numeric_limits<uint64_t>::max() / 2 *
		  cosl(static_cast<double> (h) * b) * sinl(b);
		uint64_t x = std::llround(std::abs(std::ceil(x0)));
		uint64_t y = std::llround(std::abs(std::ceil(y0)));

		p[i] = x ^ y;
		a[i] = p[i];
	      }

	  for(size_t i = 0; i < 16; i++)
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
	    for(size_t j = 0; j < OUTPUT_SIZE_IN_BYTES; j++)
	      if(i != j)
		H[i] ^= R[j];
	}

      for(size_t i = 0; i < OUTPUT_SIZE_IN_BYTES; i++)
	printf("%02x", H[i] & 0xff);

      std::cout << std::endl;
    }

  file.close();
  return EXIT_SUCCESS;
}