/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/collInsertBlock.h
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
#ifndef shutterSystem_collInsertBlock_h
#define shutterSystem_collInsertBlock_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class collInsertBlock
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class collInsertBlock  : public collInsertBase
{
 private:

  double hGap;          ///< horrizontal gap
  double vGap;          ///< vertial gap

  void setVar(const size_t,const double);
  void setVar(const FuncDataBase&,
	      const size_t,const std::string&);

  virtual void populate(const Simulation&,const collInsertBase*);

  virtual void createSurfaces(const int);
  virtual void createObjects(Simulation&,const std::string&,
			     const std::string&);

 public:

  collInsertBlock(const int,const int,const std::string&);
  collInsertBlock(const collInsertBlock&);
  collInsertBlock& operator=(const collInsertBlock&);
  virtual ~collInsertBlock() {}   ///< Destructor
  virtual collInsertBlock* clone() const;

  virtual std::string typeName() const { return "collInsertBlock"; }

  virtual double getVar(const size_t) const;
  virtual int exitWindow(const double,
			 std::vector<int>&,Geometry::Vec3D&) const;
  virtual std::vector<Geometry::Vec3D> 
    viewWindow(const collInsertBase*) const;

  virtual void write(std::ostream&) const;
};

}  

#endif
