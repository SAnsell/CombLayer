/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/surfaceFactory.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "ArbPoly.h"
#include "CylCan.h"
#include "Cylinder.h"
#include "Cone.h"
#include "EllipticCyl.h"
#include "General.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Plane.h"
#include "Sphere.h"
#include "Torus.h"
#include "surfaceFactory.h"

namespace Geometry
{

Surface*
surfaceFactory::surfaceIndex(const Geometry::SurfKey& Index) const
  /*!
    Get a surface index
    \param Index :: runtime index 
    \return Surface pointer 
   */
{
  switch (Index)
    {
    case Geometry::SurfKey::ArbPoly:
      return new ArbPoly;
    case Geometry::SurfKey::Cone:
      return new Cone;
    case Geometry::SurfKey::CylCan:
      return new CylCan;
    case Geometry::SurfKey::Cylinder:
      return new Cylinder;
    case Geometry::SurfKey::EllipticCyl:
      return new EllipticCyl;
    case Geometry::SurfKey::General:
      return new General;
    case Geometry::SurfKey::MBrect:
      return new MBrect;
    case Geometry::SurfKey::NullSurface:
      return new NullSurface;
    case Geometry::SurfKey::Plane:
      return new Plane;
    case Geometry::SurfKey::Sphere:
      return new Sphere;
    case Geometry::SurfKey::Torus:
      return new Torus;
    default:
      throw ColErr::InContainerError<int>
	(static_cast<int>(Index),"Unable to generate surface type");

    }
  return nullptr;
}


surfaceFactory&
surfaceFactory::Instance() 
  /*!
    Effective new command / this command 
    \returns Single instance of surfaceFactory
  */
{
  static surfaceFactory SFact;
  return SFact;
}

surfaceFactory::surfaceFactory() : 
  wordGrid({
      {"ArbPoly",Geometry::SurfKey::ArbPoly},
      {"Cone",Geometry::SurfKey::Cone},
      {"CylCan",Geometry::SurfKey::CylCan},
      {"Cylinder",Geometry::SurfKey::Cylinder},
      {"EllipticCyl",Geometry::SurfKey::EllipticCyl},
      {"General",Geometry::SurfKey::General},
      {"MBrect",Geometry::SurfKey::MBrect},
      {"NullSurface",Geometry::SurfKey::NullSurface},
      {"Plane",Geometry::SurfKey::Plane},
      {"Sphere",Geometry::SurfKey::Sphere},
      {"Torus",Geometry::SurfKey::Torus}
    })
    /*!
    Constructor
  */
{
  registerSurface();
}

surfaceFactory::~surfaceFactory()
  /*!
    Destructor
  */
{}


void
surfaceFactory::registerSurface()
  /*!
    Sets up the initial maps
  */
{
  static const std::vector<std::string> shortNames =
    {
      "arb","",
      "k/x","k/y","k/z","kx","ky","kz","",
      "rcc","",
      "c/x","c/y","c/z","cx","cy","cz","",
      "e/x","e/y","e/z","ex","ey","ez","",
      "gq","sq","",
      "box","rpp","",
      "null","",
      "p","px","py","pz","",
      "so","s","sx","sy"",sz","sph","",
      "tx","ty","tz",""
    };
  static const Geometry::SurfKey surfValue[]=
    {
      Geometry::SurfKey::ArbPoly,
      Geometry::SurfKey::Cone,
      Geometry::SurfKey::CylCan,
      Geometry::SurfKey::Cylinder,
      Geometry::SurfKey::EllipticCyl,
      Geometry::SurfKey::General,
      Geometry::SurfKey::MBrect,
      Geometry::SurfKey::NullSurface,
      Geometry::SurfKey::Plane,
      Geometry::SurfKey::Sphere,
      Geometry::SurfKey::Torus
    };

  size_t svIndex(0);
  for(const std::string& SN : shortNames)
    {
      if (SN.empty())
	  svIndex++;
      else
	{
	  keyGrid.emplace(SN,surfValue[svIndex]);
	}
    }
  return;
}

Surface*
surfaceFactory::createSurfaceID(const std::string& Key) const
  /*!
    Creates an instance of surface
    given a valid key. 
    
    \param Key :: Form of first ID
    \throw InContainerError for the key if not found
    \return new surface object.
  */    
{
  ELog::RegMethod RegA("surfaceFactory","createSurfaceID");
  
  MapTYPE::const_iterator mc;
  mc=keyGrid.find(Key);
  if (mc==keyGrid.end())
    throw ColErr::InContainerError<std::string>
      (Key,"Surface key");
  return surfaceIndex(mc->second);
}

int
surfaceFactory::getIndex(const std::string& Key) const
  /*!
    Get the index , given a valid key. 
    
    \param Key :: Form of first ID
    \throw InContainerError for the key if not found
    \return new surface object.
  */    
{
  ELog::RegMethod RegA("surfaceFactory","getIndex");
  MapTYPE::const_iterator mc;
  mc=wordGrid.find(Key);
  if (mc==wordGrid.end())
    throw ColErr::InContainerError<std::string>
	(Key,"key in wordGrid");

  return static_cast<int>(mc->second);
}

Surface*
surfaceFactory::createSurface(const std::string& Key) const
  /*!
    Creates an instance of surface
    given a valid key.     
    \param Key :: Item to get 
    \throw InContainerError for the key if not found
    \return new surface object.
  */    
{
  ELog::RegMethod RegA("surfaceFactory","createSurface");

  MapTYPE::const_iterator vc;
  vc=wordGrid.find(Key);
  if (vc==wordGrid.end())
    throw ColErr::InContainerError<std::string>
      (Key,"Surface key");
    
  return surfaceIndex(vc->second);
}

Surface*
surfaceFactory::processLine(const std::string& Line) const
  /*!
    Creates an instance of a surface
    given a valid line. This can deal with a surface
    including a leading identifier
    
    \param Line :: Full description of line
    \throw InContainerError for the key if not found
    \return new surface object.
  */    
{
  ELog::RegMethod RegA("surfaceFactory","processLine");

  int id(0);
  std::string key;
  std::string procLine(Line);
  StrFunc::section(procLine,id);           // Id is only set if this succeeds

  if (!StrFunc::convert(procLine,key))
    throw ColErr::InvalidLine(Line,"surfaceFactory::processLine [no key]" );
  
  Surface* X = createSurfaceID(key);
  const int errNum=X->setSurface(procLine);
  if (!errNum)
    {
      if (id) X->setName(id);
      return X;
    }
  if (errNum>-100)                              // Full error
    {
      ELog::EM<<"Error with surface TYPE :: "<<X->className()<<ELog::endCrit;
      ELog::EM<<"Error with surface ["<<X->setSurface(procLine)<<
	"] "<<procLine<<ELog::endCrit;
      throw ColErr::InvalidLine(Line,"input procLine");

    }
  delete X;  
  X=new Geometry::NullSurface(id,0);
  return X;
}


} // NAMESPACE geometry
