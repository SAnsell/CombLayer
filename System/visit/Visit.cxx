/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visit/Visit.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <array>
#include <fmt/core.h>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineUnit.h"
#include "LineTrack.h"
#include "Visit.h"

Visit::Visit() :
  outType(VISITenum::cellID),
  lineAverage(0),nPts({0,0,0})
  /*!
    Constructor
  */
{}

Visit::Visit(const Visit& A) : 
  outType(A.outType),lineAverage(A.lineAverage),
  Origin(A.Origin),XYZ(A.XYZ),nPts(A.nPts),
  mesh(A.mesh)
  /*!
    Copy constructor
    \param A :: Visit to copy
  */
{};

Visit&
Visit::operator=(const Visit& A)
  /*!
    Assignment operator
    \param A :: Visit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      outType=A.outType;
      lineAverage=A.lineAverage;
      Origin=A.Origin;
      XYZ=A.XYZ;
      nPts=A.nPts;
      mesh=A.mesh;
    }
  return *this;
}

Visit::~Visit()
  /*!
    Destructor
   */
{}

size_t
Visit::getMaxIndex() const
  /*!
    Get largest index [preference to Z]
    \return Max index [0-2]
  */						
{
  if (nPts[0]>nPts[1])
    return (nPts[0]>nPts[2]) ? 0 : 2;
  
  return (nPts[1]>nPts[2]) ? 1 : 2;
}

void 
Visit::setBox(const Geometry::Vec3D& startPt,
	      const Geometry::Vec3D& endPt)
  /*!
    Set the box start and end point
    \param startPt :: Point on near corner
    \param endPt :: Point on opposite corner
  */
{
  Origin=startPt;
  XYZ=endPt-Origin;
  return;
}

void
Visit::setIndex(const size_t A,const size_t B,const size_t C)
  /*!
    Set the index values , checks to ensure that 
    they are greater than zero
    \param A :: Xcoordinate division
    \param B :: Ycoordinate division
    \param C :: Zcoordinate division
  */
{
  nPts[0]=A;
  nPts[1]=B;
  nPts[2]=C;
  mesh.resize(A,B,C);
  return;
}

void
Visit::setType(const VISITenum& A)
  /*!
    Set a given visit output type
    \param A :: Output type						
  */
{
  outType=A;
  return;
}

double
Visit::getResult(const MonteCarlo::Object* ObjPtr) const
  /*!
    Determine what to calculate for an object 
    based on the mesh form
    \param ObjPtr :: object to calculate for
    \return determined value from the object
  */
{
  if (!ObjPtr) return 0.0;
  switch(outType)
    {
    case VISITenum::cellID:
      return ObjPtr->getName();
    case VISITenum::material:
      return ObjPtr->getMatID();
    case VISITenum::density:
      return ObjPtr->getDensity();
    case VISITenum::imp:
      return ObjPtr->getImp("neutron");
    case VISITenum::weight:
      return 0.0;
    }
  return 0.0;
}

void
Visit::populateLine(const Simulation& System,
		    const std::set<std::string>&)
  /*!
    The big population call with lines
    \param System :: Simulation system
    \param Active :: Active set of cells to use (ranged)
   */
{
  ELog::RegMethod RegA("Visit","populateLine");

  // First get max N
  const size_t IMax=getMaxIndex();
  const size_t IA = (IMax) ? 0: 1;
  const size_t IB = (IMax!=2) ? 2 : 1;
  
  const size_t nA = nPts[IA];
  const size_t nB = nPts[IB];
    
  Geometry::Vec3D aVec;  

  double stepXYZ[3];
  for(size_t i=0;i<3;i++)
    stepXYZ[i]=XYZ[i]/static_cast<double>(nPts[i]);
		  
  const Geometry::Vec3D XStep=(IMax==0) ?
    Geometry::Vec3D(0,1,0)*stepXYZ[IA] :
    Geometry::Vec3D(1,0,0)*stepXYZ[IA];

  const Geometry::Vec3D YStep=(IMax!=2) ?
    Geometry::Vec3D(0,0,1)*stepXYZ[IB] :
    Geometry::Vec3D(0,1,0)*stepXYZ[IB];

  const Geometry::Vec3D longStep=
    (IMax==1) ? (YStep*XStep).unit()*XYZ[IMax] :
    (XStep*YStep).unit()*XYZ[IMax];

  for(size_t i=0;i<nA;i++)
    for(size_t j=0;j<nB;j++)
      { 
	const Geometry::Vec3D aVec=Origin+
	  XStep*(static_cast<double>(i)+0.5)+
	  YStep*(static_cast<double>(j)+0.5);

	ModelSupport::LineTrack OTrack(aVec,aVec+longStep);
	OTrack.calculate(System);

	double T=0.0;
	size_t index(0);
	for(const ModelSupport::LineUnit& lu : OTrack.getTrackPts())
	  {
	    T+=lu.segmentLength;
	    // number of step through fem object
	    if (lu.objPtr)
	      {
		const size_t mid=Visit::procPoint(T,stepXYZ[IMax]);
		const int mValue=getResult(lu.objPtr);
		for(size_t cnt=0;cnt<mid;cnt++)
		  getMeshUnit(IMax,index++,i,j)=mValue;
	      }
	  }
      }
  
  return;
}

