#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "NameStack.h"
#include "RegMethod.h"

#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "FuncDataBase.h"
#include "Simulation.h"

#include "Triple.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "HeadRule.h"
#include "Surface.h"
#include "Object.h"
#include <sstream>
#include "FileReport.h"
#include "OutputLog.h"
#include "MatMesh.h"

namespace essSystem
{
  MatMesh::MatMesh(Simulation &System) :
    SimPtr(&System)
  {
    /*!
      \param SimPtr :: simulation
     */
    ELog::RegMethod RegA("MatMesh", "Constructor");
    return;
  }

  MatMesh::~MatMesh()
  {
    ELog::RegMethod RegA("MatMesh", "Destructor");
    return;
  }

  void MatMesh::Dump(const std::string &fname,
		     const double& xmin, const double& xmax,
		     const double& ymin, const double& ymax,
		     const double& zmin, const double& zmax,
		     const size_t& nX, const size_t& nY, const size_t& nZ) const
  /*!
    Dumps a mesh with materials in ASCII file 'fname'
    \param xmin,ymin,zmin :: Point with min coordinates
    \param xmax,ymax,zmax :: Point with max coordinates
    \param nX :: x-coordinate division
    \param nY :: y-coordinate division
    \param nZ :: z-coordinate division
    \param fname :: output file name
  */
    
  {
    ELog::RegMethod RegA("MatMesh", "Dump");

    Geometry::Vec3D startPt(xmin,ymin,zmin);
    Geometry::Vec3D endPt(xmax,ymax,zmax);

    Geometry::Vec3D Origin = startPt; // start corner (x,y,z=min)
    Geometry::Vec3D XYZ = endPt-Origin;
    Geometry::Vec3D aVec;
    MonteCarlo::Object *ObjPtr(0);
    
    Triple<long int> nPts = Triple<long int>(static_cast<long int>(nX), static_cast<long int>(nY), static_cast<long int>(nZ));
    double  stepXYZ[3];
    for (size_t i=0; i<3; i++)
      stepXYZ[i] = XYZ[i]/static_cast<double>(nPts[i]);
  

    std::ofstream fmesh;
    fmesh.open(fname);
    int mat = 0;

    for (size_t i=0; i<nX; i++) {
      aVec[0] = stepXYZ[0]*(static_cast<double>(i)+0.5);
      for (size_t j=0; j<nY; j++) {
	aVec[1] = stepXYZ[1]*(static_cast<double>(j)+0.5);
	for (size_t k=0; k<nZ; k++) {
	  aVec[2] = stepXYZ[2]*(static_cast<double>(k)+0.5);
	  const Geometry::Vec3D Pt = Origin+aVec;
	  ObjPtr = SimPtr->findCell(Pt, ObjPtr);
	  if (ObjPtr)
	    mat = ObjPtr->getMat();
	  else 
	    mat = -1;
	  fmesh << Pt[0] << " " << Pt[1] << " " << Pt[2] << "\t" << mat << std::endl;
	  
	}
      }
    }
    fmesh.close();
    
    
    return;
  }
}
