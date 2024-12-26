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

#ifndef _clover_h_
#define _clover_h_

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sstream>

#define CLOVER_VERSION 20241225

class clover
{
 public:
  clover(const size_t output_size_in_bytes):
    m_output_size_in_bytes
    (std::max(output_size_in_bytes,
	      static_cast<decltype(m_output_size_in_bytes)> (8)))
  {
    m_H = new char[m_output_size_in_bytes];
    m_PI = new char[m_output_size_in_bytes];
    m_R = new char[m_output_size_in_bytes];
    m_a = new uint64_t[m_output_size_in_bytes / 8];
    m_buffer = new char[m_output_size_in_bytes];
    m_p = new uint64_t[m_output_size_in_bytes / 8];
    std::memcpy
      (m_PI,
       "141592653589793238462643383279502884197169399375"
       "105820974944592307816406286208998628034825342117"
       "06798214808651328230664709384460",
       m_output_size_in_bytes);
    std::memcpy(m_H, m_PI, m_output_size_in_bytes);
  }

  ~clover()
  {
    delete []m_H;
    delete []m_PI;
    delete []m_R;
    delete []m_a;
    delete []m_buffer;
    delete []m_p;
  }

  std::string output(void) const
  {
    std::ostringstream s;

    for(decltype(m_output_size_in_bytes) i = 0;
	i < m_output_size_in_bytes;
	i++)
      s << std::hex << std::setfill('0') << std::setw(2) << (m_H[i] & 0xff);

    return s.str();
  }

  void add(const char *data, const size_t size)
  {
    if(data == nullptr || size <= 0)
      return;

    auto const static pi = std::acos(-1);

    std::memset(m_R, 0, m_output_size_in_bytes * sizeof(*m_R));
    std::memset(m_a, 0, m_output_size_in_bytes / 8 * sizeof(*m_a));
    std::memset(m_p, 0, m_output_size_in_bytes / 8 * sizeof(*m_p));
    std::memcpy(m_buffer, data, std::min(m_output_size_in_bytes, size));

    if(m_output_size_in_bytes > size)
      std::memcpy(&m_buffer[size], m_PI, m_output_size_in_bytes - size);

    for(decltype(m_output_size_in_bytes) i = 0, j = 0, k = 0;
	i < m_output_size_in_bytes;
	i++)
      {
	m_a[j] |= static_cast<uint64_t> (m_buffer[i] & 0xff) << 8 * k;

	if((i + 1) % 8 == 0)
	  {
	    j += 1;
	    k = 0;
	  }
	else
	  k += 1;

	if(j >= m_output_size_in_bytes / 8)
	  break;
      }

    for(decltype(m_output_size_in_bytes) h = 13; h <= 75; h += 13)
      {
	for(decltype(m_output_size_in_bytes) i = 0;
	    i < m_output_size_in_bytes / 8;
	    i++)
	  {
	    auto const b =
	      (static_cast<long double> (m_a[i]) /
	       std::numeric_limits<uint64_t>::max()) * h * pi;
	    auto const x0 = std::numeric_limits<uint64_t>::max() / 2 *
	      cosl(b * static_cast<double> (h)) * cosl(b);
	    auto const y0 = std::numeric_limits<uint64_t>::max() / 2 *
	      cosl(b * static_cast<double> (h)) * sinl(b);
	    auto const x = static_cast<uint64_t> (std::llround(std::ceil(x0)));
	    auto const y = static_cast<uint64_t> (std::llround(std::ceil(y0)));

	    m_a[i] = m_p[i] = x ^ y;
	  }

	for(decltype(m_output_size_in_bytes) i = 0;
	    i < m_output_size_in_bytes;
	    i++)
	  for(decltype(m_output_size_in_bytes) j = 0;
	      j < m_output_size_in_bytes / 8;
	      j++)
	    {
	      m_H[i] ^= static_cast<char> (m_a[j]);
	      m_H[i] ^= static_cast<char> (m_a[j] << 11);
	      m_H[i] ^= static_cast<char> (m_a[j] << 13);
	      m_H[i] ^= static_cast<char> (m_a[j] << 17);
	    }
      }

    for(decltype(m_output_size_in_bytes) i = 0;
	i < m_output_size_in_bytes / 8;
	i++)
      {
	m_R[i * 8 + 0] = static_cast<char> (m_p[i]);
	m_R[i * 8 + 1] = static_cast<char> ((m_p[i] >> 8) & 0xff);
	m_R[i * 8 + 2] = static_cast<char> ((m_p[i] >> 16) & 0xff);
	m_R[i * 8 + 3] = static_cast<char> ((m_p[i] >> 24) & 0xff);
	m_R[i * 8 + 4] = static_cast<char> ((m_p[i] >> 32) & 0xff);
	m_R[i * 8 + 5] = static_cast<char> ((m_p[i] >> 40) & 0xff);
	m_R[i * 8 + 6] = static_cast<char> ((m_p[i] >> 48) & 0xff);
	m_R[i * 8 + 7] = static_cast<char> ((m_p[i] >> 56) & 0xff);
      }

    for(decltype(m_output_size_in_bytes) i = 0;
	i < m_output_size_in_bytes;
	i++)
      m_H[i] ^= m_R[i];
  }

 private:
  char *m_H = nullptr;
  char *m_PI = nullptr;
  char *m_R = nullptr;
  char *m_buffer = nullptr;
  size_t m_output_size_in_bytes = 0;
  uint64_t *m_a = nullptr;
  uint64_t *m_p = nullptr;
};

#endif
