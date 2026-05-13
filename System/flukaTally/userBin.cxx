/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   flukaTally/userBin.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "writeSupport.h"
#include "stringCombine.h"
#include "Vec3D.h"

#include "flukaTally.h"
#include "userBin.h"

namespace flukaSystem
{


userBin::userBin(const int ID,const int outID) :
  userBin("mesh",ID,outID)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBin::userBin(const std::string& tallyName,
		   const int ID,const int outID) :
  flukaTally(tallyName,ID,outID),
  meshType(10),particle("208"),
  Pts({0,0,0}),
  hasLinkTransform(false)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBin::userBin(const userBin& A) :
  flukaTally(A),
  meshType(A.meshType),particle(A.particle),
  Pts(A.Pts),minCoord(A.minCoord),
  maxCoord(A.maxCoord),
  hasLinkTransform(A.hasLinkTransform),
  linkOrigin(A.linkOrigin),
  linkX(A.linkX),linkY(A.linkY),linkZ(A.linkZ)
  /*!
    Copy constructor
    \param A :: userBin to copy
  */
{}

userBin&
userBin::operator=(const userBin& A)
  /*!
    Assignment operator
    \param A :: userBin to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
      meshType=A.meshType;
      particle=A.particle;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
      hasLinkTransform=A.hasLinkTransform;
      linkOrigin=A.linkOrigin;
      linkX=A.linkX;
      linkY=A.linkY;
      linkZ=A.linkZ;
    }
  return *this;
}


userBin*
userBin::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userBin(*this);
}

userBin::~userBin()
  /*!
    Destructor
  */
{}

void
userBin::setParticle(const std::string& P)
  /*!
    Set the mesh particle
    \param P :: Partile
  */
{
  particle=P;
  return;
}

void
userBin::setDoseType(const std::string& P,
		     const std::string& D)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: particle to use
    \param D :: set dose type
  */
{
  ELog::RegMethod RegA("userBin","setDoseType");

  flukaTally::setDoseType(P,D);
  return;
}


void
userBin::setLinkTransform(const Geometry::Vec3D& origin,
			  const Geometry::Vec3D& X,
			  const Geometry::Vec3D& Y,
			  const Geometry::Vec3D& Z)
  /*!
    Store the link point world position and orientation axes so that
    write() can emit ROT-DEFIni / ROTPRBIN cards aligning the USRBIN
    mesh with the link point's local frame.
    \param origin :: Link point world position
    \param X :: Link point X axis (world frame)
    \param Y :: Link point Y axis (world frame)
    \param Z :: Link point Z axis (world frame)
  */
{
  hasLinkTransform = true;
  linkOrigin = origin;
  linkX = X;
  linkY = Y;
  linkZ = Z;
  return;
}

void
userBin::setIndex(const std::array<size_t,3>& IDX)
  /*!
    Sets the individual index for each x,y,z
    \param IDX :: array of three object
  */
{
  ELog::RegMethod RegA("userBin","setIndex");
  for(size_t i=0;i<3;i++)
    {
      if (!IDX[i])
	throw ColErr::IndexError<size_t>(IDX[i],i,"IDX[index] zero");
      Pts[i]=IDX[i];
    }

  return;
}


void
userBin::setCoordinates(const Geometry::Vec3D& A,
			const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  ELog::RegMethod RegA("userBin","setCoordinates");

  minCoord=A;
  maxCoord=B;
  // Add some checking here
  for(size_t i=0;i<3;i++)
    {
      if (std::abs(minCoord[i]-maxCoord[i])<Geometry::zeroTol)
	throw ColErr::NumericalAbort(StrFunc::makeString(minCoord)+" ::: "+
				     StrFunc::makeString(maxCoord)+
				     " Equal components");
      if (minCoord[i]>maxCoord[i])
	std::swap(minCoord[i],maxCoord[i]);
    }


  return;
}

void
userBin::writeAuxScore(std::ostream& OX) const
  /*!
    Write an auxScore card
    \param OX :: Ouput stream
  */
{
  if (!auxParticle.empty() && particle=="DOSE-EQ")
    {
      std::ostringstream cx;
      cx<<"AUXSCORE USRBIN "<<auxParticle<<" - "<<keyName
	<<" - - "<<doseType;
      StrFunc::writeFLUKA(cx.str(),OX);
    }
  return;
}


void
userBin::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region.
    When a link transform has been set via setLinkTransform(), three
    ROT-DEFIni cards and one ROTPRBIN card are emitted so that the
    USRBIN mesh follows the link point under geometry rotations.
    The USRBIN extents are expressed in the link point's local frame.
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  const int outputUnit(getOutUnit());

