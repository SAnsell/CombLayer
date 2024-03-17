/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visit/FEM.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineTrack.h"
#include "dataSlice.h"
#include "multiData.h"
#include "FEMdatabase.h"
#include "FEM.h"

FEM::FEM() :
  lineAverage(0),nPts({0,0,0})
  /*!
    Constructor
  */
{}

FEM::FEM(const FEM& A) : 
  lineAverage(A.lineAverage),
  Origin(A.Origin),XYZ(A.XYZ),nPts(A.nPts),
  matMesh(A.matMesh)
  /*!
    Copy constructor
    \param A :: FEM to copy
  */
{};

FEM&
FEM::operator=(const FEM& A)
  /*!
    Assignment operator
    \param A :: FEM to copy
    \return *this
  */
{
  if (this!=&A)
    {
      lineAverage=A.lineAverage;
      Origin=A.Origin;
      XYZ=A.XYZ;
      nPts=A.nPts;
      matMesh=A.matMesh;
    }
  return *this;
}

FEM::~FEM()
  /*!
    Destructor
   */
{}

size_t
FEM::procPoint(double& unitLen,const double lStep)
  /*!
    \param unitLen :: Single unit length
    \param lStep :: Step length
    \return number to increase
  */
{
  //  ELog::RegMethod RegA("FEM","procPoint");

  const size_t outCnt=static_cast<size_t>(unitLen/lStep);
  unitLen -= static_cast<double>(outCnt)*lStep;
  return outCnt;
}

size_t
FEM::getMaxIndex() const
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
FEM::setBox(const Geometry::Vec3D& startPt,
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
FEM::setIndex(const size_t A,const size_t B,const size_t C)
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
  matMesh.resize(A,B,C);
  rhoCp.resize(A,B,C);
  K.resize(A,B,C);
  return;
}

int&
FEM::getMeshUnit(const size_t posIndex,const size_t index,
		 const size_t i,const size_t j)
  /*!
    Get mesh unit based on posIndex being index place [0-2]
    and i,j begin the other two coordinates
    \param posIndex :: Index of Max direction (0-2)
    \param i :: other index in mesh
    \param j :: other index in mesh
    \return matMesh[i][j] / [index]
  */
{ 
  if (posIndex==0)
    return matMesh.get()[index][i][j];
  else if (posIndex==1)
    return matMesh.get()[i][index][j];
  
  return matMesh.get()[i][j][index];
}

double&
FEM::getRhoUnit(const size_t posIndex,const size_t index,
		 const size_t i,const size_t j)
  /*!
    Get mesh unit based on posIndex being index place [0-2]
    and i,j begin the other two coordinates
    \param posIndex :: Index of Max direction (0-2)
    \param i :: other index in mesh
    \param j :: other index in mesh
    \return matMesh[i][j] / [index]
  */
{ 
  if (posIndex==0)
    return rhoCp.get()[index][i][j];
  else if (posIndex==1)
    return rhoCp.get()[i][index][j];
  
  return rhoCp.get()[i][j][index];
}

double&
FEM::getKUnit(const size_t posIndex,
	      const size_t index,
	      const size_t i,const size_t j)
  /*!
    Get mesh unit based on posIndex being index place [0-2]
    and i,j begin the other two coordinates
    \param posIndex :: Index of Max direction (0-2)
    \param index :: index of posIndex mesh unit
    \param i :: other index in mesh
    \param j :: other index in mesh
    \return K[i][j] / [index]
  */
{ 
  if (posIndex==0)
    return K.get()[index][i][j];
  else if (posIndex==1)
    return K.get()[i][index][j];
  
  return K.get()[i][j][index];
}

void
FEM::populateLine(const Simulation& System)
  /*!
    The big population call with lines
    \param System :: Simulation system
    \param Active :: Active set of cells to use (ranged)
   */
{
  ELog::RegMethod RegA("FEM","populateLine");

  // First get max N
  const size_t IMax=getMaxIndex();
  const size_t IA = (IMax) ? 0: 1;
  const size_t IB = (IMax!=2) ? 2 : 1;
  
  const long int nA = static_cast<long int>(nPts[IA]);
  const long int nB = static_cast<long int>(nPts[IB]);
    
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

  for(long int i=0;i<nA;i++)
    for(long int j=0;j<nB;j++)
      { 
	std::vector<double> distVec;
	std::vector<MonteCarlo::Object*> cellVec;
	
	const Geometry::Vec3D aVec=Origin+
	  XStep*(static_cast<double>(i)+0.5)+
	  YStep*(static_cast<double>(j)+0.5);

	ModelSupport::LineTrack OTrack(aVec,aVec+longStep);
	OTrack.calculate(System);
	OTrack.createCellPath(cellVec,distVec);

	double T=0.0;
	long int index(0);
	for(size_t ii=0;ii<cellVec.size();ii++)
	  {
	    T+=distVec[ii];
	    const size_t mid=FEM::procPoint(T,stepXYZ[IMax]);
	    const int mValue=cellVec[ii]->getMatID();

	    for(size_t cnt=0;cnt<mid;cnt++)
	      getMeshUnit(IMax,index++,i,j)=mValue;
	  }
      }  
  return;
}

