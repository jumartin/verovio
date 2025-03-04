/////////////////////////////////////////////////////////////////////////////
// Name:        timestamp.cpp
// Author:      Laurent Pugin
// Created:     2016
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "timestamp.h"

//----------------------------------------------------------------------------

#include <cassert>

//----------------------------------------------------------------------------

namespace vrv {

//----------------------------------------------------------------------------
// TimestampAttr
//----------------------------------------------------------------------------

TimestampAttr::TimestampAttr() : LayerElement(TIMESTAMP_ATTR, "tstp-")
{
    this->Reset();
}

TimestampAttr::~TimestampAttr() {}

void TimestampAttr::Reset()
{
    LayerElement::Reset();

    m_actualDurPos = 0.0;
}

double TimestampAttr::GetTimestampAttrAlignmentDuration(int meterUnit) const
{
    return DUR_MAX / meterUnit * m_actualDurPos;
}

} // namespace vrv
