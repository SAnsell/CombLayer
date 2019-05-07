/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsDiagnosticUnit.h
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#ifndef xraySystem_cosaxsDiagnosticUnit_h
#define xraySystem_cosaxsDiagnosticUnit_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsDiagnosticUnit
  \version 1.0
  \author Konstantin Batkov
  \date 2 May 2019
  \brief CoSAXS diagnostic unit
*/

class cosaxsDiagnosticUnit :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Inner length
  double width;                 ///< Inner width
  double height;                ///< Inner height

  int wallThick; ///< Wall thickness
  int wallMat;                  ///< Wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsDiagnosticUnit(const std::string&);
  cosaxsDiagnosticUnit(const cosaxsDiagnosticUnit&);
  cosaxsDiagnosticUnit& operator=(const cosaxsDiagnosticUnit&);
  virtual cosaxsDiagnosticUnit* clone() const;
  virtual ~cosaxsDiagnosticUnit();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


