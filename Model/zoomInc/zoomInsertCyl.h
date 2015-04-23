/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/zoomInsertCyl.h
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
#ifndef shutterSystem_zoomInsertCyl_h
#define shutterSystem_zoomInsertCyl_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class zoomInsertCyl
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class zoomInsertCyl  : public zoomInsertBase
{
 private:

  double radGap;          ///< rad gap

  void setVar(const int,const double);
  void setVar(const FuncDataBase&,
	      const int,const std::string&);

  virtual void populate(const Simulation&,const zoomInsertBase*);

  virtual void createSurfaces(const int);
  virtual void createObjects(Simulation&,const std::string&,
			     const std::string&);

 public:

  zoomInsertCyl(const int,const int,const std::string&);
  zoomInsertCyl(const zoomInsertCyl&);
  zoomInsertCyl& operator=(const zoomInsertCyl&);
  virtual ~zoomInsertCyl() {}   ///< Destructor
  virtual zoomInsertCyl* clone() const;

  virtual std::string typeName() const { return "zoomInsertCyl"; }

  virtual double getVar(const int) const;
  virtual int exitWindow(const double,
			 std::vector<int>&,Geometry::Vec3D&) const;

};

}  

#endif
