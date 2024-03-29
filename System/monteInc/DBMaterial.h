/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/DBMaterial.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

  /// Enumeration type for construction
  enum  class MatType { mcnp, photon, neutron };
  
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
  /// String to id-number
  typedef std::map<std::string,int> SCTYPE;
  /// Storage type for Materials (based on id number)
  typedef std::map<int,MonteCarlo::Material*> MTYPE;

  /// Storage type for Neutron Materials 
  typedef std::map<int,scatterSystem::neutMaterial*> NTYPE;

  static MatType matType;
  
  int nextID;        ///< Next id number
  
  SCTYPE IndexMap;   ///< Map of indexes
  MTYPE  MStore;     ///< Store of materials
  NTYPE  NStore;     ///< Store of neutron materials [if exist]

  DBMaterial();

  ///\cond SINGLETON
  DBMaterial(const DBMaterial&);
  DBMaterial& operator=(const DBMaterial&);
  ///\endcond SINGLETON

  void initMaterial();
  void initMXUnits();
  void checkNameIndex(const int,const std::string&) const;
  int getFreeNumber() const;

  int createOrthoParaMix(const std::string&,const double);
  int createMix(const std::string&,const std::string&,
		const std::string&,const double);
  int createNewDensity(const std::string&,const std::string&,
		       const double);

  int getNextID();
  
 public:

  static void setMatType(const MatType M) { matType=M; }
  static DBMaterial& Instance();
  
  ~DBMaterial();        ///< Destructor
  
  /// get data store
  const MTYPE& getStore() const { return MStore; }
  /// Get neutron material list
  const NTYPE& getNeutMat() const { return NStore; }

  // to be deprecated:
  const MonteCarlo::Material& getMaterial(const int) const;
  const MonteCarlo::Material& getMaterial(const std::string&) const;

  const MonteCarlo::Material* getMaterialPtr(const int) const;
  const MonteCarlo::Material* getMaterialPtr(const std::string&) const;

  const MonteCarlo::Material* getVoidPtr() const;

  void resetMaterial(const MonteCarlo::Material&);
  void setMaterial(const MonteCarlo::Material&);
  void setNeutMaterial(const int,const scatterSystem::neutMaterial&);
  void setNeutMaterial(const int,const scatterSystem::neutMaterial*);

  MonteCarlo::Material&
  createMaterial(const std::string&,const std::string&,
		 const std::string&,const std::string&);
  
  bool createMaterial(const std::string&);
  int processMaterial(const std::string&);

  bool hasKey(const std::string&) const;
  bool hasKey(const int) const;
  const std::string& getKey(const int) const;
  int getIndex(const std::string&) const;
  void removeThermal(const std::string&);
  void removeAllThermal();
  void cloneMaterial(const std::string&,const std::string&);
  void overwriteMaterial(const std::string&,const std::string&);

  void deactivateParticle(const std::string&);
  
  void setENDF7();

  void readFile(const std::string&);

  void createPhotonMaterial();
};

}

#endif
