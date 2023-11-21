/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/OffsetFlangePipe.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_OffsetFlangePipe_h
#define constructSystem_OffsetFlangePipe_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class OffsetFlangePipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief OffsetFlangePipe unit  
*/

class OffsetFlangePipe :
    public constructSystem::GeneralPipe
{
 private:
  
  double flangeAXStep;           ///< Joining Flange XStep
  double flangeAZStep;           ///< Joining Flange ZStep
  double flangeAXYAngle;         ///< Joining Flange angle xy
  double flangeAZAngle;          ///< Joining Flange angle z
  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeALength;          ///< Joining Flange length

  double flangeBXStep;           ///< Joining Flange XStep
  double flangeBZStep;           ///< Joining Flange ZStep
  double flangeBXYAngle;         ///< Joining Flange angle xy
  double flangeBZAngle;          ///< Joining Flange angle z
  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeBLength;          ///< Joining Flange length
    
  Geometry::Vec3D flangeAYAxis;        ///< front axis for flange
  Geometry::Vec3D flangeBYAxis;        ///< Back axis for flange
  
  void populate(const FuncDataBase&) override;
  void createSurfaces() override;
  void createObjects(Simulation&);
  void createLinks();

 public:

  OffsetFlangePipe(const std::string&);
  OffsetFlangePipe(const OffsetFlangePipe&);
  OffsetFlangePipe& operator=(const OffsetFlangePipe&);
  ~OffsetFlangePipe() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
 
