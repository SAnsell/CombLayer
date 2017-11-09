/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/surfIndex.h
*
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef surfIndex_h
#define surfIndex_h

namespace Geometry
{
  class Surface;
}

namespace ModelSupport
{

/*!
  \class surfIndex 
  \version 1.0
  \author S. Ansell
  \date December 2009
  \brief Storage for all the surfaces in the problem
*/

class surfIndex
{
 public:

  typedef std::map<int,Geometry::Surface*> STYPE;  ///< Storage type
  
 private:
 
  int uniqNum;                      ///< uniq number
  STYPE SMap;                       ///< Index of kept surfaces
  std::map<int,int> holdMap;        ///< Hold/Write map :: surfaceN : write/no-write flag
  
  surfIndex();

  ////\cond SINGLETON
  surfIndex(const surfIndex&);
  surfIndex& operator=(const surfIndex&);
  ////\endcond SINGLETON

  int processSurfaces(const std::string&);

  
 public:
  
  static surfIndex& Instance();
  
  ~surfIndex();

  Geometry::Surface* addSurface(Geometry::Surface*);
  void insertSurface(Geometry::Surface*);
  Geometry::Surface* removeEqualSurface(const int);
  Geometry::Surface* cloneSurface(const Geometry::Surface*);
  
  template<typename T> T* createSurf(const int);
  template<typename T> T* createNewSurf(int&);
  template<typename T> T* createUniqSurf(const int);
  template<typename T> T* addTypeSurface(T*);

  int getUniq();

  void reset();
  void createSurface(const int,const std::string&);  
  void createSurface(const int,const int,const std::string&);
  int checkSurface(const int,const Geometry::Vec3D&) const; 
  void deleteSurface(const int);
  void renumber(const int,const int);

  Geometry::Surface* getSurf(const int) const; 
  
  int calcRenumber(const int,std::vector<std::pair<int,int> >&) const;
  int calcRenumber(const std::vector<int>&,const std::vector<int>&,
		   int,std::vector<std::pair<int,int> >&) const;
  int calcRenumber(const int,const int,int,
		   std::vector<std::pair<int,int> >&) const;

  /// access to map
  const STYPE& surMap() const { return SMap; }
  void setKeep(const int,const int);

  bool mapValid() const;
  int keepFlag(const int) const;
  std::vector<int> keepVector() const;

  int findEqualSurf(const int,const int,
		    std::map<int,Geometry::Surface*>&) const;
  void removeOpposite(const int);
  int findOpposite(const Geometry::Surface*) const;

  int readOutputSurfaces(const std::string&);
};

}

#endif
