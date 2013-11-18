/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/DBMaterial.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ModelSupport_DBMaterial_h
#define ModelSupport_DBMaterial_h

namespace scatterSystem
{
  class neutMaterial;
}

namespace ModelSupport
{

/*!
  \class DBMaterial 
  \version 1.0
  \author S. Ansell
  \date December 2009
  \brief Storage fo all the surfaces in the problem
*/

class DBMaterial
{  
 private:

  /// Storage type for Materials
  typedef std::map<int,MonteCarlo::Material> MTYPE;
  /// Storage type for Neut Materials
  typedef std::map<int,scatterSystem::neutMaterial*> NTYPE;

  MTYPE  MStore;    ///< Store of materials
  NTYPE  NStore;     ///< Store of neutron materials [if exist]
  /// Active list
  std::set<int> active;

  DBMaterial();

  ///\cond SINGLETON
  DBMaterial(const DBMaterial&);
  DBMaterial& operator=(const DBMaterial&);
  ///\endcond SINGLETON

  void initMaterial();
  void initMXUnits();
  
 public:
  
  static DBMaterial& Instance();
  
  ~DBMaterial() {}  ///< Destructor
  
  /// get data store
  const MTYPE& getStore() const { return MStore; }
  const NTYPE& getNeutMat() const { return NStore; }

  void setMaterial(const MonteCarlo::Material&);
  void setNeutMaterial(const int,const scatterSystem::neutMaterial&);
  void setNeutMaterial(const int,const scatterSystem::neutMaterial*);
  void resetActive();
  void setActive(const int);
  bool isActive(const int) const;
  void setENDF7();

  void writeCinder(std::ostream& OX) const;
  void writeMCNPX(std::ostream& OX) const;
};

}

#endif
