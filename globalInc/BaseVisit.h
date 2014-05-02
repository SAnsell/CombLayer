/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   globalInc/BaseVisit.h
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
#ifndef BaseVisit_h
#define BaseVisit_h


namespace Geometry
{
  class Surface;
  class Quadratic;
  class ArbPoly;
  class CylCan;
  class Cylinder;
  class Cone;
  class EllipticCyl;
  class General;
  class MBrect;
  class NullSurface;
  class Plane;
  class Sphere;
  class Torus;
  
  class Line;
}

namespace MonteCarlo
{

  class Material;
  class Object;
  class Simulation;  

}

namespace ModelSupport
{
  class surfIndex;
  class matIndex;
}

namespace scatterSystem
{
  class neutMaterial;
}

namespace Transport
{
  class AreaBeam;  
  class Beam;
  class Detector;
  class VolumeBeam;  
}

class FuncDataBase;


namespace Global
{ 
/*!
  \class BaseVisit
  \version 1.0
  \author S. Ansell<
  \brief Visitation for all
  \date November 2007
  
  This is a base visit class to everything. The 
  idea is that we can specialise it to do differnt things.
  
 */

class BaseVisit
{

public:

  BaseVisit() {}            ///< Default constructor
  BaseVisit(const BaseVisit&) {} ///< Copy constructor
  virtual ~BaseVisit() {}   ///< Destructor

  /// \cond TABLE

  virtual void Accept(const Geometry::Surface&) {} 
  virtual void Accept(const Geometry::Quadratic&) {} 
  virtual void Accept(const Geometry::ArbPoly&) {} 
  virtual void Accept(const Geometry::Cone&) {}    
  virtual void Accept(const Geometry::CylCan&) {} 
  virtual void Accept(const Geometry::Cylinder&) {} 
  virtual void Accept(const Geometry::EllipticCyl&) {} 
  virtual void Accept(const Geometry::General&) {}  
  virtual void Accept(const Geometry::Line&) {}    
  virtual void Accept(const Geometry::MBrect&) {}  
  virtual void Accept(const Geometry::NullSurface&) {}  
  virtual void Accept(const Geometry::Plane&) {}    
  virtual void Accept(const Geometry::Sphere&) {}   
  virtual void Accept(const Geometry::Torus&) {}  

  virtual void Accept(const MonteCarlo::Material&) {}  
  virtual void Accept(const MonteCarlo::Object&) {}   
  virtual void Accept(const MonteCarlo::Simulation&) {} 

  virtual void Accept(const ModelSupport::matIndex&) {}  
  virtual void Accept(const ModelSupport::surfIndex&) {}  

  virtual void Accept(const scatterSystem::neutMaterial&) {}  

  virtual void Accept(const Transport::AreaBeam&) {} 
  virtual void Accept(const Transport::Beam&) {} 
  virtual void Accept(const Transport::Detector&) {} 
  virtual void Accept(const Transport::VolumeBeam&) {} 

  virtual void Accept(const FuncDataBase&) {}  
  /// \endcond TABLE

};


} // NAMESPACE 

#endif
