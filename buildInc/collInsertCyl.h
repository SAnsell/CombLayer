/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/collInsertCyl.h
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
#ifndef shutterSystem_collInsertCyl_h
#define shutterSystem_collInsertCyl_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class collInsertCyl
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class collInsertCyl  : public collInsertBase
{
 private:

  double radGap;          ///< rad gap

  void setVar(const size_t,const double);
  void setVar(const FuncDataBase&,
	      const size_t,const std::string&);

  virtual void populate(const Simulation&,const collInsertBase*);

  virtual void createSurfaces(const int);
  virtual void createObjects(Simulation&,const std::string&,
			     const std::string&);

 public:

  collInsertCyl(const int,const int,const std::string&);
  collInsertCyl(const collInsertCyl&);
  collInsertCyl& operator=(const collInsertCyl&);
  virtual ~collInsertCyl() {}   ///< Destructor
  virtual collInsertCyl* clone() const;

  /// Accessor to type
  virtual std::string typeName() const { return "collInsertCyl"; }

  virtual double getVar(const size_t) const;
  virtual int exitWindow(const double,
			 std::vector<int>&,Geometry::Vec3D&) const;
  virtual std::vector<Geometry::Vec3D> 
    viewWindow(const collInsertBase*) const;

};

}  

#endif
