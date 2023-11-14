/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/SolenoidGenerator.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef setVariable_SolenoidGenerator_h
#define setVariable_SolenoidGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SolenoidGenerator
  \version 1.0
  \author Konstantin Batkov
  \date November 2023
  \brief SolenoidGenerator for variables
*/

class SolenoidGenerator
{
 private:

  double length;                ///< Total length including void
  double frameWidth;            ///< Frame width
  double spacerThick;           ///< Frame spacer thickness
  double frameThick;            ///< Frame thickness (outer walls)
  double coilRadius;            ///< Wire coil radius

  std::string frameMat;         ///< Frame material
  std::string coilMat;          ///< Coil material
  int nCoils;                   ///< Number of coils
  int nFrameFacets;             ///< Number of frame facets

 public:

  SolenoidGenerator();
  SolenoidGenerator(const SolenoidGenerator&);
  SolenoidGenerator& operator=(const SolenoidGenerator&);
  virtual ~SolenoidGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
