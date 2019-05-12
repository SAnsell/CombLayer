/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/DipoleGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_DipoleGenerator_h
#define setVariable_DipoleGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DipoleGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief DipoleGenerator for variables
*/

class DipoleGenerator 
{
 private:

  double length;                 ///< frame length

  double poleAngle;              ///< Arc of cylinder angle
  double poleRadius;             ///< radius of cylinder angle
  double poleGap;                ///< Gap from centre point
  double poleWidth;              ///< width [in rotated frame]
  double poleHeight;             ///< width [in rotated frame]

  double coilLength;             ///< Length of coil
  double coilWidth;              ///< Width of coil
  double coilHeight;             ///< Height of coil
  
  std::string poleMat;           ///< pole piece of magnet
  std::string coilMat;           ///< coil material

 public:

  DipoleGenerator();
  DipoleGenerator(const DipoleGenerator&);
  DipoleGenerator& operator=(const DipoleGenerator&);
  virtual ~DipoleGenerator();
  
  
  virtual void generateDipole(FuncDataBase&,const std::string&,
			      const double,const double) const;

};

}

#endif
 
