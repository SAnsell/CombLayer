/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/LQuadH.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov / Stuart Ansell
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
#ifndef tdcSystem_LQuadH_h
#define tdcSystem_LQuadH_h

class Simulation;


namespace tdcSystem
{
/*!
  \class LQuadH
  \version 1.0
  \author K. Batkov
  \date May 2020

  \brief QH (old name: QG) quadrupole magnet for MAX IV
*/

class LQuadH :
    public tdcSystem::LQuadF
{
 private:
  double polePitch;             // pole pitch angle

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  LQuadH(const std::string&);
  LQuadH(const std::string&,const std::string&);
  LQuadH(const LQuadH&);
  LQuadH& operator=(const LQuadH&);
  virtual ~LQuadH();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