void
FEM::populatePoint(const Simulation& System)
  /*!
    The big population call
    \param System :: Simulation system
   */
{
  ELog::RegMethod RegA("FEM","populatePoint");

  MonteCarlo::Object* ObjPtr(0);
  Geometry::Vec3D aVec;

  const sliceUnit<int> MAT=matMesh.get();
  
  if (nPts[0]>=1 && nPts[1]>=1 && nPts[2]>=1)
    {
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
		  
		  if (!ObjPtr)
		    ELog::EM<<"Zero Cell == "<<Pt<<ELog::endErr;
		  MAT[i][j][k]=ObjPtr->getMatID();
		}

	    }
	}
    }
  return;
}

void
FEM::populate(const Simulation& System)
  /*!
    The big population call
    \param SimPtr :: Simulation system
   */
{
  ELog::RegMethod RegA("FEM","populate(sim)");

  if (lineAverage)
    populateLine(System);
  else
    populatePoint(System);

  matInventory.clear();
  const ModelSupport::FEMdatabase& FM=
    ModelSupport::FEMdatabase::Instance();
  
  const std::vector<int>& matV=matMesh.getVector();
  std::vector<double>& rVec=rhoCp.getVector();
  std::vector<double>& kVec=K.getVector();
      
  for(size_t i=0;i<matV.size();i++)
    {
      rVec[i]=FM.getRhoCp(matV[i]);
      kVec[i]=FM.getK(matV[i]);
      if (matV[i])
	matInventory.emplace(matV[i]);
    }
  
  return;
}

void
FEM::writeFEM(const std::string& FName) const
  /*!
    Write out a FreeFEM mesh file using rectangular-triangle
    coordinate frame
    \param FName :: filename 
  */
{
  ELog::RegMethod RegA("FEM","writeFEM");
  
  if (FName.empty()) return;

    
  const ModelSupport::FEMdatabase& FM=
    ModelSupport::FEMdatabase::Instance();
  
  std::ofstream OX(FName.c_str());
 
  OX<<"# FEM DataFile Version"<<std::endl;
  OX<<"# Geometry Data"<<std::endl;
  OX<<"# NPts Low : High"<<std::endl;
  OX<<"Index : "<<nPts[0]<<" "<<nPts[1]<<" "<<nPts[2]<<std::endl;
  OX<<"Low : "<< Origin<<std::endl;
  OX<<"High : "<< Origin+XYZ<<std::endl;
  for(const int m : matInventory)
    OX<<"Mat : "<<m<<" : "<<FM.getKey(m)<<std::endl;
  OX<<"END HEADER"<<std::endl;

  const sliceUnit<const int> MAT=matMesh.get();
  const sliceUnit<const double> Rho=rhoCp.get();
  const sliceUnit<const double> KGet=K.get();

  const size_t nX(nPts[0]);
  const size_t nY(nPts[1]);
  const size_t nZ(nPts[2]);
  
  const Geometry::Vec3D aX(XYZ.X()/static_cast<double>(nX),0,0);
  const Geometry::Vec3D aY(0,XYZ.Y()/static_cast<double>(nY),0);
  const Geometry::Vec3D aZ(0,0,XYZ.Z()/static_cast<double>(nZ));

  
  
  for(size_t i=0;i<nX;i++)  
    for(size_t j=0;j<nY;j++)
      for(size_t k=0;k<nZ;k++)
	{
	  const Geometry::Vec3D Pt=Origin
	    +aX*(static_cast<double>(i)+0.5)
	    +aY*(static_cast<double>(j)+0.5)
	    +aZ*(static_cast<double>(k)+0.5);

	  OX<<i<<" "<<j<<" "<<k<<" "
	    <<Pt<<"     "
	    <<matMesh.get()[i][j][k]<<" "<<Rho[i][j][k]
	    <<" "<<KGet[i][j][k]<<std::endl;
      }
  OX.close();
  return;
}
