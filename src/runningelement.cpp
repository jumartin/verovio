/////////////////////////////////////////////////////////////////////////////
// Name:        runningelement.cpp
// Author:      Laurent Pugin
// Created:     2017
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "runningelement.h"

//----------------------------------------------------------------------------

#include <cassert>

//----------------------------------------------------------------------------

#include "doc.h"
#include "editorial.h"
#include "fig.h"
#include "functorparams.h"
#include "num.h"
#include "page.h"
#include "rend.h"
#include "staff.h"
#include "svg.h"
#include "text.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// RunningElement
//----------------------------------------------------------------------------

RunningElement::RunningElement()
    : Object(RUNNING_ELEMENT, "re"), ObjectListInterface(), AttHorizontalAlign(), AttTyped()
{
    this->RegisterAttClass(ATT_HORIZONTALALIGN);
    this->RegisterAttClass(ATT_TYPED);

    this->Reset();
}

RunningElement::RunningElement(ClassId classId)
    : Object(classId, "re"), ObjectListInterface(), AttHorizontalAlign(), AttTyped()
{
    this->RegisterAttClass(ATT_HORIZONTALALIGN);
    this->RegisterAttClass(ATT_TYPED);

    this->Reset();
}

RunningElement::RunningElement(ClassId classId, const std::string &classIdStr)
    : Object(classId, classIdStr), AttHorizontalAlign(), AttTyped()
{
    this->RegisterAttClass(ATT_HORIZONTALALIGN);
    this->RegisterAttClass(ATT_TYPED);

    this->Reset();
}

RunningElement::~RunningElement() {}

void RunningElement::Reset()
{
    Object::Reset();
    this->ResetHorizontalAlign();
    this->ResetTyped();

    m_isGenerated = false;

    m_drawingPage = NULL;
    m_drawingYRel = 0;

    int i;
    for (i = 0; i < 3; ++i) {
        m_drawingScalingPercent[i] = 100;
    }
}

bool RunningElement::IsSupportedChild(Object *child)
{
    if (child->IsTextElement()) {
        assert(dynamic_cast<TextElement *>(child));
    }
    else if (child->IsEditorialElement()) {
        assert(dynamic_cast<EditorialElement *>(child));
    }
    else {
        return false;
    }
    return true;
}

void RunningElement::FilterList(ListOfConstObjects &childList) const
{
    ListOfConstObjects::iterator iter = childList.begin();

    while (iter != childList.end()) {
        // remove nested rend elements
        if ((*iter)->Is(REND)) {
            if ((*iter)->GetFirstAncestor(REND)) {
                iter = childList.erase(iter);
                continue;
            }
        }
        // Also remove anything that is not a fig
        else if (!(*iter)->Is(FIG)) {
            iter = childList.erase(iter);
            continue;
        }
        ++iter;
    }
}

int RunningElement::GetDrawingX() const
{
    if (!m_drawingPage) return 0;

    /*
    if (this->GetHalign() == HORIZONTALALIGNMENT_left) {
        return 0;
    }
    else if (this->GetHalign() == HORIZONTALALIGNMENT_center) {
        return m_drawingPage->GetContentWidth() / 2;
    }
    else if (this->GetHalign() == HORIZONTALALIGNMENT_right) {
        return m_drawingPage->GetContentWidth();
    }
    */

    return 0;
}

int RunningElement::GetDrawingY() const
{
    m_cachedDrawingY = 0;
    return m_drawingYRel;
}

void RunningElement::SetDrawingYRel(int drawingYRel)
{
    this->ResetCachedDrawingY();
    m_drawingYRel = drawingYRel;
}

int RunningElement::GetWidth() const
{
    if (!m_drawingPage) return 0;

    Doc *doc = dynamic_cast<Doc *>(m_drawingPage->GetFirstAncestor(DOC));
    if (!doc) return 0;

    return (doc->m_drawingPageContentWidth);
    // return m_drawingPage->GetContentWidth();
}

void RunningElement::SetDrawingPage(Page *page)
{

    this->ResetList(this);

    this->ResetCachedDrawingX();
    m_drawingPage = page;

    if (page) {
        this->SetCurrentPageNum(page);
    }
}

