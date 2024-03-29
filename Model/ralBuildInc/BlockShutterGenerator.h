/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   ralVarInc/BlockShutterGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef setVariable_BlockShutterGenerator_h
#define setVariable_BlockShutterGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BlockShutterGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BlockShutterGenerator for variables
*/

class BlockShutterGenerator
{
 private:

  size_t NSections;        ///< Number of sections required
  size_t steelNumber;      ///< Number of steel blocks per section

  double shutterFStep;     ///< Forward step of insert
  double shutterLen;       ///< Length of shutter insert
  double colletHGap;       ///< Collet gap horrizontal
  double colletVGap;       ///< Collet gap vertial
  double colletFGap;       ///< Collet gap forward

  double steelOffset;      ///< Steel back step distance
  double steelAWidth;          ///< Steel width front half units
  double steelBWidth;          ///< Steel width back half units

  double b4cThick;         ///< Total b4c thick
  double b4cSpace;         ///< Total b4c spacer
  
  std::string colletMat;   ///< Collet material
  std::string b4cMat;      ///< b4c material
  std::string steelMat;    ///< Inner Steel mat
  

 public:

  BlockShutterGenerator();
  BlockShutterGenerator(const BlockShutterGenerator&);
  BlockShutterGenerator& operator=(const BlockShutterGenerator&);
  ~BlockShutterGenerator() =default;

  void generateBox(FuncDataBase&,const std::string&,
		   const double,const double,
		   const double,const double,
		   const double,const double,
		   const double,const double) const;
  void generateCyl(FuncDataBase&,const std::string&,const double) const;

};

}

#endif
