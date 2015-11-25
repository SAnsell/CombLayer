/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   essBuild/DiskPreSimple.cxx
  *
  * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "CellMap.h"
#include "Plane.h"
#include "BilbaoWheelInnerStructure.h"

namespace essSystem
{

  BilbaoWheelInnerStructure::BilbaoWheelInnerStructure(const std::string& Key) :
    attachSystem::ContainedComp(),
    attachSystem::FixedComp(Key,6),
    insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
    cellIndex(insIndex+1)
    /*!
      Constructor
      \param Key :: Name of construction key
    */
  {}

  BilbaoWheelInnerStructure::BilbaoWheelInnerStructure(const BilbaoWheelInnerStructure& A) : 
    attachSystem::ContainedComp(A),
    attachSystem::FixedComp(A),
    insIndex(A.insIndex),
    cellIndex(A.cellIndex),
    brickLen(A.brickLen),
    brickWidth(A.brickWidth),
    brickMat(A.brickMat),
    brickGapLen(A.brickGapLen),
    brickGapWidth(A.brickGapWidth),
    brickGapMat(A.brickGapMat),
    nSectors(A.nSectors),
    secSepThick(A.secSepThick),
    secSepMat(A.secSepMat)
    /*!
      Copy constructor
      \param A :: BilbaoWheelInnerStructure to copy
    */
  {}

