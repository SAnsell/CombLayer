/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/portSet.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef constructSystem_portSet_h
#define constructSystem_portSet_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class portSet
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief portSet unit  

  This stores a group of ports that are attached to some FixedComp
  unit (FUnit). The FUnit that it is attached to is set in the constructor
  and is unimportant
*/

class portSet
{
 private:

  attachSystem::FixedComp& FUnit;        ///< Fixed referenece
  attachSystem::CellMap* cellPtr;        ///< Pointer if exists
  bool outerVoid;                        ///< Is outer void needed
  std::string outerVoidName;             ///< Is outer void needed
  
  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<std::shared_ptr<portItem>> Ports;     


  void populate(const FuncDataBase&);
  template<typename T>
  int procSplit(Simulation&,const std::string&,const int,const int,
		 const T&,const T&);
  
 public:

  portSet(attachSystem::FixedComp&);  // effective this
  portSet(const portSet&);
  portSet& operator=(const portSet&);
  virtual ~portSet();

  int splitVoidPorts(Simulation&,const std::string&,
		     const int,const int);
  int splitVoidPorts(Simulation&,const std::string&,const int,
		     const int,const Geometry::Vec3D&);
  int splitVoidPorts(Simulation&,const std::string&,const int,
		     const int,const std::vector<size_t>&);

  void setPortRotation(const size_t,const Geometry::Vec3D&);

  void addInsertPortCells(const int);
  void intersectPorts(Simulation&,const size_t,const size_t) const;
  void intersectVoidPorts(Simulation&,const size_t,const size_t) const;
  const portItem& getPort(const size_t) const;

  void insertAllInCell(MonteCarlo::Object&) const;
  void insertAllInCell(Simulation&,const int) const;
  void insertAllInCell(Simulation&,const std::vector<int>&) const; 
  void insertPortInCell(Simulation&,
			const std::vector<std::set<int>>&) const;

  /// set outer void [expect "outerVoid"  as name]
  void setOuterVoid(const std::string& ON)
    { outerVoid=1; outerVoidName=ON;}
  void createPorts(Simulation&,MonteCarlo::Object*,
		   const HeadRule&,const HeadRule&);
  void createPorts(Simulation&,const std::string&);
};

}

#endif
 
