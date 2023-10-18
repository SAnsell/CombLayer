/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/RFGunGenerator.h
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
#ifndef setVariable_RFGunGenerator_h
#define setVariable_RFGunGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class RFGunGenerator
  \version 1.0
  \author Konstantin Batkov
  \date October 2023
  \brief RFGunGenerator for variables
*/

class RFGunGenerator
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double wallThick;             ///< Wall thickness

  std::string mainMat;          ///< Main material
  std::string wallMat;          ///< Wall material

 public:

  RFGunGenerator();
  RFGunGenerator(const RFGunGenerator&);
  RFGunGenerator& operator=(const RFGunGenerator&);
  virtual ~RFGunGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
