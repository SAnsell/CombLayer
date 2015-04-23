/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   md5Inc/MD5hash.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef MD5hash_h
#define MD5hash_h


/// This needs to be 4 bytes long only
typedef  unsigned int uint4;
typedef  unsigned long int uint8;

/*!
  \union Unit
  \author S. Ansell
  \version 1.0
  \brief Union support for character md5 buffer
  \date Noveber 2012
*/

union Unit
{
  unsigned char Cell[64];      ///< 512 bits (64*8)
  uint4 W[16];                 ///< 16x32 bit units
  uint8 LW[8];                 ///< 8x64 bit units
};

/*!
  \class MD5hash
  \author S. Ansell
  \version 1.0
  \brief Calculate an MD5 sum of a character stream/set
  \date Noveber 2012
*/

class MD5hash
{
 private:
  
  static uint4 r[64];           ///< Round shift values
  static uint4 k[64];           ///< Sin part
  uint4 h0;                     ///< First tracking number
  uint4 h1;                     ///< Second tracking number
  uint4 h2;                     ///< Third tracking number
  uint4 h3;                     ///< Fourth tracking number

  // Stuff for string
  Unit Item;                  ///< Item data
  size_t cNum;              ///< Position in string  

  void init();


  int getNextUnit(const std::string&);
  void getOverFill(const std::string&);
  void mainLoop();
  static uint4 leftRotate(const uint4&,const uint4&);

 public:
  
  MD5hash();
  MD5hash(const MD5hash&); 
  MD5hash& operator=(const MD5hash&); 
  ~MD5hash() {}
  
  /// access the unit
  const Unit& getUnit() const { return Item; }
  std::string processMessage(const std::string&);
 

};

#endif
