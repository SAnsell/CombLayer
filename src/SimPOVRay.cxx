/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimPOVRay.cxx
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "Element.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"

#include "Simulation.h"
#include "SimPOVRay.h"


SimPOVRay::SimPOVRay() : Simulation()
  /*!
    Constructor
  */
{}


SimPOVRay::SimPOVRay(const SimPOVRay& A) : Simulation(A)
 /*! 
   Copy constructor
   \param A :: Simulation to copy
 */
{}

SimPOVRay&
SimPOVRay::operator=(const SimPOVRay& A)
  /*!
    Assignment operator
    \param A :: SimPOVRay to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
    }
  return *this;
}



void
SimPOVRay::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard POVRay output 
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPOVRay","writeCells");
  
  
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writePOVRay(OX);
  return;
}

void
SimPOVRay::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();

  for(const ModelSupport::surfIndex::STYPE::value_type& sm : SurMap)
    sm.second->writePOVRay(OX);
  OX<<std::endl;
  return;
} 

void
SimPOVRay::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPOVRay","writeMaterial");

  // WRITE OUT ASSIGNMENT:
  for(const OTYPE::value_type& mp : OList)
    mp.second->writePOVRaymat(OX);
    
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    DB.setActive(mp->second->getMat());

  DB.writePOVRay(OX);
  
  // Overwrite textures by a user-provided file textures.inc
  OX << "#if (file_exists(\"textures.inc\"))" << std::endl;
  OX << "#include \"textures.inc\"" << std::endl;
  OX << "#end"  << std::endl;
  
  return;
}
  
void
SimPOVRay::write(const std::string& Fname) const
  /*!
    Write out all the system for povray
    \param Fname :: Output file 
  */
{
  ELog::RegMethod RegA("SimPOVRay","write");

  std::ofstream OX(Fname.c_str()); 
  OX << "// POV-Ray model from CombLayer."<<std::endl;
  OX << "// This file contains only geomety." << std::endl;
  OX << "// It is supposed to be included from a .pov file with defined camera, light source and material textures" << std::endl;
  OX << std::endl;
  OX << "// Material" << std::endl;
  writeMaterial(OX);
  OX << std::endl;
  OX << "// Surfaces" << std::endl;
  // The 'Reorient' macro is taken from John VanSickle's Thorougly Useful Macros
  // http://www.geocities.ws/evilsnack/macs.html
  // It is used by the InfiniteCylinder macro from this post
  // http://news.povray.org/povray.newusers/message/<3A0F50A5.1C648BDF%40erols.com>
  OX << "#macro Reorient(Axis1,Axis2)" << std::endl
     << " #local vX1=vnormalize(Axis1);" << std::endl
     << " #local vX2=vnormalize(Axis2);" << std::endl
     << " #local vY=vnormalize(vcross(vX1,vX2));" << std::endl
     << "#if (vlength(vY)>0)" << std::endl
     << " #local vZ1=vnormalize(vcross(vX1,vY));" << std::endl
     << " #local vZ2=vnormalize(vcross(vX2,vY));" << std::endl
     <<"  matrix < vX1.x, vY.x,vZ1.x, vX1.y,vY.y,vZ1.y, vX1.z,vY.z, vZ1.z, 0,0,0 >" << std::endl
     <<"  matrix < vX2.x,vX2.y,vX2.z, vY.x,vY.y, vY.z, vZ2.x,vZ2.y,vZ2.z, 0,0,0 >" << std::endl
     <<"#end" << std::endl
     <<"#end" << std::endl
     << std::endl;
  OX << "#macro InfiniteCylinder(PointA,PointB,Radius)" << std::endl
     << " quadric { <1,0,1>,<0,0,0>,<0,0,0>,-Radius*Radius" << std::endl
     << " Reorient(y,vnormalize(PointB-PointA))" << std::endl
     << " translate PointA" << std::endl
     << "}" << std::endl
     << "#end" << std::endl
     << std::endl;
  writeSurfaces(OX);
  OX << "// Cells" << std::endl;
  writeCells(OX);
  OX.close();
  return;
}