  BilbaoWheelInnerStructure&
  BilbaoWheelInnerStructure::operator=(const BilbaoWheelInnerStructure& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheelInnerStructure to copy
    \return *this
  */
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedComp::operator=(A);
	cellIndex=A.cellIndex;
	brickLen=A.brickLen;
	brickWidth=A.brickWidth;
	brickMat=A.brickMat;
	brickGapLen=A.brickGapLen;
	brickGapWidth=A.brickGapWidth;
	brickGapMat=A.brickGapMat;
	nSectors=A.nSectors;
	secSepThick=A.secSepThick;
	secSepMat=A.secSepMat;
      }
    return *this;
  }

  BilbaoWheelInnerStructure*
  BilbaoWheelInnerStructure::clone() const
  /*!
    Clone self 
    \return new (this)
  */
  {
    return new BilbaoWheelInnerStructure(*this);
  }

  BilbaoWheelInnerStructure::~BilbaoWheelInnerStructure()
  /*!
    Destructor
  */
  {}
  

  void
  BilbaoWheelInnerStructure::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","populate");

    brickLen=Control.EvalVar<double>(keyName+"BrickLength");
    brickWidth=Control.EvalVar<double>(keyName+"BrickWidth");
    brickMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickMat");

    brickGapLen=Control.EvalVar<double>(keyName+"BrickGapLength");
    brickGapWidth=Control.EvalVar<double>(keyName+"BrickGapWidth");
    brickGapMat=ModelSupport::EvalMat<int>(Control,keyName+"BrickGapMat");

    nSectors=Control.EvalVar<int>(keyName+"NSectors");
    if (nSectors<1)
      ELog::EM << "NSectors must be >= 1" << ELog::endErr;
    secSepThick=Control.EvalVar<double>(keyName+"SectorSepThick");
    secSepMat=ModelSupport::EvalMat<int>(Control,keyName+"SectorSepMat");  

    return;
  }

  void
  BilbaoWheelInnerStructure::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createUnitVector");
    attachSystem::FixedComp::createUnitVector(FC);

    return;
  }


  void
  BilbaoWheelInnerStructure::createSurfaces(const attachSystem::FixedComp& Wheel)
  /*!
    Create planes for the inner structure iside BilbaoWheel
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createSurfaces");

    //    const double BilbaoWheelZTop = Wheel.getLinkPt(7)[2];

  // segmentation
  double theta(0.0);
  double thetarad(0.0);
  const double dTheta = 360.0/nSectors;
  int SIsec(insIndex+5000);
  Geometry::Plane *p[nSectors*2];
  int i=0; // plane counter
  for (int j=0; j<nSectors; j++)
    {
      theta = j*dTheta;
      thetarad = theta*M_PI/180.0;
      ModelSupport::buildPlaneRotAxis(SMap, SIsec+1, Origin, X, Z, theta+90); // invisible divider
      p[i++] = ModelSupport::buildPlaneRotAxis(SMap, SIsec+3, Origin-X*(secSepThick/2.0*cos(thetarad))-Y*(secSepThick/2.0*sin(thetarad)), X, Z, theta);
      p[i++] = ModelSupport::buildPlaneRotAxis(SMap, SIsec+4, Origin+X*(secSepThick/2.0*cos(thetarad))+Y*(secSepThick/2.0*sin(thetarad)), X, Z, theta);

      SIsec += 10;
    }
  // bricks
  i=0;
  for (int j=0; j<nSectors; j++)
    {
      if (j==0)
	createBrickSurfaces(Wheel, p[i+2], p[i+1]);
      //	createBrickSurfaces(Wheel, p[i], p[nSectors-2]);
      i+=2;
    }


    return; 
  }

  void
  BilbaoWheelInnerStructure::createObjects(Simulation& System, attachSystem::FixedComp& Wheel)
  /*!
    Create the objects
    \param System :: Simulation to add results
    \param Wheel :: Wheel object where the inner structure is to be added
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createObjects");
    
    attachSystem::CellMap* CM = dynamic_cast<attachSystem::CellMap*>(&Wheel);
    if (!CM)
      throw ColErr::DynamicConv("FixedComp","CellMap",Wheel.getKeyName());

    const std::pair<int,double> MatInfo=CM->deleteCellWithData(System, "Inner");
    const int innerMat = MatInfo.first;
    const double innerTemp = MatInfo.second;

    const std::string vertStr = Wheel.getLinkString(6) + Wheel.getLinkString(7); // top+bottom
    const std::string cylStr = Wheel.getLinkString(8) + Wheel.getLinkString(9); // min+max radii

    //    System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,vertStr+cylStr));
    

    //    int SI(insIndex);
    int SIsec(insIndex+5000), SI1;
    std::string Out;
    if (nSectors==1)
      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,vertStr+cylStr)); // same as "Inner" cell from BilbaoWheel
    else 
      {
	for (int j=0; j<nSectors; j++)
	  {
	    // Tungsten
	    SI1 = (j!=nSectors-1) ? SIsec+10 : insIndex+5000;
	    Out = ModelSupport::getComposite(SMap, SIsec, SI1, " 4 -3M ");
	    if (j!=0)
		System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,innerTemp,
						 Out+vertStr+cylStr));
	    else
	      createBricks(System, Wheel, 
			   ModelSupport::getComposite(SMap, SIsec," 4 "), // side plane
			   ModelSupport::getComposite(SMap, SI1, " -3 ")); // another side plane
	    
	    // Pieces of steel between Tungsten sectors
	    // -1 is needed since planes 3 and -4 cross Tunsten in two places,
	    //     so we need to select only one
	    Out = ModelSupport::getComposite(SMap, SIsec, " 3 -4 -1 ");
	    System.addCell(MonteCarlo::Qhull(cellIndex++,secSepMat,innerTemp,Out+vertStr+cylStr));

	    SIsec+=10;
	  }
      }



    return; 
  }

  void
  BilbaoWheelInnerStructure::createBrickSurfaces(const attachSystem::FixedComp& Wheel,
						 const Geometry::Plane *pSide1,
						 const Geometry::Plane *pSide2)
  /*
    Creates surfaces for individual Tungsten bricks
    pSide1 :: wheel segment side plane
    pSide2 :: wheel segment side plane
   */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createBrickSurfaces");

    const Geometry::Surface *innerCyl = SMap.realSurfPtr(Wheel.getLinkSurf(8));
    const Geometry::Surface *outerCyl = SMap.realSurfPtr(Wheel.getLinkSurf(9));

    const Geometry::Plane *pz = ModelSupport::buildPlane(SMap,insIndex+5,Origin,Z);
    Geometry::Vec3D nearPt(0, -10, 0);
    Geometry::Vec3D p1 = SurInter::getPoint(pSide1, outerCyl, pz, nearPt);
    Geometry::Vec3D p2 = SurInter::getPoint(pSide2, outerCyl, pz, nearPt);
    Geometry::Vec3D p3 = p2 + Geometry::Vec3D(0.0, 0.0, 1.0);

    // radial planes
    int SI(insIndex+6000);
    // first (outermost) layer
    Geometry::Plane *prad1 = 0; // first radial plane of the bricks
    const int Nlayers=4;
    for (int i=0; i<Nlayers; i++)
      {
	if (i==0)
	  {
	    prad1 = ModelSupport::buildPlane(SMap, SI+5, p1, p2, p3,
					     Geometry::Vec3D(0.0, 0.0, 0.0));
	  }
	else
	  {
	    // plane which goes after brick and gap
	    ModelSupport::buildShiftedPlane(SMap, SI+5, prad1, -(brickLen+brickGapLen)*i);
	  }

	// back side of the brick
	Geometry::Plane *ptmp = ModelSupport::buildShiftedPlane(SMap, SI+6, prad1,
								-(brickLen*(i+1)+brickGapLen*i));

	SI += 10;
      }

    //  perpendicular to radial planes
    // only 2 layers are needed since other layers use the same planes
    Geometry::Plane *phor1 = 0; // first perpendicular to radial plane of the bricks
    int SJ(insIndex+6000);
    for (int i=0; i<50; i++)
      {
	// 1st layer
	if (i==0)
	  phor1 = ModelSupport::buildRotatedPlane(SMap, SJ+1, prad1, 90, Z, p2);
	else // after brick and gap
	  ModelSupport::buildShiftedPlane(SMap, SJ+1, phor1,
					  i*(brickWidth+brickGapWidth)); 
	
	// after brick
	ModelSupport::buildShiftedPlane(SMap, SJ+2, phor1,
					i*(brickWidth+brickGapWidth)+brickWidth);
	
	// 2nd layer
	// after brick
	ModelSupport::buildShiftedPlane(SMap, SJ+11, phor1,
					(2*i+1)*(brickWidth+brickGapWidth)/2.0);
	// after brick and gap
	ModelSupport::buildShiftedPlane(SMap, SJ+12, phor1,
					(2*i+1)*(brickWidth+brickGapWidth)/2.0+brickWidth);

	SJ += 20;
      }
  }

  void
  BilbaoWheelInnerStructure::createBricks(Simulation& System, attachSystem::FixedComp& Wheel,
					  const std::string side1, const std::string side2)
  /*
    Create cells for bricks in the given sector
   */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createBricks");

    const std::string sideStr = side1 + side2;
    const std::string vertStr = Wheel.getLinkString(6) + Wheel.getLinkString(7); // top+bottom
    const std::string innerCyl = Wheel.getLinkString(8);
    const std::string outerCyl = Wheel.getLinkString(9);

    std::string Out,Out1;
    int SI(insIndex+6000);
    // He layer in front of the bricks
    Out = ModelSupport::getComposite(SMap, SI, " 5 ");
    System.addCell(MonteCarlo::Qhull(cellIndex++, brickGapMat, 0, Out+vertStr+outerCyl));

    // bricks
    const int Nlayers = 4;
    std::string layerStr;
    for (int i=0; i<Nlayers; i++)
      {
	Out = ModelSupport::getComposite(SMap, SI, " -5  6 ");
	layerStr = Out;
	if (i>2) // otherwise we add bricks (tmp)
	  System.addCell(MonteCarlo::Qhull(cellIndex++, brickMat, 0, Out+vertStr+sideStr));
	else {
	  int SJ(insIndex+6000);
	  for (int j=0; j<17; j++)
	    {
	      int bOffset = i%2 ? SJ+10 : SJ;
	      Out1 = ModelSupport::getComposite(SMap, bOffset, " 1 -2 ");
	      if (j==0)
		Out1 = Out1 + side1;
	      System.addCell(MonteCarlo::Qhull(cellIndex++, brickMat, 0,
					       Out1+layerStr+vertStr));
	      
	      Out1 = ModelSupport::getComposite(SMap, bOffset, bOffset+20, " 2 -1M ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++, brickGapMat, 0,
					       Out1+layerStr+vertStr));
	      
	      SJ += 20;
	    }
	}

	if (i==Nlayers-1) 
	  Out = ModelSupport::getComposite(SMap, SI, SI+10, " -6  ") + innerCyl;
	else
	  Out = ModelSupport::getComposite(SMap, SI, SI+10, " -6 5M ");
	System.addCell(MonteCarlo::Qhull(cellIndex++, brickGapMat, 0, Out+vertStr+sideStr));
	
	SI += 10;
      }
  }

  void
  BilbaoWheelInnerStructure::createLinks()
  /*!
    Creates a full attachment set
  */
  {  
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createLinks");


    return;
  }

  void
  BilbaoWheelInnerStructure::createAll(Simulation& System,
				       attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point	       
  */
  {
    ELog::RegMethod RegA("BilbaoWheelInnerStructure","createAll");

    populate(System.getDataBase());
    createUnitVector(FC);

    createSurfaces(FC);
    createObjects(System, FC);
    createLinks();

    insertObjects(System);       
    return;
  }

}  // NAMESPACE essSystem
