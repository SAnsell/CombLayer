/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/CurrentMonitorGenerator.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef setVariable_CurrentMonitorGenerator_h
#define setVariable_CurrentMonitorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CurrentMonitorGenerator
  \version 1.0
  \author Konstantin Batkov
  \date January 2024
  \brief CurrentMonitorGenerator for variables
*/

class CurrentMonitorGenerator
{
 private:

  double length;                ///< Total length including void
  double frontRadius;           ///< Inner radius of the front segment
  double midRadius;             ///< Inner radius of the middle segment
  double backRadius;            ///< Inner radius of the back segment
  double outerRadius;           ///< Outer radius
  double wallThick;             ///< Wall thickness

  std::string mainMat;          ///< Main material
  std::string wallMat;          ///< Wall material

 public:

  CurrentMonitorGenerator();
  CurrentMonitorGenerator(const CurrentMonitorGenerator&);
  CurrentMonitorGenerator& operator=(const CurrentMonitorGenerator&);
  virtual ~CurrentMonitorGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
