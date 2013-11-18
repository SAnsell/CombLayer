/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/ShutterCreate.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "SurInter.h"
#include "Simulation.h"
#include "shutterBlock.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "GeneralShutter.h"
#include "ChipIRShutter.h"
#include "ZoomShutter.h"
#include "chipDataStore.h"
#include "ShutterCreate.h"


namespace shutterSystem
{

std::string
cylinderIntercept(const int CyNum,
		  const int FP,const int BP,
		  const std::vector<int>& Sides)
 /*!
    Edge planes are Top:Bottom / Side:Side
    \param CyNum :: Axial Cylinder
    \param FP :: Front plane
    \param BP :: Back plane
    \param Sides :: The box edges (four)
    \return string for cell inclusion
 */
{
  ELog::RegMethod RegA("ShutterCreate","cyclinderIntercept");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  ELog::EM<<"HERE"<<ELog::endCrit;
  std::vector<const Geometry::Surface*> Edges;
  for(int i=0;i<4;i++)
    {
      Edges.push_back(SurI.getSurf(abs(Sides[i])));
      if (!Edges.back())
	throw ColErr::InContainerError<int>(Sides[i],RegA.getBase());
    }
  const Geometry::Cylinder* Axiel=
    dynamic_cast<const Geometry::Cylinder*>(SurI.getSurf(CyNum));
  const Geometry::Surface* FrontP=SurI.getSurf(FP);
  const Geometry::Surface* BackP=SurI.getSurf(BP);
  if (!Axiel || !FrontP || !BackP)
    {
      ELog::EM<<"C/F/B == "<<CyNum<<" "<<FP<<" "<<BP<<ELog::endWarn;
      ELog::EM<<"Cylinder Axiel "<<*Axiel<<ELog::endWarn;
      ELog::EM<<"FrontPT: "<<*FrontP<<ELog::endWarn;
      ELog::EM<<"BackPT: "<<*BackP<<ELog::endWarn;
    }
  std::vector<int> outIndex; // results

  // First calculate lines in centre:
  const Geometry::Vec3D& Cpt=Axiel->getCentre();
  const Geometry::Vec3D& CAx=Axiel->getNormal();
  const double R=Axiel->getRadius();
  MonteCarlo::LineIntersectVisit LI(Cpt,CAx);
  try
    {
      LI.clearTrack();
      const Geometry::Vec3D PtA=LI.getPoint(FrontP);
      LI.clearTrack();
      const Geometry::Vec3D PtB=LI.getPoint(BackP);
      // Loop over surface : 
      // Accept if (a) within a radius distance (b) opposite signs
      for(int i=0;i<4;i++)
        {
	  const double DA=Edges[i]->distance(PtA);	  
	  const double DB=Edges[i]->distance(PtB);
	  if (fabs(DA)<R || fabs(DB)<R || DB*DA<=0.0)
	    outIndex.push_back(i);
	}
    }
  catch(ColErr::ExBase&)
    {
      ELog::EM<<"ShutterCreate No intercept"<<ELog::endErr;
    }

  std::vector<int>::const_iterator vc;
  std::ostringstream cx;
  for(vc=outIndex.begin();vc!=outIndex.end();vc++)
    cx<<Sides[*vc]<<" ";

  return cx.str();
}


}  // NAMESPACE shutterSystem