double&
Visit::getMeshUnit(const size_t posIndex,const size_t index,
		   const size_t i,const size_t j)
  /*!
    Get mesh unit based on posIndex being index place [0-2]
    and i,j begin the other two coordinates
    \param posIndex :: Index Max
    \param index :: primary index
    \param i :: other index in mesh
    \param j :: other index in mesh
    \return mesh[i][j] / [index]
  */
{ 
  if (posIndex==0)
    return mesh.get()[index][i][j];
  else if (posIndex==1)
    return mesh.get()[i][index][j];
  
  return mesh.get()[i][j][index];
}
  
long int
Visit::procDist(double& fullLen,const double lStep,
		double unitLen,double& aFrac)
  /*!
    Allows unit steps through a length returning
    the index 

    \param fullLen :: Length remaining
    \param unitLen :: Single unit length
    \param aFrac :: Frac of first unit

    \return number to increase
   */
{
  ELog::RegMethod RegA("Visit","procDist");

  long int outCnt(-1);
  while(unitLen>Geometry::zeroTol)
    {
      fullLen -= aFrac*lStep;
      unitLen -= aFrac*lStep;
      aFrac=1.0;
      outCnt++;
    }
  if (unitLen<0.0)
    {
      aFrac+=unitLen/lStep; // this is negative and deal with a<1.0
      fullLen+=aFrac*lStep;
    }
  return outCnt;
}

long int
Visit::procPoint(double& unitLen,const double lStep)
  /*!
    \param unitLen :: Single unit length
    \param lStep :: Step length
    \return number to increase
  */
{
  ELog::RegMethod RegA("Visit","procPoint");

  const long int outCnt=static_cast<long int>(unitLen/lStep);
  unitLen -= static_cast<double>(outCnt)*lStep;
  return outCnt;
}
 
void
Visit::populatePoint(const Simulation& System,
		     const std::set<std::string>& Active)
  /*!
    The big population call
    \param System :: Simulation system
    \param Active :: Active set of cells to use (ranged)
   */
{
  ELog::RegMethod RegA("Visit","populatePoint");

  MonteCarlo::Object* ObjPtr(0);
  Geometry::Vec3D aVec;

  const bool aEmptyFlag=Active.empty();

  double stepXYZ[3];
  for(size_t i=0;i<3;i++)
    stepXYZ[i]=XYZ[i]/static_cast<double>(nPts[i]);

  for(size_t i=0;i<nPts[0];i++)
    {
      aVec[0]=stepXYZ[0]*(static_cast<double>(i)+0.5);
      for(size_t j=0;j<nPts[1];j++)
        {
	  aVec[1]=stepXYZ[1]*(0.5+static_cast<double>(j));
	  for(size_t k=0;k<nPts[2];k++)
	    {
	      aVec[2]=stepXYZ[2]*(0.5+static_cast<double>(k));
	      const Geometry::Vec3D Pt=Origin+aVec;
	      ObjPtr=System.findCell(Pt,ObjPtr);
	      
	      // Active Set Code:
	      if (!aEmptyFlag)
		{
		  const std::string rangeStr=
		    System.inRange(ObjPtr->getName());
		  
		  if (Active.find(rangeStr)!=Active.end())
		    mesh.get()[i][j][k]=getResult(ObjPtr);
		  else
		    mesh.get()[i][j][k]=0.0;
		}
	      // OLD Code:
	      else
		{
		  if (!ObjPtr)
		    ELog::EM<<"Zero Cell == "<<Pt<<ELog::endErr;
		  mesh.get()[i][j][k]=getResult(ObjPtr);
		}

	    }
	}
    }
  return;
}

void
Visit::populate(const Simulation& System,
		const std::set<std::string>& Active)
  /*!
    The big population call
    \param SimPtr :: Simulation system
    \param Active Cells
   */
{
  ELog::RegMethod RegA("Visit","populate(sim)");

  if (lineAverage)
    populateLine(System,Active);
  else
    populatePoint(System,Active);
  
  return;
}

