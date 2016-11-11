#include <iostream>
#include <vector>
#include <algorithm>
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

    System.populateCells();
    System.validateObjSurfMap();

    return;
  }

  MatMesh::~MatMesh()
  {
    ELog::RegMethod RegA("MatMesh", "Destructor");
    return;
  }

  std::string
  MatMesh::getMaterials(std::vector<int> vmat) const
  {
    ELog::RegMethod RControl("MatMesh","getMaterials(std::vector<int>)");
    std::ostringstream buffer;

    std::sort(vmat.begin(), vmat.end());
    const size_t N = vmat.size(); // total number of material entries
    std::vector<int> vmatu = vmat;
    std::vector<int>::iterator it = std::unique(vmatu.begin(), vmatu.end());
    vmatu.resize(static_cast<size_t>((std::distance(vmatu.begin(), it))));
    std::vector<int> vfrac(vmatu.size(), 0); // fractions

    for (size_t j=0; j<vmatu.size(); j++) {
      for (size_t i=0; i<N; i++)
	if (vmatu[j] == vmat[i])
	  vfrac[j]++;
      buffer << vfrac[j]*100.0/static_cast<double>(N) << "%" << vmatu[j] << "\t";
    }
    return buffer.str();
  }


  std::vector<int>
  MatMesh::getMaterials(const Geometry::Vec3D &center, double *stepXYZ, size_t N) const
  {
    ELog::RegMethod RControl("MatMesh","getMaterials");

    std::vector<int> vmat;
    double x, y, z, r;
    double xmin = 100000;
    double xmax = -10000;

    MonteCarlo::Object *ObjPtr(0);

    for (size_t i=0; i<N; i++) {
      r = (rand() % 100)/100.0 - 0.5; // -0.5 ... 0.5
      x = center[0] + r*stepXYZ[0];
      if (x<xmin) xmin=x;
      if (x>xmax) xmax=x;

      r = (rand() % 100)/100.0 - 0.5;
      y = center[1] + r*stepXYZ[1];

      r = (rand() % 100)/100.0 - 0.5;
      z = center[2] + r*stepXYZ[2];
      Geometry::Vec3D p(x, y, z);

      ObjPtr = SimPtr->findCell(p, ObjPtr);
      if (ObjPtr)
	vmat.push_back(ObjPtr->getMat());
      else
	ELog::EM << "Can't find cell at " << p << ELog::endCrit;
    }

    return vmat;
  }

  void MatMesh::Dump(const std::vector<double>& vx,
		     const std::vector<double>& vy,
		     const std::vector<double>& vz,
		     const char *fname) const
  {
    ELog::RegMethod RControl("MatMesh","Dump(vx, vy, vz, fname)");

    const size_t nX = vx.size()-1;
    const size_t nY = vy.size()-1;
    const size_t nZ = vz.size()-1;

    MonteCarlo::Object *ObjPtr(0);

    std::ofstream fmesh;
    fmesh.open(fname);
    int mat = 0;
    std::vector<int> vmat;
    double xmin, ymin, zmin, xmax, ymax, zmax;
    double  stepXYZ[3];

    for (size_t k=0; k<nZ; k++) {
      std::cout << k << "/" << nZ << std::endl;
      for (size_t j=0; j<nY; j++) {
	for (size_t i=0; i<nX; i++) {

	  const Geometry::Vec3D Pt((vx[i+1]+vx[i])/2.0, (vy[j+1]+vy[j])/2.0, (vz[k+1]+vz[k])/2.0);
	  ObjPtr = SimPtr->findCell(Pt, ObjPtr);
	  if (ObjPtr)
	    mat = ObjPtr->getMat();
	  else
	    mat = -1;
	  xmin = vx[i];	  xmax = vx[i+1];
	  ymin = vy[j];	  ymax = vy[j+1];
	  zmin = vz[k];	  zmax = vz[k+1];
	  stepXYZ[0] = xmax-xmin;
	  stepXYZ[1] = ymax-ymin;
	  stepXYZ[2] = zmax-zmin;
	  vmat = getMaterials(Pt, stepXYZ, 1000);
	  fmesh << xmin << " " << xmax << " " << ymin << " " << ymax << " " << zmin << " " << zmax << "\t";
	  fmesh << "\t" << getMaterials(vmat) << std::endl;;
	}
      }
    }
    fmesh.close();
  }

  void MatMesh::Dump(const Geometry::Vec3D &startPt, const Geometry::Vec3D &endPt,
		     const size_t nX, const size_t nY, const size_t nZ,
		     const char *fname) const
  {
    /*
      Dumps a mesh with materials in ASCII file 'fname'
      \param startPt :: Point with min coordinates
      \param endPt :: Point with max coordinates
      \param nX :: x-coordinate division
      \param nY :: y-coordinate division
      \param nZ :: z-coordinate division
      \param fname :: output file name
    */

    ELog::RegMethod RControl("MatMesh","Dump(start, end, nx, ny, nz)");

    Geometry::Vec3D Origin = startPt; // start corner (x,y,z=min)
    Geometry::Vec3D XYZ = endPt-Origin;
    
    Triple<long int> nPts = Triple<long int>(static_cast<long int>(nX), static_cast<long int>(nY), static_cast<long int>(nZ));
    double  stepXYZ[3];
    for (size_t i=0; i<3; i++)
      stepXYZ[i] = XYZ[i]/static_cast<double>(nPts[i]);

    std::vector<double> vx, vy, vz;

    for (size_t k=0; k<=nZ; k++)
      vz.push_back(startPt[2] + stepXYZ[2]*k);
    for (size_t j=0; j<=nY; j++)
      vy.push_back(startPt[1] + stepXYZ[1]*j);
    for (size_t i=0; i<=nX; i++)
      vx.push_back(startPt[0] + stepXYZ[0]*i);

    Dump(vx, vy, vz, fname);
    
    return;
  }

}