int RunningElement::GetContentHeight() const
{
    int height = 0;
    int i;
    for (i = 0; i < 3; ++i) {
        height += this->GetRowHeight(i);
    }
    return height;
}

int RunningElement::GetRowHeight(int row) const
{
    assert((row >= 0) && (row < 3));

    int i;
    int height = 0;
    for (i = 0; i < 3; ++i) {
        height = std::max(height, this->GetCellHeight(row * 3 + i));
    }
    return height;
}

int RunningElement::GetColHeight(int col) const
{
    assert((col >= 0) && (col < 3));

    int i;
    int height = 0;
    for (i = 0; i < 3; ++i) {
        height += this->GetCellHeight(i * 3 + col);
    }
    return height;
}

int RunningElement::GetCellHeight(int cell) const
{
    assert((cell >= 0) && (cell < 9));

    int columnHeight = 0;
    const ArrayOfTextElements *textElements = &m_cells[cell];
    ArrayOfTextElements::const_iterator iter;
    for (iter = textElements->begin(); iter != textElements->end(); ++iter) {
        if ((*iter)->HasContentBB()) {
            columnHeight += (*iter)->GetContentY2() - (*iter)->GetContentY1();
        }
    }
    return columnHeight;
}

bool RunningElement::AdjustDrawingScaling(int width)
{
    int i, j;
    bool scale = false;
    // For each row
    for (i = 0; i < 3; ++i) {
        int rowWidth = 0;
        // For each column
        for (j = 0; j < 3; ++j) {
            ArrayOfTextElements *textElements = &m_cells[i * 3 + j];
            ArrayOfTextElements::iterator iter;
            int columnWidth = 0;
            // For each object
            for (iter = textElements->begin(); iter != textElements->end(); ++iter) {
                if ((*iter)->HasContentBB()) {
                    int iterWidth = (*iter)->GetContentX2() - (*iter)->GetContentX1();
                    columnWidth = std::max(columnWidth, iterWidth);
                }
            }
            rowWidth += columnWidth;
        }
        if (rowWidth && (rowWidth > width)) {
            m_drawingScalingPercent[i] = width * 100 / rowWidth;
            scale = true;
        }
    }
    return scale;
}

bool RunningElement::AdjustRunningElementYPos()
{
    int i, j;
    ArrayOfTextElements::iterator iter;

    // First adjust the content of each cell
    for (i = 0; i < 9; ++i) {
        int cumulatedYRel = 0;
        ArrayOfTextElements *textElements = &m_cells[i];
        // For each object
        for (iter = textElements->begin(); iter != textElements->end(); ++iter) {
            if (!(*iter)->HasContentBB()) {
                continue;
            }
            int yShift = (*iter)->GetContentY2();
            (*iter)->SetDrawingYRel(cumulatedYRel - yShift);
            cumulatedYRel += ((*iter)->GetContentY1() - (*iter)->GetContentY2());
        }
    }

    int rowYRel = 0;
    // For each row
    for (i = 0; i < 3; ++i) {
        int currentRowHeigt = this->GetRowHeight(i);
        // For each column
        for (j = 0; j < 3; ++j) {
            int cell = i * 3 + j;
            int colYShift = 0;
            // middle row - it needs to be middle-aligned so calculate the colYShift accordingly
            if (i == 1) {
                colYShift = (currentRowHeigt - this->GetCellHeight(cell)) / 2;
            }
            // bottom row - it needs to be bottom-aligned so calculate the colYShift accordingly
            else if (i == 2) {
                colYShift = (currentRowHeigt - this->GetCellHeight(cell));
            }

            ArrayOfTextElements *textElements = &m_cells[cell];
            ArrayOfTextElements::iterator iter;
            // For each object - adjust the yRel according to the rowYRel and the colYshift
            for (iter = textElements->begin(); iter != textElements->end(); ++iter) {
                if (!(*iter)->HasContentBB()) {
                    continue;
                }
                (*iter)->SetDrawingYRel((*iter)->GetDrawingYRel() + rowYRel - colYShift);
            }
        }
        rowYRel -= currentRowHeigt;
    }

    return true;
}

