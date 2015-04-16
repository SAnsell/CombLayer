/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/zoomInsertBase.h
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
#ifndef shutterSystem_zoomInsertBase_h
#define shutterSystem_zoomInsertBase_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class zoomInsertBase
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class zoomInsertBase  : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 protected:

  const int blockIndex;         ///< Index of block

  const int surfIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell

  Geometry::Vec3D beamOrigin;   ///< beam directions
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

  void createUnitVector(const zoomInsertBase&);
  void createUnitVector(const FixedComp&);
  
  void createLinks();
  
  virtual void populate(const Simulation&,const zoomInsertBase*) =0; 
  virtual void createSurfaces(const int) =0;
  virtual void createObjects(Simulation&,const std::string&,
			     const std::string&) =0;

  
 public:

  zoomInsertBase(const int,const int,const std::string&);
  zoomInsertBase(const zoomInsertBase&);
  zoomInsertBase& operator=(const zoomInsertBase&);
  virtual zoomInsertBase* clone() const =0;
  virtual ~zoomInsertBase() {}   ///< Destructor

  virtual std::string typeName() const =0;
  virtual double getVar(const int) const =0;

  /// Get materials
  int getMat() const { return matN; }
  bool equalExternal(const zoomInsertBase&) const;

  Geometry::Vec3D getWindowCentre() const;
  virtual int exitWindow(const double,std::vector<int>&,
			 Geometry::Vec3D&) const =0;

  void initialize(Simulation&,const attachSystem::FixedComp&);
  void initialize(Simulation&,const zoomInsertBase&);
  void setOrigin(const Geometry::Vec3D&,const double,
		 const double,const double,const double);
  void createAll(Simulation&,const int,
		 const std::string&,const std::string&);
  void createAll(Simulation&,const zoomInsertBase&);

};

}  

#endif
