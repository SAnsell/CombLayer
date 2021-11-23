/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/ExptHutGenerator.h
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
#ifndef setVariable_ExptHutGenerator_h
#define setVariable_ExptHutGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ExptHutGenerator
  \version 1.0
  \author S. Ansell
  \date July 2021
  \brief Standard (exept length/chicane) optics hutch for Max IV
*/

class ExptHutGenerator  :
    public OpticsHutGenerator
{
 private:

  double ringWidth;     ///< Ring side width (interior)
  double pbFrontThick;     ///< if front lead is needed (+ve)

  double cornerAngle;   ///< Angle of corner
  double cornerYStep;   ///< Setp back for corner point

  double fHoleXStep;    ///< Front hole x-step
  double fHoleZStep;    ///< Front hole z-step
  double fHoleRadius;   ///< Front hole radius

 public:

  ExptHutGenerator();
  ExptHutGenerator(const ExptHutGenerator&) =default;
  ExptHutGenerator& operator=(const ExptHutGenerator&) =default;
  ~ExptHutGenerator() =default;

  void setCorner(const double,const double);

  void setFBLead(const double,const double);
  void setRoofLead(const double);
  void setWallLead(const double);
  
  void setFrontHole(const double,const double,const double);
  void generateHut(FuncDataBase&,const std::string&,
		   const double,const double) const;

};

}

#endif
 