int RunningElement::GetAlignmentPos(data_HORIZONTALALIGNMENT h, data_VERTICALALIGNMENT v) const
{
    int pos = 0;
    switch (h) {
        case (HORIZONTALALIGNMENT_left): break;
        case (HORIZONTALALIGNMENT_center): pos += POSITION_CENTER; break;
        case (HORIZONTALALIGNMENT_right): pos += POSITION_RIGHT; break;
        default: pos += POSITION_LEFT; break;
    }
    switch (v) {
        case (VERTICALALIGNMENT_top): break;
        case (VERTICALALIGNMENT_middle): pos += POSITION_MIDDLE; break;
        case (VERTICALALIGNMENT_bottom): pos += POSITION_BOTTOM; break;
        default: pos += POSITION_MIDDLE; break;
    }
    return pos;
}

void RunningElement::SetCurrentPageNum(const Page *currentPage)
{
    assert(currentPage);

    int currentNum = currentPage->GetIdx() + 1;

    Num *num = dynamic_cast<Num *>(this->FindDescendantByType(NUM));
    if (!num || (num->GetLabel() != "page")) return;

    Text *text = dynamic_cast<Text *>(num->FindDescendantByType(TEXT));
    if (!text || (text->GetText() != U"#")) return;

    Text *currentText = num->GetCurrentText();
    assert(currentText);

    currentText->SetText(UTF8to32(StringFormat("%d", currentNum)));
}

void RunningElement::LoadFooter(const Doc *doc)
{
    Fig *fig = new Fig();
    Svg *svg = new Svg();

    const Resources &resources = doc->GetResources();
    const std::string footerPath = resources.GetPath() + "/footer.svg";
    pugi::xml_document footerDoc;
    footerDoc.load_file(footerPath.c_str());
    svg->Set(footerDoc.first_child());
    fig->AddChild(svg);
    fig->SetHalign(HORIZONTALALIGNMENT_center);
    fig->SetValign(VERTICALALIGNMENT_bottom);
    this->AddChild(fig);
}

void RunningElement::AddPageNum(data_HORIZONTALALIGNMENT halign, data_VERTICALALIGNMENT valign)
{
    Rend *rend = new Rend();
    data_FONTSIZE fs;
    fs.SetTerm(FONTSIZETERM_small);
    rend->SetFontsize(fs);
    rend->SetHalign(halign);
    rend->SetValign(valign);
    Text *dash1 = new Text();
    dash1->SetText(U"– ");
    Num *num = new Num();
    num->SetLabel("page");
    Text *text = new Text();
    text->SetText(U"#");
    Text *dash2 = new Text();
    dash2->SetText(U" –");

    num->AddChild(text);
    rend->AddChild(dash1);
    rend->AddChild(num);
    rend->AddChild(dash2);
    this->AddChild(rend);
}

//----------------------------------------------------------------------------
// Functor methods
//----------------------------------------------------------------------------

int RunningElement::PrepareDataInitialization(FunctorParams *)
{
    int i;
    for (i = 0; i < 9; ++i) {
        m_cells[i].clear();
    }
    for (i = 0; i < 3; ++i) {
        m_drawingScalingPercent[i] = 100;
    }

    const ListOfObjects &childList = this->GetList(this);
    for (ListOfObjects::const_iterator iter = childList.begin(); iter != childList.end(); ++iter) {
        int pos = 0;
        AreaPosInterface *interface = dynamic_cast<AreaPosInterface *>(*iter);
        assert(interface);
        pos = this->GetAlignmentPos(interface->GetHalign(), interface->GetValign());
        TextElement *text = vrv_cast<TextElement *>(*iter);
        assert(text);
        m_cells[pos].push_back(text);
    }

    return FUNCTOR_CONTINUE;
}

int RunningElement::Save(FunctorParams *functorParams)
{
    if (this->IsGenerated()) {
        return FUNCTOR_SIBLINGS;
    }
    else {
        return Object::Save(functorParams);
    }
}

int RunningElement::SaveEnd(FunctorParams *functorParams)
{
    if (this->IsGenerated()) {
        return FUNCTOR_SIBLINGS;
    }
    else {
        return Object::SaveEnd(functorParams);
    }
}

int RunningElement::AlignVertically(FunctorParams *functorParams)
{
    AlignVerticallyParams *params = vrv_params_cast<AlignVerticallyParams *>(functorParams);
    assert(params);

    params->m_pageWidth = this->GetWidth();

    return FUNCTOR_CONTINUE;
}

} // namespace vrv
