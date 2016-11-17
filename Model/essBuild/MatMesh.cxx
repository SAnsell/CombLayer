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
#include "Exception.h"
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
    /*!
      Convert array of integer material IDs into string
      \param vmat :: array of materials
     */
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
    /*!
      Calculate material fractions in the given cell
      \param center :: coordinates of the cell centre
      \param stepXYZ :: cell size
      \param N :: number of randm points to sample
     */
    ELog::RegMethod RControl("MatMesh","getMaterials");

    std::vector<int> vmat;
    double x, y, z, r;
    MonteCarlo::Object *ObjPtr(0);

    for (size_t i=0; i<N; i++) {
      r = (rand() % 100)/100.0 - 0.5; // -0.5 ... 0.5
      x = center[0] + r*stepXYZ[0];

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
    /*!
      Dump material mesh defined by XYZ boundaries into ASCII file
      \param vx :: array of boundaries along the x-axis
      \param vy :: array of boundaries along the y-axis
      \param vz :: array of boundaries along the z-axis
     */
    ELog::RegMethod RControl("MatMesh","Dump(vx, vy, vz, fname)");
    ELog::EM << "Dumping material mesh into\t" << fname << ELog::endBasic;

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
    /*!
      Dumps a mesh with materials in ASCII file 'fname'
      \param startPt :: Point with min coordinates
      \param endPt :: Point with max coordinates
      \param nX :: x-coordinate division
      \param nY :: y-coordinate division
      \param nZ :: z-coordinate division
      \param fname :: output file name
    */

    ELog::RegMethod RControl("MatMesh","Dump(start, end, nx, ny, nz, fname)");

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

  void MatMesh::Dump(const int &matmesh, const char *fname) const
  {
    /*!
      Dumps a mesh with materials in ASCII file 'fname'.
      The mesh is defined by the pre-defined index 'matmesh'.
    */
    ELog::RegMethod RControl("MatMesh","Dump(n, fname)");

    if (fname==0)
      fname = ("matmesh" + std::to_string(matmesh) + ".dat").c_str();

    if (matmesh==1)
      {
        Geometry::Vec3D ptStart(-70, -70, -80);
        Geometry::Vec3D ptEnd(70, 70, -20);
        Dump(ptStart, ptEnd, 10, 10, 10, fname);
      }
    else if (matmesh==6) // used for flux mesh in the old geometry 0e9675a (1 Jul 2016, /home/kbat/figs/butterfly/Flux-mesh)
      {
	std::vector<double> vx {-67, -56.2333, -45.4667, -34.7, -30.8633, -27.0267, -23.19, -19.3533, -15.5167, -11.68, -10.8545, -10.0291, -9.20364, -8.37818, -7.55273, -6.72727, -5.90182, -5.07636, -4.25091, -3.42545, -2.6, -2.3, -1.8, -1.5, -1.1, -0.8, 0.8, 1.1, 1.5, 1.8, 2.3, 2.6, 3.42545, 4.25091, 5.07636, 5.90182, 6.72727, 7.55273, 8.37818, 9.20364, 10.0291, 10.8545, 11.68, 15.5167, 19.3533, 23.19, 27.0267, 30.8633, 34.7, 45.4667, 56.2333, 67};
	std::vector<double> vy {-67, -56.2333, -45.4667, -34.7, -29.85, -25, -20.15, -15.3, -15, -14.5, -14.2, -12.8476, -11.4952, -10.1429, -8.79048, -7.4381, -6.08571, -4.73333, -3.38095, -2.02857, -0.67619, 0.67619, 2.02857, 3.38095, 4.73333, 6.08571, 7.4381, 8.79048, 10.1429, 11.4952, 12.8476, 14.2, 14.5, 15, 15.3, 20.15, 25, 29.85, 34.7, 45.4667, 56.2333, 67};
	std::vector<double> vz {-75, -68.22, -61.44, -54.66, -47.88, -41.1, -40.7, -40.1, -39.8, -37.81, -35.82, -33.83, -31.84, -29.85, -27.86, -25.87, -23.88, -21.89, -19.9, -19.6, -18.6, -18.3, -17.8, -17.4, -17, -16.7, -16.2, -15.9, -15.5, -15.2, -13.6, -13.3, -12.9, -12.6, -12.1, -11.8, -11.4, -11, -10.5, -10.3, -10.2, -7.3, -7, 7, 7.3, 10.3, 10.6, 11.1, 11.4, 11.8, 12.1, 13.7, 14, 14.4, 14.7, 15.2, 15.5, 15.8, 16.5, 16.8, 17.1, 19.1, 21.1, 23.1, 25.1, 27.1, 29.1, 31.1, 33.1, 35.1, 37.1, 37.4, 38, 38.4, 45.72, 53.04, 60.36, 67.68, 75};
	Dump(vx, vy, vz, fname);
      }
    else
      {
	throw ColErr::ExitAbort("matmesh==" + std::to_string(matmesh) + " not supported");
	//	throw ColErr::RangeError<int>(n, 0, 0, "not supported");
      }
  }
}
