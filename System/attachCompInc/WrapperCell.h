/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   attachCompInp/WrapperCell.h
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
#ifndef attachSystem_WrapperCell_h
#define attachSystem_WrapperCell_h

class Simulation;

namespace attachSystem
{

/*!
  \class WrapperCell
  \version 1.0
  \author Stuart Ansell
  \date February 2021
  \brief Create a cell that wrapps a number of components
  
  This object wraps a number of FixedComp object that are expected 
  to be in a close proximity.

  The process is that:
    - objects are registered with addUnit
    - surfaces on the exterior are added with setSurfaces
    - position of objects for create all are registerd.
    
    Surfaces are the objects (which are wrapped) and the external surface
    to make the wrapper cell. The syntace is name of surface 
    [front/back etc]. This is a placeholder and not used, [FixedComp,side].

    FixeComp objects are created IF the connection unit is set.
    to make the wrapper cell. Connections are mapped as: 
    [NameOfObject : {FixedCompName : sideName} ]
    Note that FixedCompName can apply to any object within the simulation
    and a special name "this" can be used for the current location of the 
    Wrapper cell. The wrapper cell is built relative to an object but 
    in is a convinience method to get the link points correct.
*/

class WrapperCell :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 public:

  /// general surf/line connections
  typedef std::map<std::string,std::pair<std::string,std::string>> mapTYPE;

 private:
  
  const std::string baseName;

  
  /// Unit for the shielding
  std::vector<std::shared_ptr<attachSystem::FixedComp>> Units;
  /// Connections
  std::map<std::string,std::pair<std::string,std::string>>
    connections;
  std::map<std::string,std::pair<std::string,std::string>>
    surfaces;
  
  void createObjects(Simulation&);
  void createLinks();

 public:

  WrapperCell(const std::string&,const std::string&);
  WrapperCell(const WrapperCell&);
  WrapperCell& operator=(const WrapperCell&);
  virtual ~WrapperCell();

  void addUnit(std::shared_ptr<attachSystem::FixedComp>);
  
  void addConnection(const std::string&,const std::string&,const std::string&);
  void addSurface(const std::string&,const std::string&,const std::string&);
 
  void setConnections(mapTYPE&&);
  void setSurfaces(mapTYPE&&);

  
  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
