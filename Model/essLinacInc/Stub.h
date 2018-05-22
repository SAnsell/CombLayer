/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/Stub.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_Stub_h
#define essSystem_Stub_h

class Simulation;

namespace essSystem
{

/*!
  \class Stub
  \version 1.0
  \author Konstantin Batkov
  \date 21 May 2018
  \brief Stub
*/

class Stub : public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::FrontBackCut
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  std::vector<double> length;        ///< Length of each length
  double width;                 ///< Width
  double height;                ///< height

  double wallThick;             ///< Thickness of wall

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  Stub(const std::string&,const size_t);
  Stub(const Stub&);
  Stub& operator=(const Stub&);
  virtual Stub* clone() const;
  virtual ~Stub();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
 