  if (hasLinkTransform)
    {
      // --- ROT-DEFIni: encode the transformation world → local frame ---
      //
      // The link point's rotation matrix R = [linkX | linkY | linkZ]
      // (columns are the local axes expressed in world coordinates).
      // We need the inverse: R^T = Rx(-gamma) * Ry(-beta) * Rz(-alpha),
      // using ZYX Euler angles extracted from R:
      //   R = Rz(alpha) * Ry(beta) * Rx(gamma)
      //
      // FLUKA ROT-DEFIni with Theta=0:
      //   j=3, Phi=alpha  →  Rz(-alpha)
      //   j=2, Phi=beta   →  Ry(-beta)
      //   j=1, Phi=gamma  →  Rx(-gamma)
      // Three cards with the same SDUM are composed left-to-right
      // (later card is outermost), giving: Rx(-gamma)*Ry(-beta)*Rz(-alpha)=R^T.
      // The translation (-linkOrigin) is carried by the first card.

      const double toDeg = 180.0 / M_PI;
      const double cosB = std::sqrt(linkX[0]*linkX[0] + linkX[1]*linkX[1]);

      double alpha, beta, gamma;
      beta = std::atan2(-linkX[2], cosB) * toDeg;

      if (cosB > 1e-10)
        {
          alpha = std::atan2(linkX[1], linkX[0]) * toDeg;
          gamma = std::atan2(linkY[2], linkZ[2]) * toDeg;
        }
      else
        {
          // Gimbal lock: beta = ±90°.  Set alpha = 0, derive gamma from Y.
          alpha = 0.0;
          if (linkX[2] < 0)   // beta = +90°
            gamma = std::atan2(linkY[0], linkY[1]) * toDeg;
          else                // beta = -90°
            gamma = std::atan2(-linkY[0], linkY[1]) * toDeg;
        }

      // Wrap all angles to [0°, 360°) for FLUKA conventions
      if (alpha < 0.0) alpha += 360.0;
      if (beta  < 0.0) beta  += 360.0;
      if (gamma < 0.0) gamma += 360.0;

      // Transformation index: use |outputUnit| as the ROT-DEFIni index i.
      const int rotIdx = std::abs(outputUnit);
      const std::string rotName = "R" + std::to_string(rotIdx);

      // Card 1: j=3 (z-rotation by alpha), carry the translation -linkOrigin.
      cx << "ROT-DEFI "
         << (3 + rotIdx * 1000) << " "
         << 0.0 << " " << alpha << " "
         << (-linkOrigin) << " "
         << rotName;
      StrFunc::writeFLUKA(cx.str(), OX);

      // Card 2: j=2 (y-rotation by beta), no translation.
      if (std::abs(beta)>Geometry::zeroTol) { // TODO: also check translation
	cx.str("");
	cx << "ROT-DEFI "
	   << (2 + rotIdx * 1000) << " "
	   << 0.0 << " " << beta << " "
	   << 0.0 << " " << 0.0 << " " << 0.0 << " "
	   << rotName;
	StrFunc::writeFLUKA(cx.str(), OX);
      }

      // Card 3: j=1 (x-rotation by gamma), no translation.
      if (std::abs(gamma)>Geometry::zeroTol) { // TODO: also check translation
	cx.str("");
	cx << "ROT-DEFI "
	   << (1 + rotIdx * 1000) << " "
	   << 0.0 << " " << gamma << " "
	   << 0.0 << " " << 0.0 << " " << 0.0 << " "
	   << rotName;
	StrFunc::writeFLUKA(cx.str(), OX);
      }

      // --- USRBIN: extents in the link point's local frame ---
      const Geometry::Vec3D localMin = minCoord - linkOrigin;
      const Geometry::Vec3D localMax = maxCoord - linkOrigin;

      cx.str("");
      cx << "USRBIN " << meshType << " " << particle << " "
         << outputUnit << " " << localMax << " " << keyName;
      StrFunc::writeFLUKA(cx.str(), OX);

      cx.str("");
      cx << "USRBIN " << localMin << " ";
      for (size_t i=0; i<3; i++)
        cx << Pts[i] << " ";
      cx << "  & ";
      StrFunc::writeFLUKA(cx.str(), OX);

      writeAuxScore(OX);

      // --- ROTPRBIN: associate the ROT-DEFIni with this USRBIN ---
      cx.str("");
      cx << "ROTPRBIN - " << rotName << " - " << keyName << " - - ";
      StrFunc::writeFLUKA(cx.str(), OX);
    }
  else
    {
      cx << "USRBIN " << meshType << " " << particle << " "
         << outputUnit << " " << maxCoord << " " << keyName;
      StrFunc::writeFLUKA(cx.str(), OX);

      cx.str("");
      cx << "USRBIN " << minCoord << " ";
      for (size_t i=0; i<3; i++)
        cx << Pts[i] << " ";
      cx << "  & ";
      StrFunc::writeFLUKA(cx.str(), OX);
      writeAuxScore(OX);
    }

  flukaTally::write(OX);
  return;
}

}  // NAMESPACE flukaSystem
