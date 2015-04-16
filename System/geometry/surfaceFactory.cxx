/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/surfaceFactory.cxx
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
#include <boost/multi_array.hpp>
#include <boost/any.hpp>
#include <boost/type_traits.hpp>
#include <boost/multi_array.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
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
#include "surfVector.h"
#include "surfaceFactory.h"

namespace Geometry
{

/*!
  \struct surfCreate
  \brief Executes runtime on Index -- Surf type 
  \version 1.0
  \author S. Ansell
  \date September 2008
  
  Templates: 
   - State (list of types) 
   - Index :: Type id number to execute on (runtime)
*/
template<class State,class Index>
struct surfCreate
{
  static Surface* dispatch(const int I)
    /*!
      Creates the pointer to the surface
      Index refers to the positon on the ExportClass list
      \param I :: Index of runtime value (check with Index)
      \retval Ptr :: success
    */
    {
      if (I==Index())
        {
	  typedef typename boost::mpl::at<ExportClass,Index>::type ObjectType;
	  return new ObjectType;
	}
      else
        {
	  return State::dispatch(I);
	}
    }
};

/*!
  \struct unknownCreate
  \brief Executes failed runtime on TypePtr->write
  \version 1.0
  \author S. Ansell
  \date June 2008
*/
struct unknownCreate
{
  /// Return 0 (no type)
  static Surface* dispatch(const int)
    {
      return 0;
    }
};

Surface*
surfaceFactory::surfaceIndex(const int Index) const
  /*!
    Get a surface index
    \param Index :: runtime index 
    \return Surface pointer 
   */
{
  typedef boost::mpl::fold<boost::mpl::range_c<int,0,ExportSize>,
    unknownCreate,surfCreate<boost::mpl::_1 , boost::mpl::_2> >::type FTYPE;

  return FTYPE::dispatch(Index);
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

surfaceFactory::surfaceFactory() 
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
  for(int i=0;i<ExportSize;i++)
    wordGrid.insert(MapTYPE::value_type(ExportNames[i],i));
 
  int cnt(0);
  int index(0);
  while(cnt<ExportSize)
    {
      if (ShortNames[index])
	keyGrid.insert(MapTYPE::value_type(ShortNames[index],cnt));
      else
	cnt++;

      index++;
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
  MapTYPE::const_iterator mc;
  mc=keyGrid.find(Key);
  if (mc==keyGrid.end())
    {
      ELog::RegMethod RegA("surfaceFactory","createSurfaceID");
    
      throw ColErr::InContainerError<std::string>
	(Key,RegA.getFull());
    }
  
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
  MapTYPE::const_iterator mc;
  mc=wordGrid.find(Key);
  if (mc==wordGrid.end())
    {
      ELog::RegMethod RegA("surfaceFactory","getIndex");
      throw ColErr::InContainerError<std::string>
	(Key,"key in wordGrid");
    }
  return mc->second;
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
    {

      throw ColErr::InContainerError<std::string>
	(Key,ELog::RegMethod::getFull());
    }
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
  
  Surface *X = createSurfaceID(key);
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
      throw ColErr::InvalidLine(Line,"Line");

    }
  delete X;  
  X=new Geometry::NullSurface(id,0);
  return X;
}


} // NAMESPACE geometry
