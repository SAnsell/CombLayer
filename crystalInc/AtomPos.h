/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   crystalInc/AtomPos.h
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
#ifndef Crystal_AtomPos_h
#define Crystal_AtomPos_h

namespace Crystal
{

 
  /*!
    \class AtomPos
    \version 1.0
    \author S. Ansell
    \date March 2007
    \brief Holds an atom in a unit cell
  */

class AtomPos
{
 private:

  std::string Name;           ///< Name/Symbol
  int Z;                      ///< Z number
  int Ion;                    ///< Ion type
  Geometry::Vec3D Cent;       ///< Centre vector
  double occ;                 ///< Occupance fraction
  
 public:

  AtomPos();
  AtomPos(const std::string&,const int,const int,
	  const Geometry::Vec3D&,const double);
  AtomPos(const AtomPos&);
  AtomPos& operator=(const AtomPos&);
  bool operator<(const AtomPos&) const; 
  ~AtomPos();

  int equal(const AtomPos&,const double =1e-3) const;

  void setAtom(const int,const int=0);
  void setOcc(const double); 
  void setPos(const Geometry::Vec3D&);

  double getEdgeFactor() const;

  /// Get Name
  const std::string& getName() const { return Name; }  
  /// Z number accessor
  int getZ() const { return Z; }
  /// Get Position
  const Geometry::Vec3D& getPos() const { return Cent; } 
  /// get Occupancy number
  double getOcc() const { return occ; }

  void write(std::ostream&) const;

};

 /*!
   \class nearCentre 
   \version 1.0
   \brief Gets the closes atom to the centre 
   \author S. Ansell
   \date March 2007
 */
class nearCentre : 
 public std::binary_function<AtomPos,AtomPos,bool> 
{
 public:
  /// Comparitor for positions to centre
  bool operator()(const AtomPos& A,const AtomPos& B) const
    {
      return (A.getPos().abs()<B.getPos().abs());
    }
};


std::ostream&
operator<<(std::ostream&,const AtomPos&);

}  // NAMESPACE Crystal

#endif

