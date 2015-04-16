/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/chipDataStore.cxx
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
#include <memory>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "chipDataStore.h"

namespace chipIRDatum
{

chipDataStore&
chipDataStore::Instance()
  /*!
    Singleton constructor
    \return chipDataStore This
  */
{
  static chipDataStore A;
  return A;
}

chipDataStore::chipDataStore()  :
  U(mm),CUnits(CTotal),DUnits(DTotal),EUnits(ETotal),
  MUnits(MTotal),VUnits(VTotal),Cflag(CTotal),
  Dflag(DTotal),Eflag(ETotal),Vflag(VTotal),
  CRef(CTotal),DRef(DTotal),ERef(ETotal),VRef(VTotal)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{
  fill(CUnits.begin(),CUnits.end(),cm);
  fill(DUnits.begin(),DUnits.end(),cm);
  fill(EUnits.begin(),EUnits.end(),cm);
  fill(MUnits.begin(),MUnits.end(),cm);
  fill(VUnits.begin(),VUnits.end(),cm);
 
  CUnits[shutterAxis]=none;
  CUnits[preCollAxis]=none;
  CUnits[collVAxis]=none;
  CUnits[collHAxis]=none;
  CUnits[insertAxis]=none;

  EUnits[zoomBendStartNorm]=none;
  EUnits[zoomBendExitNorm]=none;

  // Knowns:
  Cflag[0]=1;
  CRef[0]=Geometry::Vec3D(0,0,0);
  CName.push_back("Target Start Point");
  CName.push_back("VoidVessel Target Centre");
  CName.push_back("ChipIR Start Point");
  CName.push_back("W1 Beam Direction");
  CName.push_back("ChipIR Beam direction (from C3)");
  CName.push_back("Secondary Scatterer Impact point");
  CName.push_back("Secondary Scatterer centre positon");
  CName.push_back("Shutter:shinePt1");
  CName.push_back("Shutter:shinePt2");
  CName.push_back("Shutter:shinePt3");
  CName.push_back("Shutter:shinePt4");
  CName.push_back("Shutter:shinePt5");
  CName.push_back("Shutter:shinePt6");
  CName.push_back("Shutter:shinePt7");
  CName.push_back("Shutter:shinePt8");
  CName.push_back("Collimator Vertical Origin");
  CName.push_back("Collimator Vertical Axis");
  CName.push_back("Collimator Horrizontal Origin");
  CName.push_back("Collimator Horrizontal Axis");
  CName.push_back("PreCollimator Origin");
  CName.push_back("PreCollimator Axis");
  CName.push_back("PreCollimator Hole Centre");
  CName.push_back("Insert Start");
  CName.push_back("Insert End");
  CName.push_back("Insert Axis");
  CName.push_back("Guide:innerPt1");
  CName.push_back("Guide:innerPt2");
  CName.push_back("Guide:innerPt3");
  CName.push_back("Guide:innerPt4");
  CName.push_back("Guide:innerPt5");
  CName.push_back("Guide:innerPt6");
  CName.push_back("Guide:innerPt7");
  CName.push_back("Guide:innerPt8");
  CName.push_back("Filter Position");
  CName.push_back("Blockhouse Start");
  CName.push_back("Collimator");
  CName.push_back("Table 1 Centre");
  CName.push_back("Table 2 Centre");
  CName.push_back("beamStop");

  DName.push_back("ShutterPt1");
  DName.push_back("ShutterPt2");
  DName.push_back("ShutterPt3");
  DName.push_back("ShutterPt4");
  DName.push_back("ShutterPt5");
  DName.push_back("ShutterPt6");
  DName.push_back("ShutterPt7");
  DName.push_back("ShutterPt8");
  DName.push_back("ShutterMidPtA");
  DName.push_back("ShutterMidPtB");
  DName.push_back("Table1Intercept with shutter axis");
  DName.push_back("Table2Intercept with shutter axis");
  DName.push_back("Guide Centre Point [at bulk wall]");
  DName.push_back("Hutch right Angle-Flat Apex");
  DName.push_back("Flood Centre Point");
  DName.push_back("Flood Corner 1");
  DName.push_back("Flood Corner 2");
  DName.push_back("Flood Corner 3");
  DName.push_back("Flood Corner 4");

  EName.push_back("Zoom Bender shutter exit");
  EName.push_back("Zoom Bender start");
  EName.push_back("Zoom Bender centre");
  EName.push_back("Zoom Bender exit");
  EName.push_back("Zoom Bender start-normal");
  EName.push_back("Zoom Bender exit-normal");
  EName.push_back("Zoom Box exit");
  EName.push_back("Zoom Guide exit");
  EName.push_back("Zoom Shutter exit");

  MName.push_back("Groove Radius Centre");

  VName.push_back("torpedo Outer Radius");
  VName.push_back("Shutter Outer Radius");
  VName.push_back("Inner Bulk Outer Radius");
  VName.push_back("Outer Bulk Outer Radius");
  VName.push_back("Zoom Bend Radius");
  VName.push_back("Zoom Bend Length");
  VName.push_back("ChipIR Insert Radius");
}

void
chipDataStore::setCNum(const CPENUM A,const Geometry::Vec3D& Pt) 
  /*!
    Set the C value Datum points
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A<0 || A>=CTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setCNum");
      throw ColErr::IndexError<long int>(A,CTotal,RegA.getFull());
    }
  Cflag[A]=1;
  CRef[A]=Pt;
  return;
}

void
chipDataStore::setDNum(const DPENUM A,const Geometry::Vec3D& Pt) 
  /*!
    Set the D datum point
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A<0 || A>=DTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setDNum");
      throw ColErr::IndexError<long int>(A,DTotal,RegA.getFull());
    }
  Dflag[A]=1;
  DRef[A]=Pt;
  return;
}

void
chipDataStore::setENum(const EPENUM A,const Geometry::Vec3D& Pt) 
  /*!
    Set the E value Datum points
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A<0 || A>=ETotal)
    {
      ELog::RegMethod RegA("chipDataStore","setENum");
      throw ColErr::IndexError<long int>(A,ETotal,RegA.getFull());
    }
  Eflag[A]=1;
  ERef[A]=Pt;
  return;
}

void
chipDataStore::setMNum(const MPENUM A,const Geometry::Vec3D& Pt) 
  /*!
    Set the M value Datum points
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A<0 || A>=MTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setMNum");
      throw ColErr::IndexError<long int>(A,MTotal,RegA.getFull());
    }
  Mflag[A]=1;
  MRef[A]=Pt;
  return;
}

void
chipDataStore::setVNum(const VPENUM A,const double& Pt) 
  /*!
    Set the datum point
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A<0 || A>=VTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setVNum");
      throw ColErr::IndexError<size_t>(A,VTotal,RegA.getFull());
    }
  Vflag[A]=1;
  VRef[A]=Pt;
  return;
}

void
chipDataStore::setCNum(const size_t A,const Geometry::Vec3D& Pt) 
  /*!
    Set the C value Datum points
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A>=CTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setCNum");
      throw ColErr::IndexError<size_t>(A,CTotal,RegA.getFull());
    }
  Cflag[A]=1;
  CRef[A]=Pt;
  return;
}

void
chipDataStore::setDNum(const size_t A,const Geometry::Vec3D& Pt) 
  /*!
    Set the D datum point
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A>=DTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setDNum");
      throw ColErr::IndexError<size_t>(A,DTotal,RegA.getFull());
    }
  Dflag[A]=1;
  DRef[A]=Pt;
  return;
}

void
chipDataStore::setENum(const size_t A,const Geometry::Vec3D& Pt) 
  /*!
    Set the E value Datum points
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A>=ETotal)
    {
      ELog::RegMethod RegA("chipDataStore","setENum");
      throw ColErr::IndexError<size_t>(A,ETotal,RegA.getFull());
    }
  Eflag[A]=1;
  ERef[A]=Pt;
  return;
}

void
chipDataStore::setMNum(const size_t A,const Geometry::Vec3D& Pt) 
  /*!
    Set the M value Datum points
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A>=ETotal)
    {
      ELog::RegMethod RegA("chipDataStore","setMNum");
      throw ColErr::IndexError<size_t>(A,MTotal,RegA.getFull());
    }
  Mflag[A]=1;
  MRef[A]=Pt;
  return;
}

void
chipDataStore::setVNum(const size_t A,const double& Pt) 
  /*!
    Set the datum point
    \param A :: Index value
    \param Pt :: Point value to est
  */
{
  if (A>=VTotal)
    {
      ELog::RegMethod RegA("chipDataStore","setVNum");
      throw ColErr::IndexError<size_t>(A,VTotal,RegA.getFull());
    }
  Vflag[A]=1;
  VRef[A]=Pt;
  return;
}

void
chipDataStore::postProcess()
  /*!
    Post processing to pick up values required
  */
{
  ELog::RegMethod RegA("chipDataStore","postProcess");

  // Get mid point:
  DRef[shutterMidPtA]=Geometry::Vec3D(0,0,0);
  DRef[shutterMidPtB]=Geometry::Vec3D(0,0,0);
  Dflag[shutterMidPtA]=0;
  Dflag[shutterMidPtB]=0;
  size_t index;
  for(index=0;index<4 && Dflag[shutterPt1+index];index++)
    DRef[shutterMidPtA]+=DRef[shutterPt1+index];
  if (index==4)
    {
      DRef[shutterMidPtA]/=4.0;
      Dflag[shutterMidPtA]=1;
    }
  for(index=0;index<4 && Dflag[shutterPt5+index];index++)
    DRef[shutterMidPtB]+=DRef[shutterPt5+index];
  if (index==4)
    {
      DRef[shutterMidPtB]/=4.0;
      Dflag[shutterMidPtB]=1;
    }

  // Get table plane:
  if (Cflag[table1Cent] && Cflag[table2Cent] &&
      Dflag[shutterMidPtA] && Dflag[shutterMidPtB])
    {
      const Geometry::Vec3D TableAxis=CRef[table1Cent]-CRef[table2Cent];
      Geometry::Plane AP(0,0,CRef[table1Cent],TableAxis);
      Geometry::Plane BP(0,0,CRef[table2Cent],TableAxis);
      MonteCarlo::LineIntersectVisit LV(DRef[shutterMidPtB],
		      	DRef[shutterMidPtB]-DRef[shutterMidPtA]);
      DRef[tab1Inter]=LV.getPoint(&AP);
      LV.clearTrack();
      DRef[tab2Inter]=LV.getPoint(&BP);
      Dflag[tab1Inter]=1;
      Dflag[tab2Inter]=1;
    }

  return;
}

Geometry::Vec3D
chipDataStore::getCValue(const size_t index) const
  /*!
    Get the value for a point		       
    \param index :: Index value
    \return Scaled value
   */
{
  if (CUnits[index]==none || CUnits[index]==U)
    return CRef[index];
  double scale=(CUnits[index]==mm) ? 0.1 : 1.0;
  scale*=(U==mm) ? 10.0 : 1.0;
  return CRef[index]*scale;
}

Geometry::Vec3D
chipDataStore::getDValue(const size_t index) const
  /*!
    Get the value for a point		       
    \param index :: Index value
    \return Scaled value
   */
{
  if (DUnits[index]==none || DUnits[index]==U)
    return DRef[index];
  double scale=(DUnits[index]==mm) ? 0.1 : 1.0;
  scale*=(U==mm) ? 10.0 : 1.0;
  return DRef[index]*scale;
}


Geometry::Vec3D
chipDataStore::getEValue(const size_t index) const
  /*!
    Get the value for a point		       
    \param index :: Index value
    \return Scaled value
   */
{
  if (EUnits[index]==none || EUnits[index]==U)
    return ERef[index];
  double scale=(EUnits[index]==mm) ? 0.1 : 1.0;
  scale*=(U==mm) ? 10.0 : 1.0;
  return ERef[index]*scale;
}

Geometry::Vec3D
chipDataStore::getMValue(const size_t index) const
  /*!
    Get the value for a point		       
    \param index :: Index value
    \return Scaled value
   */
{
  if (MUnits[index]==none || MUnits[index]==U)
    return MRef[index];
  double scale=(MUnits[index]==mm) ? 0.1 : 1.0;
  scale*=(U==mm) ? 10.0 : 1.0;
  return MRef[index]*scale;
}

void
chipDataStore::writeMasterTable(const std::string& Fname) 
  /*!
    Write out the master table to a file
    \param Fname :: Filename
  */
{
  ELog::RegMethod RegA("chipDataStore","writeMasterTable");

  postProcess();

  ELog::EM<<"File for output :"<<Fname<<ELog::endDebug;
  boost::format CFMT("C%1$d : %2$s%|50t| == ");
  boost::format DFMT("D%1$d : %2$s%|50t| == ");
  boost::format EFMT("E%1$d : %2$s%|50t| == ");
  boost::format MFMT("E%1$d : %2$s%|50t| == ");
  boost::format VFMT("V%1$d : %2$s%|50t| == ");
  std::ofstream OX;

  const double uScale((U==mm) ? 10.0 : 1.0);
  const std::string uSym((U==mm) ? " (mm) " : " (cm) ");
    
  
  OX.open(Fname.c_str());
  for(size_t i=0;i<Cflag.size();i++)
    {
      if (Cflag[i])
	{
	  if (CUnits[i]==none)
	    OX<<(CFMT % (i+1) % CName[i])<<getCValue(i)<<""<<std::endl;
	  else
	    OX<<(CFMT % (i+1) % CName[i])<<getCValue(i)<<uSym<<std::endl;
	}
    }
  OX<<"---------------------------------"<<std::endl;
  for(size_t i=0;i<Dflag.size();i++)
    {
      if (Dflag[i])
	OX<<DFMT % (i+1) % DName[i]<<DRef[i]*uScale<<uSym<<std::endl;
    }

  OX<<"---------------------------------"<<std::endl;
  for(size_t i=0;i<Eflag.size();i++)
    {
	  if (EUnits[i]==none)
	    OX<<(EFMT % (i+1) % EName[i])<<getEValue(i)<<""<<std::endl;
	  else
	    OX<<(EFMT % (i+1) % EName[i])<<getEValue(i)<<uSym<<std::endl;
    }
  OX<<"---------------------------------"<<std::endl;
  for(size_t i=0;i<Mflag.size();i++)
    {
      if (Mflag[i])
	OX<<MFMT % (i+1) % MName[i]<<MRef[i]*uScale<<uSym<<std::endl;
    }

  OX<<"---------------------------------"<<std::endl;
  for(size_t i=0;i<Vflag.size();i++)
    {
      if (Vflag[i])
	OX<<VFMT % (i+1) % VName[i]<<VRef[i]*uScale<<uSym<<std::endl;
    }
  OX.close();
  return;
}
 
 
}  // NAMESPACE hutchSystem
