/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/collInsertBase.h
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
#ifndef shutterSystem_collInsertBase_h
#define shutterSystem_collInsertBase_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class collInsertBase
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class collInsertBase  : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 protected:

  const int blockIndex;         ///< Index of block

  const int surfIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell

  Geometry::Vec3D beamOrigin;   ///< beam mid point
  Geometry::Vec3D beamX;        ///< beam directions
  Geometry::Vec3D beamY;        ///< beam directions
  Geometry::Vec3D beamZ;        ///< beam directions
 
  int insertCell;               ///< Cell to insert into

  double fStep;         ///< Forward step
  double centX;         ///< Centre X offset
  double centZ;         ///< Centre Z offset
  double length;        ///< length of unit 
  double width;         ///< Full width
  double height;        ///< Full height

  int matN;             ///< Material number  

  void createUnitVector(const collInsertBase&);
  void createUnitVector(const FixedComp&);
  
  void createLinks();
  
  virtual void populate(const Simulation&,const collInsertBase*) =0; 
  virtual void createSurfaces(const int) =0;
  virtual void createObjects(Simulation&,const std::string&,
			     const std::string&) =0;

  
 public:

  collInsertBase(const int,const int,const std::string&);
  collInsertBase(const collInsertBase&);
  collInsertBase& operator=(const collInsertBase&);
  virtual collInsertBase* clone() const =0;
  virtual ~collInsertBase() {}   ///< Destructor

  virtual std::string typeName() const =0;
  virtual double getVar(const size_t) const =0;

  /// Get materials
  int getMat() const { return matN; }
  bool equalExternal(const collInsertBase&) const;

  Geometry::Vec3D getWindowCentre() const;
  virtual int exitWindow(const double,std::vector<int>&,
			 Geometry::Vec3D&) const =0;
  virtual std::vector<Geometry::Vec3D> 
    viewWindow(const collInsertBase*) const =0;
  void initialize(Simulation&,const attachSystem::FixedComp&);
  void initialize(Simulation&,const collInsertBase&);
  void setOrigin(const Geometry::Vec3D&,const double,
		 const double,const double,const double);
  void createAll(Simulation&,const int,
		 const std::string&,const std::string&);
  void createAll(Simulation&,const collInsertBase&);

  virtual void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const collInsertBase&);
}  

#endif
