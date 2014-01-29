/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   md5/MD5hash.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Binary.h"
#include "MD5hash.h"


uint4 
MD5hash::r[64]=
  {
    7, 12, 17, 22, 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
  };

// floor(abs(sin(i+1))*2**32);
uint4 MD5hash::k[64]=
  {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x2441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
  };

MD5hash::MD5hash()
  /*!
    Constructor
   */
{
  init();
}

MD5hash::MD5hash(const MD5hash& A) : 
  h0(A.h0),h1(A.h1),h2(A.h2),h3(A.h3),Item(A.Item),
  cNum(A.cNum)
  /*!
    Copy constructor
    \param A :: MD5hash to copy
  */
{}

MD5hash&
MD5hash::operator=(const MD5hash& A)
  /*!
    Assignment operator
    \param A :: MD5hash to copy
    \return *this
  */
{
  if (this!=&A)
    {
      h0=A.h0;
      h1=A.h1;
      h2=A.h2;
      h3=A.h3;
      Item=A.Item;
      cNum=A.cNum;
    }
  return *this;
}


void
MD5hash::init()
  /*!
    Initialize the system
   */
{
  //Initialize variables:
  h0 = 0x67452301;
  h1 = 0xEFCDAB89;
  h2 = 0x98BADCFE;
  h3 = 0x10325476;
  cNum=0;
  for(int i=0;i<16;i++)
    Item.W[i]=0;
  return;
}

int
MD5hash::getNextUnit(const std::string& A)
 /*!
   Get the next unit
   \param A :: String we are processing
  */
{
  ELog::RegMethod RegA("MD5hash","getNextUnit");
  const size_t AS=A.size();
  const size_t N=(AS-cNum>64) ? 64UL : AS-cNum;

  size_t i;
  for(i=0;i<N;i++)
    Item.Cell[i]=A[i+cNum];
  if (i<64)
    {
      Item.Cell[i]=0x80;        // Always possible
      for(i++;i<64;i++)
	Item.Cell[i]=0;
      // Final unit:
      if (N<56)
	{
	  Item.LW[7]=A.size()*8;
	  return 1;
	}
      else 
	return -1;       // insufficient filling space
    }
  return 0;
}

void
MD5hash::getOverFill(const std::string& A)
 /*!
   Get the overfill string
   \param A :: String we are processing
  */
{
  ELog::RegMethod RegA("MD5hash","getOverfill");
  for(size_t i=0;i<64;i++)
    Item.Cell[i]=0;
  // Final unit:
  Item.LW[7]=A.size()*8;
  return;
}

uint4
MD5hash::leftRotate(const uint4& A,const uint4& B) 
  /*!
    Left rotate operation
    \param A :: Main number
    \param B :: Rotation complex
    \return split conjunction
  */
{
  return (A<<B) | (A >> (32-B));
}

void
MD5hash::mainLoop()
  /*!
    Process the main loop
  */
{
  uint4 a=h0;
  uint4 b=h1;
  uint4 c=h2;
  uint4 d=h3;

  uint4 f,g,temp;
  for(uint4 i=0;i<64;i++)
    {
      if (i<16)
	{
	  f=(b & c) | ((~b) & d);
	  g=i;
	}
      else if (i<32)
	{
	  f=(d & b) | ((~d) & c);
	  g=(5*i+1) % 16;
	}	  
      else if (i<48)
	{
	  f=b ^ c ^ d;
	  g=(3*i+5) % 16;
	}	  
      else 
	{
	  f=c ^ (b | (~d));
	  g=(7*i) % 16;
	}	  
      temp=d;
      d=c;
      c=b;
      b+=leftRotate((a+f+k[i]+Item.W[g]),r[i]);
      a=temp;
    }
  h0+=a;
  h1+=b;
  h2+=c;
  h3+=d;
  return;
}


std::string
MD5hash::processMessage(const std::string& A)
  /*!
    String to process, not the string does not 
    get consumed but a varible cNum is updated each time
    \param A :: String to process
    \return HASH String
   */
{
  ELog::RegMethod RegA("MD5hash","processMessage");
  init();
  
  int flag(0);
  while (!flag)
    {
      flag=getNextUnit(A);
      cNum+=64;
      mainLoop();
    }
  if (flag<0)    // Overfill case:
    {
      getOverFill(A);
      mainLoop();
    }
  // Now process last component:
  
  Item.W[0]=h0;
  Item.W[1]=h1;
  Item.W[2]=h2;
  Item.W[3]=h3;

  std::ostringstream cx;
  for(size_t i=0;i<16;i++)
    cx<<std::setfill('0')<<std::setw(2)<<
      std::hex<<static_cast<unsigned int>(Item.Cell[i]);

  return cx.str();
}
  

