/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/BlockShutterGenerator.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
  double shutterInnerLen; 
  double colletHGap;       ///< Collet gap horrizontal
  double colletVGap;       ///< Collet gap vertial
  double colletFGap;       ///< Collet gap forward

  double steelOffset;      ///< Steel back step distance

  double b4cThick;         ///< Total b4c thick
  double b4cSpace;         ///< Total b4c spacer

  
  std::string colletMat;         ///< Collet material
  

 public:

  BlockShutterGenerator();
  BlockShutterGenerator(const BlockShutterGenerator&);
  BlockShutterGenerator& operator=(const BlockShutterGenerator&);
  ~BlockShutterGenerator();

  void generateBox(FuncDataBase&,const std::string&,
		   const double,const double,
		   const double,const double,
		   const double,const double,
		   const double,const double) const;
  void generateCyl(FuncDataBase&,const std::string&,const double) const;

};

}

#endif
