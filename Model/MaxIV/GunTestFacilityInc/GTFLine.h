/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/GTFLine.h
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef xraySystem_GTFLine_h
#define xraySystem_GTFLine_h

namespace constructSystem
{
  class Bellows;
}

namespace MAXIV
{

  /*!
    \class GTFLine
    \version 1.0
    \author K. Batkov
    \date September 2023
    \brief Constructor for the Gun Test Facility line components
  */

class GTFLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// construction space for main object
  attachSystem::BlockZone buildZone;
  int innerMat;                         ///< inner material if used

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// a dummy bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  GTFLine(const std::string&);
  GTFLine(const GTFLine&);
  GTFLine& operator=(const GTFLine&);
  ~GTFLine();

  /// Assignment to inner void
  void setInnerMat(const int M) {  innerMat=M; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
