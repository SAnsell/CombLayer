/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystalInc/CifStore.h
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
#ifndef Crystal_CifStore_h
#define Crystal_CifStore_h


namespace Crystal
{

class CifLoop;
class AtomPos;
class SymUnit;

  /*!
    \class CifStore
    \author S. Ansell#include "CryMat.h"
    \version 1.0
    \date March 2008
    \brief Holds a Cif file for processing
   */
class CifStore
{
 private:


  typedef std::map<std::string,int> ZTYPE;  ///< Type of Zmap 
  typedef std::map<std::string,int> NTYPE;  ///< Type of Nmap 
  typedef std::map<std::string,int> UTYPE;  ///< Type for read of comp
  
  Geometry::Vec3D cellAxis;           ///< Cell Axis distances
  Geometry::Vec3D cellAngle;          ///< Cell angles
  Geometry::Vec3D unitVec[3];         ///< Unit vectors
  Geometry::Vec3D recipVec[3];        ///< Reciprical vectors
  double V0;                          ///< Volume [V0]
  
  int tableNumber;                    ///< Table number 
  int exafsAtom;                      ///< Central atom
  int exafsType;                      ///< Central atom (type)

  ZTYPE Zmap;                         ///< Map of Z + Ion [from symbol]
  NTYPE Nmap;                         ///< Map of SiteName [Elm+Index]

  std::vector<int> Z;                 ///< Z-number
  std::vector<int> Ions;              ///< Ions 
  std::vector<double> Bcoh;           ///< Bcoh for a given atom

  std::vector<SymUnit> Sym;           ///< Symmetry
  std::vector<AtomPos> Atoms;         ///< Atoms
  std::vector<AtomPos> Cell;          ///< Unit cell [cells]
  std::vector<AtomPos> FullCell;      ///< Total atoms in complete full zone [many unit cells]

  int readAtoms(const CifLoop&);
  int readSym(const CifLoop&);
  int readTypes(const CifLoop&);


  // PRIVATE METHODS:
  void createUnit();
  Geometry::Vec3D makeCartisian(const Geometry::Vec3D&) const;
  int inCube(const Geometry::Vec3D&,const double) const;
  void insertElement(const std::string&,double =-1000);
  void insertAtom(const std::string&,const std::string&,
		  const Geometry::Vec3D&,const double);

 public:
  
  CifStore();
  CifStore(const CifStore&);
  CifStore& operator=(const CifStore&);
  ~CifStore();

  int readFile(const std::string&);
  const std::vector<AtomPos>& calcUnitCell();
  double originDistance(const Geometry::Vec3D&) const;
  double volume() const;
  double aveOcc() const;
  int setCentralAtom(const std::string&,const int =0);
  void makeAtoms(const double);
  int applyOcc();
  void primaryRotation(const Triple<int>&,const Geometry::Vec3D&,
		       const Triple<int>&,const Geometry::Vec3D&);

  void secondaryRotation(const Geometry::Vec3D&,const double);

  void printSym() const; 
  void writeFeff(const std::string&); 

  /// Calcuate the Primary vector
  Geometry::Vec3D 
    UVec(const int A1,const int A2,const int A3) const
    {
      return (unitVec[0]*A1+unitVec[1]*A2+unitVec[2]*A3);
    }
  /// Calcuate the B vector
  Geometry::Vec3D 
    BVec(const int A1,const int A2,const int A3) const
    {
      return (recipVec[0]*A1+recipVec[1]*A2+recipVec[2]*A3);
    }
  
  /// Accessor to atoms iterator
  const std::vector<AtomPos>::const_iterator begin() const 
    { return FullCell.begin();  }
  /// Accessor to atoms iterator (end point)
  const std::vector<AtomPos>::const_iterator end() const 
    { return FullCell.end(); }

  double calcLatticeFactor(const int,const int,const int) const;
  Geometry::Vec3D calcLatticeVector(const int,const int,const int) const;
};

}  // NAMESPACE Crystal

#endif
