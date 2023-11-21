/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/RectanglePipe.h
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
#ifndef constructSystem_RectanglePipe_h
#define constructSystem_RectanglePipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class RectanglePipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief RectanglePipe unit
*/

class RectanglePipe :
  public GeneralPipe
{
 private:

  double width;         ///< full width 
  double height;        ///< full height
  
  virtual void populate(const FuncDataBase&) override;
  void createSurfaces() override;
  void createObjects(Simulation&);
  void createLinks();

 public:

  RectanglePipe(const std::string&);
  RectanglePipe(const RectanglePipe&);
  RectanglePipe& operator=(const RectanglePipe&);
  ~RectanglePipe() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
