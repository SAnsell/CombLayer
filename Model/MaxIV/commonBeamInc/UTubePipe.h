/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/UTubePipe.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_UTubePipe_h
#define xraySystem_UTubePipe_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class UTubePipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief UTubePipe unit  
*/

class UTubePipe :
    public constructSystem::GeneralPipe
{
 private:
  
  double width;                 ///< void width [inner]
  double height;                ///< void height [inner]
  double length;                ///< void length [total]
      
  void populate(const FuncDataBase&) override;
  void createSurfaces() override;
  void createObjects(Simulation&);
  void createLinks();

 public:

  UTubePipe(const std::string&);
  UTubePipe(const UTubePipe&);
  UTubePipe& operator=(const UTubePipe&);
  ~UTubePipe() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
