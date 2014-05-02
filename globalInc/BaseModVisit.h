/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   globalInc/BaseModVisit.h
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
#ifndef BaseModVisit_h
#define BaseModVisit_h

namespace Global
{ 
/*!
  \class BaseModVisit
  \version 1.0
  \author S. Ansell
  \brief Visitation for all
  \date November 2007
  
  This is a base visit class to everything. The 
  idea is that we can specialise it to do differnt things.
 */

class BaseModVisit
{

public:

  BaseModVisit() {}            ///< Default constructor
  BaseModVisit(const BaseModVisit&) {} ///< Copy constructor
  virtual ~BaseModVisit() {}   ///< Destructor

  /// \cond TABLE

  virtual void Accept(Geometry::Surface&) {} 
  virtual void Accept(Geometry::Quadratic&) {} 
  virtual void Accept(Geometry::ArbPoly&) {} 
  virtual void Accept(Geometry::Cone&) {}    
  virtual void Accept(Geometry::CylCan&) {} 
  virtual void Accept(Geometry::Cylinder&) {} 
  virtual void Accept(Geometry::EllipticCyl&) {} 
  virtual void Accept(Geometry::General&) {}
  virtual void Accept(Geometry::Line&) {}    
  virtual void Accept(Geometry::MBrect&) {}  
  virtual void Accept(Geometry::NullSurface&) {}     
  virtual void Accept(Geometry::Plane&) {}    
  virtual void Accept(Geometry::Sphere&) {}   
  virtual void Accept(Geometry::Torus&) {}  

  virtual void Accept(MonteCarlo::Material&) {}  
  virtual void Accept(MonteCarlo::Object&) {}   
  virtual void Accept(MonteCarlo::Simulation&) {} 

  virtual void Accept(ModelSupport::matIndex&) {}  
  virtual void Accept(ModelSupport::surfIndex&) {}  

  virtual void Accept(scatterSystem::neutMaterial&) {}  

  virtual void Accept(Transport::AreaBeam&) {} 
  virtual void Accept(Transport::Beam&) {} 
  virtual void Accept(Transport::Detector&) {}  
  virtual void Accept(Transport::VolumeBeam&) {}  

  virtual void Accept(FuncDataBase&) {}  
  /// \endcond TABLE

};


} // NAMESPACE 

#endif
