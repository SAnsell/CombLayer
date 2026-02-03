/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/BladeBPMGenerator.h
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#ifndef setVariable_BladeBPMGenerator_h
#define setVariable_BladeBPMGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BladeBPMGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2026
  \brief BladeBPMGenerator for variables
*/

class BladeBPMGenerator
{
 private:

  double length;                ///< Total length including XBPM insert and vacuum chamber
  double chamberLength;         ///< Vacuum chamber length including flanges
  double chamberRadius;         ///< Vacuum chamber radius (inner)
  double chamberWallThick;      ///< Vacuum chamber wall thickness
  double chamberFlangeRadius;   ///< Vacuum chamber flange radius
  double chamberFlangeLength;   ///< Vacuum chamber flange length

  std::string chamberFlangeMat; ///< Vacuum chamber flange material
  std::string chamberWallMat;   ///< Vacuum chamber wall material
  std::string voidMat;          ///< Void material

 public:

  BladeBPMGenerator();
  BladeBPMGenerator(const BladeBPMGenerator&);
  BladeBPMGenerator& operator=(const BladeBPMGenerator&);
  virtual ~BladeBPMGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
