/****************************************************************************
* Copyright (C) 2009      Daniel Chappuis                                  *
****************************************************************************
* This file is part of ReactPhysics3D.                                     *
*                                                                          *
* ReactPhysics3D is free software: you can redistribute it and/or modify   *
* it under the terms of the GNU Lesser General Public License as published *
* by the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                      *
*                                                                          *
* ReactPhysics3D is distributed in the hope that it will be useful,        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU Lesser General Public License for more details.                      *
*                                                                          *
* You should have received a copy of the GNU Lesser General Public License *
* along with ReactPhysics3D. If not, see <http://www.gnu.org/licenses/>.   *
***************************************************************************/

 // Libraries
#include "Body.h"
#include "BroadBoundingVolume.h"
#include "NarrowBoundingVolume.h"

// We want to use the ReactPhysics3D namespace
using namespace reactphysics3d;

// Constructor
Body::Body(double mass) throw(std::invalid_argument)
     : mass(mass), broadBoundingVolume(0), narrowBoundingVolume(0)  {
    // Check if the mass is not larger than zero
    if (mass <= 0.0) {
        // We throw an exception
        throw std::invalid_argument("Exception in Body constructor : the mass has to be different larger than zero");
    }
}

// Destructor
Body::~Body() {
    if (broadBoundingVolume) {
        delete broadBoundingVolume;
    }
    if (narrowBoundingVolume) {
        delete narrowBoundingVolume;
    }
}

// Set the broad-phase bounding volume
void Body::setBroadBoundingVolume(BroadBoundingVolume* broadBoundingVolume) {
    assert(broadBoundingVolume);
    this->broadBoundingVolume = broadBoundingVolume;
    broadBoundingVolume->setBodyPointer(this);
}

// Set the narrow-phase bounding volume
void Body::setNarrowBoundingVolume(NarrowBoundingVolume* narrowBoundingVolume) {
    assert(narrowBoundingVolume);
    this->narrowBoundingVolume = narrowBoundingVolume;
    narrowBoundingVolume->setBodyPointer(this);
}
