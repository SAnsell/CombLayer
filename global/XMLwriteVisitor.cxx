/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   global/XMLwriteVisitor.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/multi_array.hpp>
#include <boost/regex.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLread.h"
#include "XMLgrid.h"
#include "XMLdatablock.h"
#include "XMLload.h"
#include "XMLcollect.h"
#include "XMLgridSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "doubleErr.h"
#include "support.h"
#include "regexSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "Line.h"
#include "BaseVisit.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Quaternion.h"
#include "ArbPoly.h"
#include "Cone.h"
#include "Cylinder.h"
#include "General.h"
#include "MBrect.h"
#include "Plane.h"
#include "Sphere.h"
#include "Torus.h"
#include "Line.h"
#include "surfIndex.h"
#include "Rules.h"
#include "Track.h"
#include "HeadRule.h"
#include "Object.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "Code.h"
#include "funcList.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "varNameOrder.h"
#include "XMLwriteVisitor.h"

namespace XML
{

XMLwriteVisitor::XMLwriteVisitor() :
  BaseVisit(),managed(1),XOut(*(new XMLcollect()))
 /*!
   Constructor with construction of XOut
 */
{}

XMLwriteVisitor::XMLwriteVisitor(XMLcollect& A) :
  BaseVisit(),managed(0),XOut(A)
 /*!
   Constructor 
   \param A :: XMLcollect to change
 */
{}

XMLwriteVisitor::XMLwriteVisitor(const XMLwriteVisitor& A) :
  BaseVisit(A),managed(A.managed),
  XOut((!A.managed) ? A.XOut : *(new XMLcollect(A.XOut)))
 /*!
   Copy Constructor 
   \param A :: XMLwriteVisitor to copy
 */
{}

XMLwriteVisitor::~XMLwriteVisitor()
  /*!
    Deletion operator. Desides if XOut
    needs to be deleted.
  */
{
  if (managed)
    delete &XOut;
}

void
XMLwriteVisitor::writeStream(std::ostream& OX)  const
  /*!
    Write out the XOut object to a stream
    \param OX :: output stream
   */
{
  XOut.writeXML(OX);
  return;
}

void
XMLwriteVisitor::writeFile(const std::string& Fname)  const
  /*!
    Write out the XOut object to a file
    \param Fname :: Filename
   */
{
  if (!Fname.empty())
    {
      std::ofstream OX;
      OX.open(Fname.c_str());
      XOut.writeXML(OX);
      OX.close();
    }
  return;
}

// --------------------------------------------------------------------------
//                          GEOMETRY
// --------------------------------------------------------------------------

void
XMLwriteVisitor::Accept(const Geometry::Surface& Surf)
  /*!
    Base class writeout
    \param Surf :: Surface to write
   */
{
  XOut.addComp("Name",Surf.getName());
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::Quadratic& Surf)
  /*!
    Base class writeout
    \param Surf :: Surface to write
   */
{
  this->Accept(static_cast<const Geometry::Surface&>(Surf));
  return;
}
  
void
XMLwriteVisitor::Accept(const Geometry::ArbPoly& Surf)
  /*!
    Write out the ArbPoly
    \param Surf :: ArbPoly object to write
  */
{
  XOut.addGrp("Surface");
  XOut.getCurrent()->addAttribute("type","ArbPoly");
  this->Accept(static_cast<const Geometry::Surface&>(Surf));
  XOut.addComp("NSurface",Surf.getNSurf());

  XMLgrid<std::vector,Geometry::Vec3D>* Gptr=
    new XMLgrid<std::vector,Geometry::Vec3D>(XOut.getCurrent(),"CVec");
  Gptr->setComp(0,std::vector<Geometry::Vec3D>());
  Gptr->setEmpty(0);     // needs to be forced
  Gptr->setContLine(1);  // One vector a line
  for(size_t i=0;i<Surf.getNSurf();i++)
    Gptr->getGVec(0).push_back(Surf.getCVec(i));
  XOut.getCurrent()->addManagedObj(Gptr);

  XMLgrid<std::vector,size_t>* Hptr=
    new XMLgrid<std::vector,size_t>(XOut.getCurrent(),"CIndex");
  for(size_t i=0;i<Surf.getNSurf();i++)
    Hptr->setComp(i,Surf.getCIndex(i));
  XOut.getCurrent()->addManagedObj(Hptr);

  XOut.closeGrp();
  return;
}

void 
XMLwriteVisitor::Accept(const Geometry::Cone& Surf)
  /*!
    Write out the cone
    \param Surf :: Cone object to write
  */
{
  XOut.addGrp("Surface");
  XOut.getCurrent()->addAttribute("type","Cone");
  this->Accept(static_cast<const Geometry::Quadratic&>(Surf));
  XOut.addComp("Centre",Surf.getCentre());
  XOut.addComp("Normal",Surf.getNormal());
  XOut.addComp("Alpha",Surf.getAlpha());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::CylCan&)
  /*!
    Write out the Cylinder
    \param :: Cylinder object to write
  */
{
  XOut.addGrp("Surface");
//  this->Accept(static_cast<const Geometry::Quadratic&>(Surf));
  XOut.getCurrent()->addAttribute("type","CylCan");
//  XOut.addComp("Centre",Surf.getCentre());
//  XOut.addComp("Normal",Surf.getNormal());
//  XOut.addComp("Radius",Surf.getRadius());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::Cylinder& Surf)
  /*!
    Write out the Cylinder
    \param Surf :: Cylinder object to write
  */
{
  XOut.addGrp("Surface");
  this->Accept(static_cast<const Geometry::Quadratic&>(Surf));
  XOut.getCurrent()->addAttribute("type","Cylinder");
  XOut.addComp("Centre",Surf.getCentre());
  XOut.addComp("Normal",Surf.getNormal());
  XOut.addComp("Radius",Surf.getRadius());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::General& Surf)
  /*!
    Write out the Cylinder
    \param Surf :: Cylinder object to write
   */
{
  XOut.addGrp("Surface");
  this->Accept(static_cast<const Geometry::Quadratic&>(Surf));
  XOut.getCurrent()->addAttribute("type","General");
  XOut.addComp("BaseEqn",Surf.copyBaseEqn());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::MBrect& Surf)
  /*!
    Write out the MBrect
    \param Surf :: MBrect object to write
   */
{
  XOut.addGrp("Surface");
  this->Accept(static_cast<const Geometry::Surface&>(Surf));
  XOut.getCurrent()->addAttribute("type","MBrect");
  XOut.addComp("Corner",Surf.getCorner());
  XMLgrid<std::vector,Geometry::Vec3D>* Gptr=
    new XMLgrid<std::vector,Geometry::Vec3D>(XOut.getCurrent(),"LVec");

  Gptr->setComp(0,std::vector<Geometry::Vec3D>());
  Gptr->setEmpty(0);  // needs to be forces since vector can't know
  Gptr->setContLine(1);  // One vector a line
  for(size_t i=0;i<3;i++)
    Gptr->getGVec(0).push_back(Surf.getLVec(i));
  XOut.getCurrent()->addManagedObj(Gptr);
  XOut.closeGrp();      
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::NullSurface&)
  /*!
    Write out the NullSurface
    \param :: Surace to write
   */
{
  XOut.addGrp("Surface");
  XOut.getCurrent()->addAttribute("type","NullSurface");
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::Plane& Surf)
  /*!
    Write out the Plane
    \param Surf :: Plane object to write
   */
{
  XOut.addGrp("Surface");
  this->Accept(static_cast<const Geometry::Quadratic&>(Surf));
  XOut.getCurrent()->addAttribute("type","Plane");
  XOut.addComp("Normal",Surf.getNormal());
  XOut.addComp("Dist",Surf.getDistance());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::Sphere& Surf)
  /*!
    Write out the Sphere
    \param Surf :: Sphere object to write
  */
{
  XOut.addGrp("Surface");
  this->Accept(static_cast<const Geometry::Quadratic&>(Surf));
  XOut.getCurrent()->addAttribute("type","Sphere");
  XOut.addComp("Centre",Surf.getCentre());
  XOut.addComp("Radius",Surf.getRadius());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::Torus& Surf)
  /*!
    Write out the Torus
    \param Surf :: Sphere object to write
   */
{
  XOut.addGrp("Surface");
  this->Accept(static_cast<const Geometry::Surface&>(Surf));
  XOut.getCurrent()->addAttribute("type","Torus");
  XOut.addComp("Centre",Surf.getCentre());
  XOut.addComp("Normal",Surf.getNormal());
  XOut.addComp("InnerRad",Surf.getIRad());
  XOut.addComp("OutRad",Surf.getORad());

  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const Geometry::Line& LX)
  /*!
    Write out a Line
    \param LX :: Line to write
  */
{
  XOut.addGrp("Line");
  XOut.addComp("Origin",LX.getOrigin());
  XOut.addComp("Direct",LX.getDirect());
  XOut.closeGrp();
  return;
}

// -----------------------------------------------------------
//                Monte
// -----------------------------------------------------------

void
XMLwriteVisitor::Accept(const MonteCarlo::Object& Obj)
  /*!
    Write out an object
    \param Obj :: Object to write
  */
{
  XOut.addGrp("Object");
  XOut.addComp("ObjName",Obj.getName());
  XOut.addComp("cellList",Obj.str());
  XOut.closeGrp();
  return;
}


void
XMLwriteVisitor::Accept(const MonteCarlo::Material& MI)
  /*!
    Write out a material section
    \param MI :: Material block to write
  */
{
  XOut.addGrp("Material");
  XOut.addComp("Name",MI.getName());
  XOut.addComp("Density",MI.getName());
  std::ostringstream cx;
  MI.write(cx);
  XOut.addComp("Material_String",cx.str());
  XOut.closeGrp();
  return;
}

void
XMLwriteVisitor::Accept(const ModelSupport::surfIndex& SI)
  /*!
    Write out a material section
    \param SI :: Material block to write
  */
{
  const ModelSupport::surfIndex::STYPE& sMap=SI.surMap();
  ModelSupport::surfIndex::STYPE::const_iterator mc;
  XOut.addGrp("surfIndex");
  for(mc=sMap.begin();mc!=sMap.end();mc++)
    {
      (mc->second)->acceptVisitor(*this);
    }
  XOut.closeGrp();
  return;
}

///

void
XMLwriteVisitor::Accept(const FuncDataBase& FD)
  /*!
    Write out a function block
    \param FD :: FuncDataBase to write						
  */
{
  ELog::RegMethod RegA("XMLwriteVisitor","Accept(FuncDatBase)");

  const varList& VL=FD.getVarList();
  varList::varStore::const_iterator mc;

  // First item is constant that is not allowed in copy:
  // Hence value_type cannot be used
  typedef varList::varStore::mapped_type MTYPE;
  typedef std::pair<std::string,MTYPE> PTYPE;
  std::vector<PTYPE> Names;
  for(mc=VL.begin();mc!=VL.end();mc++)
    {
      Names.push_back(PTYPE(mc->first,mc->second));
    }
  sort(Names.begin(),Names.end(),varNameOrder());

  std::string Value;
  XOut.addGrp("Variables");
  std::vector<PTYPE>::const_iterator vc;
  for(vc=Names.begin();vc!=Names.end();vc++)
    {
      vc->second->getValue(Value);
      XOut.addComp("variable",Value);
      XOut.addObjAttribute("name",vc->first);
      XOut.addObjAttribute("type",vc->second->typeKey());
    }
  XOut.closeGrp();
  return;
}

}  // NAMESPACE XML

