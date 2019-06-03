/////////////////////////////////////////////////////////////////////////////
// Name:        tuning.cpp
// Author:      Laurent Pugin
// Created:     2019
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "tuning.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

namespace vrv {

//----------------------------------------------------------------------------
// Tuning
//----------------------------------------------------------------------------

Tuning::Tuning() : Object("tuning-"), AttCourseLog()
{
    RegisterAttClass(ATT_COURSELOG);

    Reset();
}

Tuning::~Tuning()
{
}

void Tuning::Reset()
{
    Object::Reset();
    ResetCourseLog();
}

void Tuning::AddChild(Object *child)
{
    child->SetParent(this);
    m_children.push_back(child);
    Modify();
}

} // namespace vrv