void
Visit::writeVTK(const std::string& FName) const
  /*!
    Write out a VTK cell
    \param FName :: filename 
  */
{
  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());
  std::ostringstream cx;
  
  double stepXYZ[3];
  for(size_t i=0;i<3;i++)
    stepXYZ[i]=XYZ[i]/static_cast<double>(nPts[i]);
  
  OX<<"# vtk DataFile Version 2.0"<<std::endl;
  OX<<"Geometry Data"<<std::endl;
  OX<<"ASCII"<<std::endl;
  OX<<"DATASET RECTILINEAR_GRID"<<std::endl;
  OX<<"DIMENSIONS "<<nPts[0]<<" "<<nPts[1]<<" "<<nPts[2]<<std::endl;
  OX<<"X_COORDINATES "<<nPts[0]<<" float"<<std::endl;
  for(size_t i=0;i<nPts[0];i++)
    OX<<fmt::format("{:<11.6g}   ",
		    (Origin[0]+stepXYZ[0]*(static_cast<double>(i)+0.5)));
  OX<<std::endl;

  OX<<"Y_COORDINATES "<<nPts[1]<<" float"<<std::endl;
  for(size_t i=0;i<nPts[1];i++)
    OX<<fmt::format("{:<11.6g}   ",
		    (Origin[1]+stepXYZ[1]*(static_cast<double>(i)+0.5)));
  OX<<std::endl;

  OX<<"Z_COORDINATES "<<nPts[2]<<" float"<<std::endl;
  for(size_t i=0;i<nPts[2];i++)
    OX<<fmt::format("{:<11.6g}   ",
		    (Origin[2]+stepXYZ[2]*(static_cast<double>(i)+0.5)));
  OX<<std::endl;

  OX<<"POINT_DATA "<<nPts[0]*nPts[1]*nPts[2]<<std::endl;
  OX<<"SCALARS cellID float 1.0"<<std::endl;
  OX<<"LOOKUP_TABLE default"<<std::endl;

  for(size_t k=0;k<nPts[2];k++)
    for(size_t j=0;j<nPts[1];j++)
      {
	for(size_t i=0;i<nPts[0];i++)
	  OX<<fmt::format("{:<11.6g}   ",
			  static_cast<double>(mesh.get()[i][j][k]));
	OX<<std::endl;
      }
  OX.close();
  return;
}

void
Visit::writeIntegerVTK(const std::string& FName) const
  /*!
    Write out a VTK cell
    \param FName :: filename 
  */
{
  if (FName.empty()) return;
  std::ofstream OX(FName.c_str());
  std::ostringstream cx;
  
  double stepXYZ[3];
  for(size_t i=0;i<3;i++)
    stepXYZ[i]=XYZ[i]/static_cast<double>(nPts[i]);
  
  OX<<"# vtk DataFile Version 2.0"<<std::endl;
  OX<<"Geometry Data"<<std::endl;
  OX<<"ASCII"<<std::endl;
  OX<<"DATASET RECTILINEAR_GRID"<<std::endl;
  OX<<"DIMENSIONS "<<nPts[0]<<" "<<nPts[1]<<" "<<nPts[2]<<std::endl;
  OX<<"X_COORDINATES "<<nPts[0]<<" float"<<std::endl;
  for(size_t i=0;i<nPts[0];i++)
    OX<<fmt::format("{:<11.6g}   ",
		    (Origin[0]+stepXYZ[0]*(static_cast<double>(i)+0.5)));
  OX<<std::endl;

  OX<<"Y_COORDINATES "<<nPts[1]<<" float"<<std::endl;
  for(size_t i=0;i<nPts[1];i++)
    OX<<fmt::format("{:<11.6g}   ",
		    (Origin[1]+stepXYZ[1]*(static_cast<double>(i)+0.5)));
  OX<<std::endl;

  OX<<"Z_COORDINATES "<<nPts[2]<<" float"<<std::endl;
  for(size_t i=0;i<nPts[2];i++)
    OX<<fmt::format("{:<11.6g}   ",
		    (Origin[2]+stepXYZ[2]*(static_cast<double>(i)+0.5)));
  OX<<std::endl;

  OX<<"POINT_DATA "<<nPts[0]*nPts[1]*nPts[2]<<std::endl;
  OX<<"SCALARS cellID int 1.0"<<std::endl;
  OX<<"LOOKUP_TABLE default"<<std::endl;


  for(size_t k=0;k<nPts[2];k++)
    for(size_t j=0;j<nPts[1];j++)
      {
	for(size_t i=0;i<nPts[0];i++)
	  OX<<fmt::format("{:<12d}  ",
			  static_cast<int>(std::round(mesh.get()[i][j][k])));
	OX<<std::endl;
      }
  OX.close();
  return;
}
