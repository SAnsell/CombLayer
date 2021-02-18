/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/CryoTransferLine.h
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#ifndef essSystem_CryoTransferLine_h
#define essSystem_CryoTransferLine_h

class Simulation;

namespace essSystem
{

/*!
  \class CryoTransferLine
  \version 1.0
  \author Konstantin Batkov
  \date 21 May 2018
  \brief CryoTransferLine
*/

class CryoTransferLine :
    public attachSystem::ContainedGroup,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::FrontBackCut
{
 private:

  std::vector<double> length;   ///< Length of each length
  double width;                 ///< Width
  double height;                ///< height

  double widthHall;             ///< Width after entrance

  double wallThick;             ///< Thickness of wall
  double roofThick;             ///< Thickness of wall

  size_t nDucts; ///< Number of ducts

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CryoTransferLine(const std::string&);
  CryoTransferLine(const CryoTransferLine&);
  CryoTransferLine& operator=(const CryoTransferLine&);
  virtual CryoTransferLine* clone() const;
  virtual ~CryoTransferLine();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


