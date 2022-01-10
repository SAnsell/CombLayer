/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/ElectroBoxGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_ElectroBoxGenerator_h
#define setVariable_ElectroBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ElectroBoxGenerator
  \version 1.0
  \author S. Ansell
  \date December 2021
  \brief Standard (exept length/chicane) electrometer box
*/

class ElectroBoxGenerator
{
 private:

  
  double elecWidth;             ///< full width
  double elecLength;            ///< full length
  double elecHeight;            ///< full height
  double voidSpace;             ///< drop of electromenter in box

  double frontThick;        ///< front lead thickness
  double backThick;         ///< side lead thickness
  double sideThick;         ///< side lead thickness
  
  double skinThick;         ///< skin thickness x 2

  std::string voidMat;             ///< void material
  std::string elecMat;             ///< electrometer material
  std::string skinMat;             ///< skin material material
  std::string wallMat;             ///< wall material

 public:

  ElectroBoxGenerator();
  ElectroBoxGenerator(const ElectroBoxGenerator&) =default;
  ElectroBoxGenerator& operator=(const ElectroBoxGenerator&) =default;
  ~ElectroBoxGenerator() =default;

  void generateBox(FuncDataBase&,const std::string&,
		   const double,const double) const;

};

}

#endif
 
