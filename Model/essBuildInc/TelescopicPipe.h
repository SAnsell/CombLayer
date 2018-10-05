/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/TelescopicPipe.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef essSystem_TelescopicPipe_h
#define essSystem_TelescopicPipe_h

class Simulation;

namespace essSystem
{

/*!
  \class TelescopicPipe
  \author S. Ansell
  \version 1.0
  \date February 2013
  \brief Multi-component pipe
*/

class TelescopicPipe : public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap
{
 private:

  size_t nSec;                    ///< Number of tube sections
  std::vector<double> radius;     ///< Radius of inner void
  std::vector<double> length;     ///< cummolative length 
  std::vector<double> zCut;       ///< Z cut for section
  std::vector<double> thick;      ///< Wall thickness  
  std::vector<int> inMat;         ///< Inner material
  std::vector<int> wallMat;       ///< wall material

  // Functions:

  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 protected:
  void populate(const FuncDataBase&);

 public:

  TelescopicPipe(const std::string&);
  TelescopicPipe(const TelescopicPipe&);
  TelescopicPipe& operator=(const TelescopicPipe&);
  virtual TelescopicPipe *clone() const;
  virtual ~TelescopicPipe();
   
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
 
};

}

#endif
 
