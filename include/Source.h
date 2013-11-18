/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/Source.h
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
#ifndef Source_h
#define Source_h

namespace Geometry
{
  class Vec3D;
  template<typename T> class Matrix;
  class Transform;
}
class Simulation;

namespace SDef
{

  class SrcBase;
  class SrcData;
  class SrcBias;
  class SrcInfo;
  class SrcProb;
  class DSBase;
  template<typename T> class DSTerm; 
  
  template<typename T> class SrcItem;
/*!
  \class Source
  \version 1.0
  \date July 2009
  \author S.Ansell
  \brief Source Term
  
  Generic source term : Built independently
*/

class Source 
{
 private:

  typedef boost::shared_ptr<SrcBase> SBasePtr;            ///< Source items
  typedef boost::shared_ptr<SrcData> SDataPtr;            ///< Source data
  typedef std::map<std::string,SBasePtr> sdMapTYPE;       ///< Storage for main line
  typedef std::vector<SDataPtr> dvecTYPE;                 ///< Storage for extra-data

  int active;                              ///< Is souce active
  Geometry::Transform* transPTR;          ///< Trans number
  sdMapTYPE sdMap;                        ///< Map of main-line objects
  dvecTYPE DVec;                          ///< Data vectors

  void populate();

  template<typename T>
  SrcItem<T>* getItem(const std::string&);

  template<typename T>
  const SrcItem<T>* getItem(const std::string&) const;

  SrcBase* getBase(const std::string&);

  void cleanGroups();
  void setTransform(const Geometry::Vec3D [3]);

 public:
   
  Source();
  Source(const Source&);
  Source& operator=(const Source&);
  ~Source();

  void setActive() { active=1; }         ///< Make active 
  void deactivate() { active=0; }         ///< Make active 
  int isActive() const { return active; } ///< is active
  template<typename T>
  void setComp(const std::string&,const T&);
  void setData(const std::string&,const SrcData&);

  void clear();

  void cutEnergy(const double);
  void substituteCell(const int,const int);
  void substituteSurface(const int,const int);
  void addComp(const std::string&,const SrcBase*);
  /// Set the transform number if needed
  void setTransform(Geometry::Transform* TP) { transPTR=TP; }

  Geometry::Vec3D getPosCentre() const;
  Geometry::Vec3D getAxisLine() const;
  
  int rotateMaster();
  void write(std::ostream&) const;   ///< Write physics cards to ostream
};

}

#endif
