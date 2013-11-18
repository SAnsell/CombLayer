/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/zoomInsertBlock.h
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
#ifndef shutterSystem_zoomInsertBlock_h
#define shutterSystem_zoomInsertBlock_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class zoomInsertBlock
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class zoomInsertBlock  : public zoomInsertBase
{
 private:

  double hGap;          ///< horrizontal gap
  double vGap;          ///< vertial gap

  void setVar(const int,const double);
  void setVar(const FuncDataBase&,
	      const int,const std::string&);

  virtual void populate(const Simulation&,const zoomInsertBase*);

  virtual void createSurfaces(const int);
  virtual void createObjects(Simulation&,const std::string&,
			     const std::string&);

 public:

  zoomInsertBlock(const int,const int,const std::string&);
  zoomInsertBlock(const zoomInsertBlock&);
  zoomInsertBlock& operator=(const zoomInsertBlock&);
  virtual ~zoomInsertBlock() {}   ///< Destructor
  virtual zoomInsertBlock* clone() const;

  virtual std::string typeName() const { return "zoomInsertBlock"; }

  virtual double getVar(const int) const;
  virtual int exitWindow(const double,
			 std::vector<int>&,Geometry::Vec3D&) const;

};

}  

#endif
