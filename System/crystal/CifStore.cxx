/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   crystal/CifStore.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <algorithm>
#include <functional>
#include <memory>
#include <limits>
#include <boost/multi_array.hpp>

#include "MersenneTwister.h"
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "support.h"
#include "mathSupport.h"
#include "Element.h"
#include "SymUnit.h"
#include "AtomPos.h"
#include "loopItem.h"
#include "CifItem.h"
#include "CifLoop.h"
#include "CifStore.h"

extern MTRand RNG;

namespace Crystal
{

CifStore::CifStore() : 
  tableNumber(0),exafsAtom(std::numeric_limits<size_t>::max()),
  exafsType(-1)
  /// Default Constructor
{}

CifStore::CifStore(const CifStore& A) :
  cellAxis(A.cellAxis),cellAngle(A.cellAngle),
  tableNumber(A.tableNumber),exafsAtom(A.exafsAtom),
  exafsType(A.exafsType),Zmap(A.Zmap),Nmap(A.Nmap),
  Z(A.Z),Ions(A.Ions),Sym(A.Sym),Atoms(A.Atoms),
  Cell(A.Cell),FullCell(A.FullCell)
  /*!
    Copy Constructor
    \param A :: CifStore to copy
   */
{
  for(int i=0;i<3;i++)
    {
      unitVec[i]=A.unitVec[i];
      recipVec[i]=A.recipVec[i];
    }
}

CifStore&
CifStore::operator=(const CifStore& A) 
  /*!
    Assignment operator
    \param A :: CifStore to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellAxis=A.cellAxis;
      cellAngle=A.cellAngle;
      for(int i=0;i<3;i++)
        {
	  unitVec[i]=A.unitVec[i];
	  recipVec[i]=A.recipVec[i];
	}
      tableNumber=A.tableNumber;
      exafsAtom=A.exafsAtom;
      exafsType=A.exafsType;
      Zmap=A.Zmap;
      Nmap=A.Nmap;
      Z=A.Z;
      Ions=A.Ions;
      Sym=A.Sym;
      Atoms=A.Atoms;
      Cell=A.Cell;
      FullCell=A.FullCell;
    }
  return *this;
}
  
CifStore::~CifStore()
  /// Destructor 
{}


void
CifStore::insertElement(const std::string& KeyName,double Ion)
  /*!
    Inserts an element and ion into the main list
    \param KeyName :: Name of ion
    \param Ion :: Ion 
  */
{
  ELog::RegMethod RegA("CifStore","insertElement");
  
  if (!KeyName.empty())
    {
      ZTYPE::const_iterator mc=Zmap.find(KeyName);
      if (mc==Zmap.end())
        {
	  std::pair<int,int> ZI=
	    MonteCarlo::Element::Instance().elmIonPair(KeyName);
	  if (Ion<-100) Ion=ZI.second;
	  if (ZI.first>0)
	    {
	      Z.push_back(ZI.first);
	      Ions.push_back(static_cast<int>(Ion));
	      Zmap.emplace(KeyName,Z.size());
	    }
	}
    }
  return;
}

void 
CifStore::insertAtom(const std::string& Site,const std::string& Elm,
		     const Geometry::Vec3D& Pt, const double Occ)
  /*!
    Insert an atom
    \param Site :: Site item name [Elm+Index]
    \param Elm :: Element identifier [Elm+Ion]
    \param Pt :: Point in fractional coordinates
    \param Occ :: Occupancy number
  */
{

  if (!Site.empty() && !Elm.empty())
    {
      // Determine if we need to update  Zmap
      insertElement(Elm);
      ZTYPE::const_iterator zc=Zmap.find(Elm);
      if (zc==Zmap.end())
	return;
      
      // Add atom:
      Atoms.push_back(AtomPos(Site,Z[zc->second],
			      Ions[zc->second],Pt,Occ));

      // Update Nmap 
      if (Nmap.find(Site)==Nmap.end())
	Nmap.insert(NTYPE::value_type(Site,zc->second));
    }
  return;
}

int
CifStore::readAtoms(const CifLoop& CLoop)
  /*!
    Activated on atom_site
    \param CLoop :: CifLoop to process
    \retval 1 :: success
    \retval 0 :: not valid
    \retval -1 :: failure
  */
{  
  ELog::RegMethod RegA("CifStore","readAtoms");

  std::vector<std::string> ItemNames;
  ItemNames.push_back("_atom_site_label");
  ItemNames.push_back("_atom_site_type_symbol");
  ItemNames.push_back("_atom_site_fract_x");
  ItemNames.push_back("_atom_site_fract_y");
  ItemNames.push_back("_atom_site_fract_z");
  ItemNames.push_back("_atom_site_occupancy");

  std::vector<size_t> IntVec;
  const int nIndex=CLoop.getIntFromKey(ItemNames,IntVec);
  if (nIndex<4) 
    return 0;

  int retval(1);
  std::string SiteName;
  std::string ElmName;
  Geometry::Vec3D Pt;
  const size_t lneCnt(CLoop.getLineCnt());
  for(size_t i=0;i<lneCnt;i++)
    {
      double occ(1.0);
      const int NFlag=CLoop.getItem(i,IntVec[0],SiteName)+
	2*CLoop.getItem(i,IntVec[1],ElmName);
      if (!NFlag) 
	retval=-1;
      else if ((NFlag & 1))
	ElmName=SiteName;
      else
	SiteName=ElmName;

      if (!CLoop.getItem(i,IntVec[2],Pt[0]) ||
	  !CLoop.getItem(i,IntVec[3],Pt[1]) ||
	  !CLoop.getItem(i,IntVec[4],Pt[2]) )
	retval=-1;
      CLoop.getItem(i,IntVec[5],occ);
      
      if (retval>0)
	{
	  insertAtom(SiteName,ElmName,Pt,occ);
	}
      else
        {
	  ELog::EM<<"Error with line :"<<i<<ELog::endErr;
	  return retval;
	}
    }
  return retval;
}

int
CifStore::readSym(const CifLoop& CLoop)
  /*!
    Activated on symmetry_equ
    \param CLoop :: CLoop object to process
    \retval 1 :: success
    \retval 0 :: not valid
    \retval -1 :: failure
  */
{
  ELog::RegMethod RegA("CifStore","readSym");

  std::vector<std::string> ItemNames;
  ItemNames.push_back("_symmetry_equiv_pos_as_xyz");

  std::vector<size_t> IntVec;
  const int nIndex=CLoop.getIntFromKey(ItemNames,IntVec);
  if (nIndex<1)  return 0;

  int retval(1);
  const size_t lneCnt(CLoop.getLineCnt());
  std::string SymElm;
  SymUnit A;
  for(size_t i=0;i<lneCnt;i++)
    {
      if (CLoop.getItem(i,IntVec[0],SymElm) &&
	  !A.setLine(SymElm)) 
        { 
	  Sym.push_back(A);
	}
      else
        {
	  ELog::EM<<"Error with on line "<<i<<ELog::endErr;
	  retval=-1;
	}
    }
  return retval;
}

int
CifStore::readTypes(const CifLoop& CLoop)
  /*!
    Read an populate the symbol list
    - based on atom_type key
    \param CLoop :: CifLoop component
    \retval 1 :: success
    \retval 0 :: not valid
    \retval -1 :: failure
  */
{
  std::vector<std::string> ItemNames;
  ItemNames.push_back("_atom_type_symbol");
  ItemNames.push_back("_atom_type_oxidation_number");

  std::vector<size_t> IntVec;
  const int nIndex=CLoop.getIntFromKey(ItemNames,IntVec);
  if (nIndex<1)  return 0;

  double ion(0.0);
  std::string SymElm;
  const size_t lneCnt(CLoop.getLineCnt());
  for(size_t i=0;i<lneCnt;i++)
    {
      if (CLoop.getItem(i,IntVec[0],SymElm) &&
	  (nIndex<2 || CLoop.getItem(i,IntVec[1],ion) ))
        { 
	  if (nIndex<2)
	    insertElement(SymElm);
	  else
	    insertElement(SymElm,ion);
	}
      else
        {
	  ELog::EM<<"Cif::readTypes Error with on line "<<i<<ELog::endErr;
	  return -1;
	}
    }
  return 1;
}


int
CifStore::readFile(const std::string& Fname)
  /*!
    Process a CIF file:
    \param Fname :: File to open
    \return 0 :: success
   */
{
  ELog::RegMethod RControl("CifStore","readFile");

  enum EBits { 
    cell_Length_a=1,cell_Length_b=2,
    cell_Length_c=4,cell_angle_alpha=8,
    cell_angle_beta=16,cell_angle_gamma=32,
    symmetry_Int_Tables=64,
    atom_types=128,symmetry_equiv=256,
    atom_sites=512 
  };
	 
  std::ifstream IX;
  IX.open(Fname.c_str());
  if (!IX.good())
    return -1;
  unsigned int itemFlag(0);       // Items found [abc ABC loop atoms]:

  // Data 
  std::string Cmd;
  int INumber;
  
  int comment(0);       // In a comment block
  CifLoop LoopComp;     // Loop component
  CifItem ItemComp;     // Item component
  int errStatus(0);     // Status of file
  std::string Line;
  while(!errStatus && !IX.fail())
    {
      // PROCESS Comment block with ;   ; delimiters
      int flag;
      do 
        {
	  Line=StrFunc::getLine(IX);
	  Line=StrFunc::fullBlock(Line);
	  if (Line.empty())
	    flag=1;
	  else 
	    {
	      if (Line[0]==';')
		comment=1-comment;
	      flag=comment;
	    }
	}
      while (!IX.fail() && flag); 

      while(!errStatus && !Line.empty())
        {
	  // IF NOT IN LOOP / try ItemComp and LoopComp
	  try
	    {
	      if (!ItemComp.getStatus())
		LoopComp.registerLine(Line);
	      if (LoopComp.getStatus()>=3)         // Finished loop:
	        {
		  // Process loop
		  if (readAtoms(LoopComp)>0)
		    itemFlag |= atom_types;
		  else if (readSym(LoopComp)>0)
		    itemFlag |= symmetry_equiv;
		  else if (readTypes(LoopComp)>0)
		    itemFlag |= atom_sites;
		  LoopComp.clearAll();
		}
	      if (!LoopComp.getStatus())
		ItemComp.registerLine(Line);

	      if (ItemComp.getStatus()==2)
	        {
		  // BASIC:
		  if (ItemComp.hasKey("_cell_length_a"))
		    {
		      if (ItemComp.getItem(cellAxis[0]))
			itemFlag |= cell_Length_a;
		    }
		  else if (ItemComp.hasKey("_cell_length_b"))
		    {
		      if (ItemComp.getItem(cellAxis[1]))
			itemFlag |= cell_Length_b;
		    }
		  else if (ItemComp.hasKey("_cell_length_c"))
		    {
		      if (ItemComp.getItem(cellAxis[2]))
			itemFlag |= cell_Length_c;
		    }
		  else if (ItemComp.hasKey("_cell_angle_alpha"))
		    {
		      if (ItemComp.getItem(cellAngle[0]))
			itemFlag |= cell_angle_alpha;
		    }
		  else if (ItemComp.hasKey("_cell_angle_beta"))
		    {
		      if (ItemComp.getItem(cellAngle[1]))
			itemFlag |= cell_angle_beta;
		    }
		  else if (ItemComp.hasKey("_cell_angle_gamma"))
	            {
		      if (ItemComp.getItem(cellAngle[2]))
			itemFlag |= cell_angle_gamma;
		    }
		  else if (ItemComp.hasKey("_symmetry_Int_Tables_number"))
		    {
		      if (ItemComp.getItem(INumber))
			itemFlag |= symmetry_Int_Tables;
		    }
		  else
		    ItemComp.clearAll();
		}
	    }
	  catch (ColErr::ExBase& A)
	    {
	      ELog::EM<<"Error with CifFile :"<<A.what()<<ELog::endErr;
	      errStatus=2;
	    }
	}
    }
  IX.close();
  // CLOSE FILE
  if (itemFlag!=1023)
    {
      if ((itemFlag | symmetry_Int_Tables | atom_types | atom_sites)!=1023)
	{
	  ELog::EM<<"Failed at item :== "<<itemFlag<<std::endl;
	  if (!(itemFlag & cell_Length_a))
	    ELog::EM<<"cell_length_a"<<" ";
	  if (!(itemFlag & cell_Length_b))
	    ELog::EM<<"cell_length_b"<<" ";
	  if (!(itemFlag & cell_Length_c))
	    ELog::EM<<"cell_length_c"<<" ";
	  if (!(itemFlag & cell_angle_alpha))
	    ELog::EM<<"_cell_angle_alpha"<<" ";
	  if (!(itemFlag & cell_angle_beta))
	    ELog::EM<<"_cell_angle_beta"<<" ";
	  if (!(itemFlag & cell_angle_gamma))
	    ELog::EM<<"_cell_angle_gamma"<<" ";
	  if (!(itemFlag & symmetry_Int_Tables))
	    ELog::EM<<"_symmetry_Int_Table"<<" ";
	  if (!(itemFlag & symmetry_equiv))
	    ELog::EM<<"_symmetry_equiv_pos_as_xyz:loop"<<" ";
	  ELog::EM.error();
	  return -2;
	}

      ELog::EM<<"Warning at item :== "<<itemFlag<<std::endl;
      if (!(itemFlag & symmetry_Int_Tables))
	ELog::EM<<"_symmetry_Int_Table"<<" ";
      if (!(itemFlag & atom_types))
	ELog::EM<<"_atom_types:loop"<<" ";
      if (!(itemFlag & atom_sites))
	ELog::EM<<"_atom_sites:loop"<<" ";
      ELog::EM.warning();
    }
  createUnit();
  return 0;
}


double
CifStore::originDistance(const Geometry::Vec3D& Pt) const
  /*!
    Determine the distance from the origin in 
    cartesian coordinates
    \param Pt :: Point in fractional space (of unit cell)
    \return Distance from origin
  */
{
  const double alpha=M_PI*cellAngle[0]/180.0;
  const double beta=M_PI*cellAngle[1]/180.0;
  const double gamma=M_PI*cellAngle[2]/180.0;

  const double a=cellAxis[0];
  const double b=cellAxis[1];
  const double c=cellAxis[2];

  const double ca=cos(alpha);
  const double cb=cos(beta);
  const double cg=cos(gamma);
  
  Geometry::Matrix<double> G(3,3);
  for(size_t i=0;i<3;i++)
    G[i][i]=cellAxis[i]*cellAxis[i];

  G[0][1]=a*b*cg;
  G[1][0]=G[0][1];

  G[0][2]=a*c*cb;
  G[2][0]=G[0][2];

  G[1][2]=b*c*ca;
  G[2][1]=G[1][2];
  
  // Use matrix as rT G r == Dist^2
  
  const double T = Pt.dotProd(G*Pt);
  return sqrt(T);
}

double
CifStore::aveOcc() const
  /*!
    Calculate the average occupancy
    \return aveOcc
   */
{
  if (Cell.empty())
    return 1.0;

  double aveO(0.0);
  std::vector<AtomPos>::const_iterator vc;
  for(vc=Cell.begin();vc!=Cell.end();vc++)
    aveO+=vc->getOcc();
  aveO/=static_cast<int>(Cell.size());
  return aveO;
}


double 
CifStore::volume() const
  /*!
    Calculate the unit cell volume
    \return General unit cell volume
  */
{
  const double alpha=M_PI*cellAngle[0]/180.0;
  const double beta=M_PI*cellAngle[1]/180.0;
  const double gamma=M_PI*cellAngle[2]/180.0;
  const double ca=cos(alpha);
  const double cb=cos(beta);
  const double cg=cos(gamma);
  double V(1.0-(ca*ca+cb*cb+cg*cg)+2.0*ca*cb*cg);
  V=sqrt(V);
  for(int i=0;i<3;i++)
    V*=cellAxis[i];
  return V;
}

void
CifStore::createUnit()
  /*!
    Make the basis vectors:
    Checked for a coupled of unit vectors 
    This was checked: found to be in agreement with
    cif.
   */
{
  ELog::RegMethod RControl("CifStore","createUnit");

  const double alpha=M_PI*(cellAngle[0]/180.0);
  const double beta=M_PI*(cellAngle[1]/180.0);
  const double gamma=M_PI*(cellAngle[2]/180.0);

  const double a=cellAxis[0];
  const double b=cellAxis[1];
  const double c=cellAxis[2];
//  const double sa=std::sin(alpha);
  const double sb=std::sin(beta);
// NOT SURE ABOUT THIS
//  const double sg=std::sin(gamma);   

  const double ca=std::cos(alpha);
  const double cb=std::cos(beta);
  const double cg=std::cos(gamma);

  unitVec[0]=Geometry::Vec3D(a,0,0);
  unitVec[1]=Geometry::Vec3D(b*cg,b*(ca-cg*cb)/sb,
			     b*sqrt(sb*sb-cg*cg-ca*ca+2*ca*cb*cg)/sb);
  unitVec[2]=Geometry::Vec3D(c*cb,c*sb,0.0);
  V0=volume();

  for(int i=0;i<3;i++)
    {
      recipVec[i]=unitVec[(i+1) % 3]*unitVec[(i+2) % 3];
      recipVec[i]*=(2*M_PI/V0);
    }
  return;
}

void
CifStore::secondaryRotation(const Geometry::Vec3D& Axis,
			    const double Angle) 
  /*!
    Apply a secondary rotation
    \param Axis :: Axis of rotation
    \param Angle :: Angle to rotate 
  */
{
  const Geometry::Quaternion QA=
    Geometry::Quaternion::calcQRot(Angle,Axis);
  
  for(int i=0;i<3;i++)
    {
      QA.rotate(unitVec[i]);
      QA.rotate(recipVec[i]);
    }
  return;
}

void
CifStore::primaryRotation(const Triple<int>& zIndex,
			  const Geometry::Vec3D& zAxis,
			  const Triple<int>& xIndex,
			  const Geometry::Vec3D& xAxis)
  /*!
    Rotate the zIndex to the zAxis.
    Then rotate the xIndex to the xAxis (or as close as possible)
    keeping the zAxis correct.
    \param zIndex :: z-millar index [integer]
    \param zAxis :: real Z axis
    \param xIndex :: x-millar index [integer]
    \param xAxis :: real X axis
  */
{
  ELog::RegMethod RegA("CifStore","primaryRotation");
  // First rotate the zIndex vector to the zAxis
  Geometry::Vec3D zPlane=UVec(zIndex[0],zIndex[1],zIndex[2]);

  zPlane.makeUnit();
  const Geometry::Vec3D zAxisUnit=zAxis.unit();

  const Geometry::Vec3D zRot=zPlane*zAxisUnit;
  const double cosA= acos(zPlane.dotProd(zAxisUnit));
  
  const Geometry::Quaternion QZA=
    Geometry::Quaternion::calcQRot(cosA,zRot);

  for(int i=0;i<3;i++)
    {
      QZA.rotate(unitVec[i]);
      QZA.rotate(recipVec[i]);
    }
  // Now calculate the x 
  Geometry::Vec3D xPlane=UVec(xIndex[0],xIndex[1],xIndex[2]);
  xPlane.makeUnit();

  // Projection of vector into plane is : n * (X * n)
  const Geometry::Vec3D xProj= zAxisUnit*( xPlane * zAxisUnit);
  const Geometry::Vec3D lProj= zAxisUnit*( xAxis.unit() * zAxisUnit);

  const double cosL= acos(xProj.dotProd(lProj));
  const Geometry::Quaternion QXA=
    Geometry::Quaternion::calcQRot(cosL,zAxis);
  
  for(int i=0;i<3;i++)
    {
      QXA.rotate(unitVec[i]);
      QXA.rotate(recipVec[i]);
    }

  return;
}

const std::vector<AtomPos>&
CifStore::calcUnitCell()
  /*!
    Calculate the unit cell
    \return Vector of atoms
  */
{
  ELog::RegMethod RegA("CifStore","calcUnitCell");

  std::vector<AtomPos> CX;
  std::vector<AtomPos>::const_iterator ac;
  std::vector<SymUnit>::const_iterator sc;
  for(ac=Atoms.begin();ac!=Atoms.end();ac++)
    {
      AtomPos Comp(*ac);
      const Geometry::Vec3D& APoint(ac->getPos());
      int symCnt(0);
      for(sc=Sym.begin();sc!=Sym.end();sc++)
        {

	  Comp.setPos(sc->unitPoint(APoint));
	  CX.push_back(Comp);
	  symCnt++;
	}
    }

  if (CX.empty())
    {
      ELog::EM<<"No atoms in cell"<<ELog::endErr;
      return Cell;
    }
  // Now need find uniques
  sort(CX.begin(),CX.end());
  Cell.clear();
  ac=CX.begin();
  Cell.push_back(*ac);
  for(ac++;ac!=CX.end();ac++)
    {
      const int flag=Cell.back().equal(*ac);
      if (flag==-1)
        {
	  ELog::EM<<"ERROR WITH CELLS: "
		  <<(*ac)<<" : "<<"OTHER: "
		  <<Cell.back()<<ELog::endErr;
	}
      if (!flag)
	Cell.push_back(*ac);
    }
  ELog::EM<<"Size of Cell = "<<Cell.size()<<ELog::endDiag;
  return Cell;
}

Geometry::Vec3D
CifStore::makeCartisian(const Geometry::Vec3D& FPos) const
  /*!
    Convert the coordinate into a cartesian position
    \param FPos :: Fractional Position to convert.
    \return Cartisian position
   */
{
  Geometry::Vec3D realPt;
  for(int i=0;i<3;i++)
    realPt += unitVec[i]*FPos[i];
  return realPt;
}

int
CifStore::inCube(const Geometry::Vec3D& Pt,const double R) const
  /*!
    Test is a point is in the cube
    \param Pt :: Point to test
    \param R :: Radius
  */
{
  for(size_t i=0;i<3;i++)
    if (std::abs(Pt[i])>R)
      return 0;
  // success
  return 1;
}

size_t
CifStore::setCentralAtom(const std::string& CentAtom,const size_t index) 
  /*!
    Sets the central atom from the main list
    \param CentAtom :: Name of central atom 
    \param index :: index (0 == first)
    \return Position (-1 for fail to find)
  */
{
  ELog::RegMethod RControl("CifStore","setCentralAtom");
  exafsAtom=std::numeric_limits<size_t>::max();
  // DETERMINE THE CENTRAL TYPE:
  NTYPE::const_iterator mc=Nmap.find(CentAtom);
  if (mc!=Nmap.end())  // found in table
    {
      std::vector<size_t> CentAtomList;
      std::vector<AtomPos>::iterator ac=Cell.begin();
      while(ac!=Cell.end())
        {
	  ac=
	    find_if(ac,Cell.end(),
		    std::bind(
		      std::equal_to<std::string>(),
		      std::bind(&AtomPos::getName,std::placeholders::_1),
		      CentAtom));
	  if (ac!=Cell.end())
	    {
	      CentAtomList.push_back
		(static_cast<size_t>(std::distance(Cell.begin(),ac)));
	      
	      ac++;
	    }
	} 
      
      // Print warning  if failed ?
      if (!CentAtomList.empty())
        {
	  ELog::EM<<"Central Atom positions:"<<ELog::endDiag;
	  std::vector<int>::const_iterator vc;
	  for(size_t i=0;i<CentAtomList.size();i++)
	    ELog::EM<<i<<" :: "<<Cell[CentAtomList[i]]<<ELog::endDiag;

	  ELog::EM.basic();
	  if (index<CentAtomList.size())
	    {
	      exafsAtom=CentAtomList[index];
	      exafsType=mc->second; 
	    }
	}
    }
  if (exafsAtom>Atoms.size())
    ELog::EM<<"Failed to find atom:"
	    <<CentAtom<<"["<<index<<"]"<<ELog::endErr;

  return exafsAtom;
}

void
CifStore::makeAtoms(const double Radius)
  /*!
    Build atoms out to a particular radius
    Assumes that Cell is good and the unit cell
    is valid.
    \param Radius :: cut distance of cube
  */
{
  ELog::RegMethod RControl("CifStore","makeAtoms");  
  FullCell.clear();
  // CACL OFFSET:
  Geometry::Vec3D Offset,RPosOff;
  if (exafsAtom<Atoms.size())
    {
      ELog::EM<<"Offset Pos == "<<Cell[exafsAtom].getPos()<<ELog::endDiag;
      Offset=Cell[exafsAtom].getPos();
      RPosOff=makeCartisian(Offset);
      ELog::EM<<" Real Pos == "<<RPosOff<<ELog::endDiag;
    }

  // Basis is in cartisian coordinates: 
  Geometry::Vec3D Basis(Radius,Radius,Radius);
  if (Basis.reBase(unitVec[0],unitVec[1],unitVec[2]))
    {
      ELog::EM.error("Failed to reBase unit vector");
      return;
    }
  Geometry::Vec3D BCheck=makeCartisian(Basis);
  ELog::EM<<"Volume "<<volume()<<ELog::endDiag;
  ELog::EM<<"Basis "<<Basis<<ELog::endDiag;
  ELog::EM<<"BCheck "<<BCheck<<ELog::endDiag;
  ELog::EM<<"Offset "<<Offset<<ELog::endDiag;
  ELog::EM<<"Unit0: "<<unitVec[0]<<ELog::endDiag;
  ELog::EM<<"Unit1: "<<unitVec[1]<<ELog::endDiag;
  ELog::EM<<"Unit2: "<<unitVec[2]<<ELog::endDiag;
  ELog::EM.diagnostic();
  
  // loop over all the unit cells + 1 to ensure we miss no
  // atoms that are outside our +/-R cube
  const int Nx(static_cast<int>(Basis[0])+1);
  const int Ny(static_cast<int>(Basis[1])+1);
  const int Nz(static_cast<int>(Basis[2])+1);
  for(int k=-Nz;k<=Nz;k++)
    for(int j=-Ny;j<=Ny;j++)
      for(int i=-Nx;i<=Nx;i++)
        {
	  // Get cartesian position of the unit cell +
	  // centre atom offset
	  const Geometry::Vec3D UCOffset=
	    makeCartisian(Geometry::Vec3D(i,j,k))-RPosOff;
	  // Loop over atoms and get there position in the cell
	  std::vector<AtomPos>::const_iterator vc;
	  for(vc=Cell.begin();vc!=Cell.end();vc++)
	    {
	      Geometry::Vec3D AtomPt=
		makeCartisian(vc->getPos())+UCOffset;
	      if (inCube(AtomPt,Radius))
	        {
		  FullCell.push_back(*vc);
		  FullCell.back().setPos(AtomPt);
		}
	    }
	}
  // OUTPUT SORT:
  if (exafsAtom<Atoms.size())
    sort(FullCell.begin(),FullCell.end(),nearCentre());
  return;
}

int
CifStore::applyOcc()
  /*!
    Loop over all the Occ atoms and remove those that are 
    outside the occupancy level
    \return Number of atoms current
  */
{
  ELog::RegMethod RControl("CifStore","applyOcc");
  int eCnt(0);
  size_t index(0);
  while(index<FullCell.size())
    {
      const double Occ=FullCell[index].getOcc();
      if (Occ<1.0 && Occ<RNG.rand())
        {
          FullCell.erase(FullCell.begin()+static_cast<long int>(index));
          eCnt++;
	}
      else
        index++;
    }
  ELog::EM<<"Total deleted -- "<<eCnt<<ELog::endDiag;
  return eCnt;
}

void
CifStore::printSym() const
  /*!
    Debug function to write out sym
  */
{
  copy(Sym.begin(),Sym.end(),
       std::ostream_iterator<SymUnit>(std::cout,"\n"));
  return;
}

void
CifStore::writeFeff(const std::string& Fname) 
  /*!
    Write out the atoms information in a Feff format
    \param Fname :: Filename 
  */
{
  ELog::RegMethod RControl("CifStore","writeFeff");

  if (exafsAtom>=Z.size())
    {
      ELog::EM<<"Failed to find a central EXAFS atom "<<ELog::endErr;
      return;
    }
  std::ofstream FileOX;
  if (!Fname.empty())
    {
      FileOX.open(Fname.c_str());
      if (!FileOX.good())
        {
	  ELog::EM<<"Failed to open file"<<Fname<<ELog::endErr;
	  return;
	}
    }
  std::ostream& OX(Fname.empty() ? std::cout : FileOX);
  // The Potentuals part
  
  OX<<" POTENTUALS"<<std::endl;
  OX<<" *    ipot   Z  element"<<std::endl;
  // EXAFS atom first:
  OX<<"         0 "<<Z[exafsAtom]<<" "<<exafsType<<std::endl;

      

  ZTYPE::const_iterator mc;
  for(mc=Zmap.begin();mc!=Zmap.end();mc++)
    {
      OX<<"        "<<mc->second<<" "<<Z[mc->second]
	<<" "<<mc->first<<std::endl;
    }
  OX<<std::endl;
  // Just the Atoms part:
  sort(FullCell.begin(),FullCell.end(),nearCentre());
  OX<<"ATOMS          * this list contains "<<
    FullCell.size()<<" atoms"<<std::endl;
  OX<<" *   x          y          z      ipot  tag"
    <<"              distance"<<std::endl;
  std::vector<AtomPos>::const_iterator vc;
  
  NTYPE::const_iterator nmc;
  for(vc=FullCell.begin();vc!=FullCell.end();vc++)
    {
      nmc=Nmap.find(vc->getName());
      const Geometry::Vec3D& RP=vc->getPos();
      for(size_t i=0;i<3;i++)
	OX<<std::setw(10)<<RP[i]<<" ";
      OX<<std::setw(5)<< (nmc!=Nmap.end() ? nmc->second : -1); 
      OX<<" "<<std::setw(8)<<vc->getName()<<" ";
      OX<<vc->getPos().abs()<<std::endl;
    }
  
  return;
}

double
CifStore::calcLatticeFactor(const int h,const int k,const int l) const
/*!
    Crystal cacluation of S(Q)
    \param h :: h parameter
    \param k :: k parameter
    \param l :: l parameter
    \return \f$ F_hkl|K| \f$
  */
{
  ELog::RegMethod RegA("CifStore","calcLatticeFactor");
  //  const Geometry::Vec3D K=recipVec[0]*h+
  //    recipVec[1]*k+recipVec[2]*l;
  // 
  std::complex<double> F;
  const Geometry::Vec3D K=BVec(h,k,l);
  std::complex<double> I(0.0,-1.0);
  std::vector<AtomPos>::const_iterator vc;
  for(vc=Cell.begin();vc!=Cell.end();vc++)
    {
      const Geometry::Vec3D XP=makeCartisian(vc->getPos());
      //      const double factor=vc->getEdgeFactor();
      F+=exp(I*(K.dotProd(XP)));
    }
  return real(F*conj(F));
}

}   // NAMESPACE Crystal
