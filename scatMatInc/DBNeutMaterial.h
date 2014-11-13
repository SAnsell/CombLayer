/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   scatMatInc/DBNeutMaterial.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef scatterSystem_NeutMaterial_h
#define scatterSystem_NeutMaterial_h

namespace scatterSystem
{

/*!
  \class DBNeutMaterial
  \version 1.0
  \author S. Ansell
  \date August 2011
  \brief Storage for true scattering materials
*/

class DBNeutMaterial
{  
 private:

  /// Storage type for Materials
  typedef std::map<int,scatterSystem::neutMaterial*> MTYPE;
  /// Store of materials
  MTYPE  MStore;
  /// Active list
  std::set<int> active;

  DBNeutMaterial();

  ////\cond SINGLETON
  DBNeutMaterial(const DBNeutMaterial&);
  DBNeutMaterial& operator=(const DBNeutMaterial&);
  ////\endcond SINGLETON

  void initMaterial();
  
 public:
  
  static DBNeutMaterial& Instance();
  
  ~DBNeutMaterial() {}  ///< Destructor
  
  /// get data store
  const MTYPE& getStore() const { return MStore; }
  void resetActive();
  void setActive(const int);
  bool isActive(const int) const;
  void setENDF7();

  const scatterSystem::neutMaterial* getMat(const int) const;

  void write(std::ostream& OX) const;
};

}

#endif
