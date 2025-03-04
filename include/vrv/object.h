/////////////////////////////////////////////////////////////////////////////
// Name:        object.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_OBJECT_H__
#define __VRV_OBJECT_H__

#include <cstdlib>
#include <functional>
#include <iterator>
#include <map>
#include <random>
#include <string>

//----------------------------------------------------------------------------

#include "attclasses.h"
#include "boundingbox.h"
#include "vrvdef.h"

namespace vrv {

class AltSymInterface;
class AreaPosInterface;
class Doc;
class DurationInterface;
class EditorialElement;
class Output;
class Filters;
class Functor;
class FunctorParams;
class LinkingInterface;
class FacsimileInterface;
class PitchInterface;
class PositionInterface;
class Resources;
class SaveParams;
class ScoreDefInterface;
class StemmedDrawingInterface;
class TextDirInterface;
class TimePointInterface;
class TimeSpanningInterface;
class Zone;

#define UNLIMITED_DEPTH -10000
#define FORWARD true
#define BACKWARD false

//----------------------------------------------------------------------------
// Object
//----------------------------------------------------------------------------

/**
 * This class represents a basic object
 */
class Object : public BoundingBox {
public:
    /**
     * @name Constructors, destructors, and other standard methods
     * Reset method resets all attribute classes
     */
    ///@{
    Object();
    Object(ClassId classId);
    Object(ClassId classId, const std::string &classIdStr);
    virtual ~Object();
    ClassId GetClassId() const final { return m_classId; }
    virtual std::string GetClassName() const { return "[MISSING]"; }
    ///@}

    /**
     * Make an object a reference object that do not own children.
     * This cannot be un-done and has to be set before any child is added.
     */
    ///@{
    void SetAsReferenceObject();
    bool IsReferenceObject() const { return m_isReferenceObject; }
    ///@}

    /**
     * Wrapper for checking if an element is a floating object (system elements and control elements)
     */
    bool IsFloatingObject() const { return (this->IsSystemElement() || this->IsControlElement()); }

    /**
     * Wrapper for checking if an element has a milestone start interface and also if is set as a milestone element
     */
    ///@{
    bool IsMilestoneElement();
    Object *GetMilestoneEnd();
    ///@}

    /**
     * @name Methods for checking if an object is part of a group of classId's.
     * For example, all LayerElement child class classId's are between LAYER_ELEMENT and LAYER_ELEMENT_max.
     * See classId enum.
     */
    ///@{
    bool IsControlElement() const { return ((m_classId > CONTROL_ELEMENT) && (m_classId < CONTROL_ELEMENT_max)); }
    bool IsEditorialElement() const { return ((m_classId > EDITORIAL_ELEMENT) && (m_classId < EDITORIAL_ELEMENT_max)); }
    bool IsLayerElement() const { return ((m_classId > LAYER_ELEMENT) && (m_classId < LAYER_ELEMENT_max)); }
    bool IsPageElement() const { return ((m_classId > PAGE_ELEMENT) && (m_classId < PAGE_ELEMENT_max)); }
    bool IsRunningElement() const { return ((m_classId > RUNNING_ELEMENT) && (m_classId < RUNNING_ELEMENT_max)); }
    bool IsScoreDefElement() const { return ((m_classId > SCOREDEF_ELEMENT) && (m_classId < SCOREDEF_ELEMENT_max)); }
    bool IsSystemElement() const { return ((m_classId > SYSTEM_ELEMENT) && (m_classId < SYSTEM_ELEMENT_max)); }
    bool IsTextElement() const { return ((m_classId > TEXT_ELEMENT) && (m_classId < TEXT_ELEMENT_max)); }
    ///@}

    /**
     * @name Methods for registering a MEI att class and for registering interfaces regrouping MEI att classes.
     */
    ///@{
    void RegisterAttClass(AttClassId attClassId) { m_attClasses.push_back(attClassId); }
    bool HasAttClass(AttClassId attClassId) const
    {
        return std::find(m_attClasses.begin(), m_attClasses.end(), attClassId) != m_attClasses.end();
    }
    void RegisterInterface(std::vector<AttClassId> *attClasses, InterfaceId interfaceId);
    bool HasInterface(InterfaceId interfaceId) const
    {
        return std::find(m_interfaces.begin(), m_interfaces.end(), interfaceId) != m_interfaces.end();
    }
    ///@}

    /**
     * @name Getter to interfaces
     */
    ///@{
    virtual AltSymInterface *GetAltSymInterface() { return NULL; }
    virtual const AltSymInterface *GetAltSymInterface() const { return NULL; }
    virtual AreaPosInterface *GetAreaPosInterface() { return NULL; }
    virtual const AreaPosInterface *GetAreaPosInterface() const { return NULL; }
    virtual BeamDrawingInterface *GetBeamDrawingInterface() { return NULL; }
    virtual const BeamDrawingInterface *GetBeamDrawingInterface() const { return NULL; }
    virtual DurationInterface *GetDurationInterface() { return NULL; }
    virtual const DurationInterface *GetDurationInterface() const { return NULL; }
    virtual LinkingInterface *GetLinkingInterface() { return NULL; }
    virtual const LinkingInterface *GetLinkingInterface() const { return NULL; }
    virtual FacsimileInterface *GetFacsimileInterface() { return NULL; }
    virtual const FacsimileInterface *GetFacsimileInterface() const { return NULL; }
    virtual PitchInterface *GetPitchInterface() { return NULL; }
    virtual const PitchInterface *GetPitchInterface() const { return NULL; }
    virtual PlistInterface *GetPlistInterface() { return NULL; }
    virtual const PlistInterface *GetPlistInterface() const { return NULL; }
    virtual PositionInterface *GetPositionInterface() { return NULL; }
    virtual const PositionInterface *GetPositionInterface() const { return NULL; }
    virtual ScoreDefInterface *GetScoreDefInterface() { return NULL; }
    virtual const ScoreDefInterface *GetScoreDefInterface() const { return NULL; }
    virtual StemmedDrawingInterface *GetStemmedDrawingInterface() { return NULL; }
    virtual const StemmedDrawingInterface *GetStemmedDrawingInterface() const { return NULL; }
    virtual TextDirInterface *GetTextDirInterface() { return NULL; }
    virtual const TextDirInterface *GetTextDirInterface() const { return NULL; }
    virtual TimePointInterface *GetTimePointInterface() { return NULL; }
    virtual const TimePointInterface *GetTimePointInterface() const { return NULL; }
    virtual TimeSpanningInterface *GetTimeSpanningInterface() { return NULL; }
    virtual const TimeSpanningInterface *GetTimeSpanningInterface() const { return NULL; }
    ///@}

    /**
     * Resource access from the document
     */
    const Resources *GetDocResources() const;

    /**
     * Reset the object, that is 1) removing all childs and 2) resetting all attributes.
     * The method is virtual, so _always_ call the parent in the method overriding it.
     */
    virtual void Reset();

    /**
     * Copy constructor that also copy the children.
     * The children are copied using the Object::Clone virtual method that
     * needs to be overridden in the child class - otherwise, it will crash.
     * Because this will create a problem if we don't check this (the parents will
     * one the same child...)
     * ID: the id is copied, it needs to be reset later if this is not wished
     */
    Object(const Object &object);

    /**
     * Copy assignement - see copy constructor.
     */
    Object &operator=(const Object &object);

    /**
     * Move all the children of the object passed as parameter to this one.
     * Objects must be of the same type.
     * After this operation, the object passed as parameter has no child anymore.
     * If idx is provided, move the children to the idx position in the object children.
     * Only moving to the same type is allow unless allowTypeChange is true.
     */
    void MoveChildrenFrom(Object *sourceParent, int idx = -1, bool allowTypeChange = false);

    /**
     * Replace the currentChild with the replacingChild.
     * The currentChild is not deleted by the methods.
     */
    void ReplaceChild(Object *currentChild, Object *replacingChild);

    /**
     * @name Insert an object before or after a given child
     */
    ///@{
    void InsertBefore(Object *child, Object *newChild);
    void InsertAfter(Object *child, Object *newChild);
    ///@}

    /**
     * Sort children by a function that takes two arguments and
     * returns true if the first argument is less than the second.
     * If the order of children changes, this returns true.
     */
    typedef bool (*binaryComp)(Object *, Object *);
    void SortChildren(binaryComp comp);

    /**
     * Move an object to another parent.
     * The object is relinquished from its current parent - see Object::Relinquish
     */
    void MoveItselfTo(Object *targetParent);

    /**
     * Method call for copying child classes.
     * The method has to be overridden.
     */
    virtual Object *Clone() const;

    /**
     * Indicate whereas children have to be copied in copy / assignment constructors.
     * This is true by default but can be overriden (e.g., for Staff, Layer)
     */
    virtual bool CopyChildren() const { return true; }

    /**
     * Reset pointers after a copy and assignment constructor call.
     * This methods has to be called expicitly when overriden because it is not called from the constructors.
     * Do not forget to call base-class equivalent whenever applicable (e.g., with more than one hierarchy level).
     */
    virtual void CloneReset();

    const std::string &GetID() const { return m_id; }
    void SetID(const std::string &id) { m_id = id; }
    void SwapID(Object *other);
    void ResetID();

    /**
     * Methods for setting / getting comments
     */
    std::string GetComment() const { return m_comment; }
    void SetComment(std::string comment) { m_comment = comment; }
    bool HasComment() { return !m_comment.empty(); }
    std::string GetClosingComment() const { return m_closingComment; }
    void SetClosingComment(std::string endComment) { m_closingComment = endComment; }
    bool HasClosingComment() { return !m_closingComment.empty(); }

    /**
     * @name Children count, with or without a ClassId.
     * Used for classes with several types of children.
     * The methods count at the first level.
     */
    ///@{
    int GetChildCount() const { return (int)m_children.size(); }
    int GetChildCount(const ClassId classId) const;
    int GetChildCount(const ClassId classId, int depth) const;
    int GetDescendantCount(const ClassId classId) const;
    ///@}

    /**
     * Child access (generic)
     */
    ///@{
    Object *GetChild(int idx);
    const Object *GetChild(int idx) const;
    Object *GetChild(int idx, const ClassId classId);
    const Object *GetChild(int idx, const ClassId classId) const;
    ///@}

    /**
     * Return the children as const reference or copy
     */
    ///@{
    ArrayOfConstObjects GetChildren() const;
    const ArrayOfObjects &GetChildren() { return m_children; }
    ///@}

    /**
     * Return a reference to the children that allows modification.
     * This method should be all only in AddChild overrides methods
     */
    ArrayOfObjects &GetChildrenForModification() { return m_children; }

    /**
     * Fill an array of pairs with all attributes and their values.
     * Return the number of attributes found.
     */
    int GetAttributes(ArrayOfStrAttr *attributes) const;

    /**
     * Check if an Object has an attribute with the specified value
     */
    bool HasAttribute(std::string attribute, std::string value) const;

    /**
     * @name Iterator methods for accessing children.
     * GetFirst returns the first element child of the specified type.
     * Its position and the specified type are stored and used of accessing next elements
     * The methods returns NULL when no child is found or when the end is reached.
     * Always call GetFirst before calling GetNext() or call GetNext(child)
     */
    ///@{
    Object *GetFirst(const ClassId classId = UNSPECIFIED);
    const Object *GetFirst(const ClassId classId = UNSPECIFIED) const;
    Object *GetNext();
    const Object *GetNext() const;
    ///@}

    /**
     * @name Retrieving next or previous sibling of a certain type.
     * Returns NULL is not found
     */
    ///@{
    Object *GetNext(const Object *child, const ClassId classId = UNSPECIFIED);
    const Object *GetNext(const Object *child, const ClassId classId = UNSPECIFIED) const;
    Object *GetPrevious(const Object *child, const ClassId classId = UNSPECIFIED);
    const Object *GetPrevious(const Object *child, const ClassId classId = UNSPECIFIED) const;
    ///@}

    /**
     * Return the last child of the object (if any, NULL otherwise)
     */
    ///@{
    Object *GetLast(const ClassId classId = UNSPECIFIED);
    const Object *GetLast(const ClassId classId = UNSPECIFIED) const;
    ///@}

    /**
     * Get the parent of the Object
     */
    ///@{
    Object *GetParent() { return m_parent; }
    const Object *GetParent() const { return m_parent; }
    ///@}

    /**
     * Set the parent of the Object.
     * The current parent is expected to be NULL.
     */
    void SetParent(Object *parent);

    /**
     * Reset the parent of the Object.
     * The current parent is not expected to be NULL.
     */
    void ResetParent() { m_parent = NULL; }

    /**
     * Base method for checking if a child can be added.
     * The method has to be overridden.
     */
    virtual bool IsSupportedChild(Object *object);

    /**
     * Base method for adding children.
     * The method can be overridden.
     */
    virtual void AddChild(Object *object);

    /**
     * Return the index position of the object in its parent (-1 if not found)
     */
    int GetIdx() const;

    /**
     * @name Get the X and Y drawing position
     */
    ///@{
    int GetDrawingX() const override;
    int GetDrawingY() const override;
    ///@}

    /**
     * @name Reset the cached values of the drawingX and Y values.
     * Reset all children recursively
     */
    ///@{
    void ResetCachedDrawingX() const override;
    void ResetCachedDrawingY() const override;
    ///@}

    /**
     * Look for the Object in the children and return its position (-1 if not found)
     */
    int GetChildIndex(const Object *child) const;

    /**
     * Look for all Objects of a class and return its position (-1 if not found)
     */
    int GetDescendantIndex(const Object *child, const ClassId classId, int depth);

    /**
     * Insert an element at the idx position.
     */
    void InsertChild(Object *element, int idx);

    /**
     * Detach the child at the idx position (NULL if not found)
     * The parent pointer is set to NULL.
     */
    Object *DetachChild(int idx);

    /**
     * Return true if the object has the child Object as descendant (reference of direct).
     * Processes in depth-first.
     */
    bool HasDescendant(const Object *child, int deepness = UNLIMITED_DEPTH) const;

    /**
     * Look for a descendant with the specified id (returns NULL if not found)
     * This method is a wrapper for the Object::FindByID functor.
     */
    ///@{
    Object *FindDescendantByID(const std::string &id, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD);
    const Object *FindDescendantByID(
        const std::string &id, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD) const;
    ///@}

    /**
     * Look for a descendant with the specified type (returns NULL if not found)
     * This method is a wrapper for the Object::FindByType functor.
     */
    ///@{
    Object *FindDescendantByType(ClassId classId, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD);
    const Object *FindDescendantByType(ClassId classId, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD) const;
    ///@}

    /**
     * Return the first element matching the Comparison functor
     * Deepness allow to limit the depth search (EditorialElements are not count)
     */
    ///@{
    Object *FindDescendantByComparison(
        Comparison *comparison, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD);
    const Object *FindDescendantByComparison(
        Comparison *comparison, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD) const;
    ///@}

    /**
     * Return the element matching the extreme value with an Comparison functor
     * Deepness allow to limit the depth search (EditorialElements are not count)
     */
    ///@{
    Object *FindDescendantExtremeByComparison(
        Comparison *comparison, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD);
    const Object *FindDescendantExtremeByComparison(
        Comparison *comparison, int deepness = UNLIMITED_DEPTH, bool direction = FORWARD) const;
    ///@}

    /**
     * Return all the objects with specified type
     */
    ///@{
    ListOfObjects FindAllDescendantsByType(
        ClassId classId, bool continueDepthSearchForMatches = true, int deepness = UNLIMITED_DEPTH);
    ListOfConstObjects FindAllDescendantsByType(
        ClassId classId, bool continueDepthSearchForMatches = true, int deepness = UNLIMITED_DEPTH) const;
    ///@}

    /**
     * Return all the objects matching the Comparison functor
     * Deepness allow to limit the depth search (EditorialElements are not count)
     */
    ///@{
    void FindAllDescendantsByComparison(ListOfObjects *objects, Comparison *comparison, int deepness = UNLIMITED_DEPTH,
        bool direction = FORWARD, bool clear = true);
    void FindAllDescendantsByComparison(ListOfConstObjects *objects, Comparison *comparison,
        int deepness = UNLIMITED_DEPTH, bool direction = FORWARD, bool clear = true) const;
    ///@}

    /**
     * Return all the objects matching the Comparison functor and being between start and end in the tree.
     * The start and end objects are included in the result set.
     */
    ///@{
    void FindAllDescendantsBetween(ListOfObjects *objects, Comparison *comparison, const Object *start,
        const Object *end, bool clear = true, int depth = UNLIMITED_DEPTH);
    void FindAllDescendantsBetween(ListOfConstObjects *objects, Comparison *comparison, const Object *start,
        const Object *end, bool clear = true, int depth = UNLIMITED_DEPTH) const;
    ///@}

    /**
     * Give up ownership of the child at the idx position (NULL if not found)
     * This is a method to be used only in the very particular case where the child
     * object cannot be detached straight away. It is typically the case
     * when this has to be done within an iterator. The parent of the object
     * will be set to NULL but the object will not be removed. If the parent
     * is not destroyed after that, you should expect problems unless Object::ClearRelinquishedChildren is called
     * In other words: do not use unless you are absolutely sure what you are doing
     */
    Object *Relinquish(int idx);

    /**
     * Removes all the children that were previously relinquished.
     * This has to be used when children are moved but then the parent is not deleted.
     */
    void ClearRelinquishedChildren();

    /**
     * Clear the children vector and delete all the objects.
     */
    void ClearChildren();

    /**
     * Remove and delete the child.
     * Return false if the child could not be found. In that case it will not be deleted.
     */
    bool DeleteChild(Object *child);

    /**
     * Delete the children that match the comparison.
     * Return the number of children deleted. Also mark the object as modified for invalidating the list.
     */
    int DeleteChildrenByComparison(Comparison *comparison);

    /**
     * Returns all ancestors
     */
    ///@{
    ListOfObjects GetAncestors();
    ListOfConstObjects GetAncestors() const;
    ///@}

    /**
     * Return the first ancestor of the specified type.
     * The maxSteps parameter limits the search to a certain number of level if not -1.
     */
    ///@{
    Object *GetFirstAncestor(const ClassId classId, int maxSteps = -1);
    const Object *GetFirstAncestor(const ClassId classId, int maxSteps = -1) const;
    ///@}

    ///@{
    Object *GetFirstAncestorInRange(const ClassId classIdMin, const ClassId classIdMax, int maxDepth = -1);
    const Object *GetFirstAncestorInRange(const ClassId classIdMin, const ClassId classIdMax, int maxDepth = -1) const;
    ///@}

    /**
     * Return the last ancestor that is NOT of the specified type.
     * The maxSteps parameter limits the search to a certain number of level if not -1.
     */
    ///@{
    Object *GetLastAncestorNot(const ClassId classId, int maxSteps = -1);
    const Object *GetLastAncestorNot(const ClassId classId, int maxSteps = -1) const;
    ///@}

    /**
     * Return the first child that is NOT of the specified type.
     */
    ///@{
    Object *GetFirstChildNot(const ClassId classId);
    const Object *GetFirstChildNot(const ClassId classId) const;
    ///@}

    /**
     * Fill the list of all the children LayerElement.
     * This is used for navigating in a Layer (See Layer::GetPrevious and Layer::GetNext).
     */
    void FillFlatList(ListOfConstObjects &list) const;

    /**
     * Check if the content was modified or not
     */
    bool IsModified() const { return m_isModified; }

    /**
     * Mark the object and its parent (if any) as modified
     */
    void Modify(bool modified = true) const;

    /**
     * @name Setter and getter of the attribute flag
     */
    ///@{
    bool IsAttribute() const { return m_isAttribute; }
    void IsAttribute(bool isAttribute) { m_isAttribute = isAttribute; }
    ///@}

    /**
     * @name Setter and getter of the expansion flag
     */
    ///@{
    bool IsExpansion() const { return m_isExpansion; }
    void IsExpansion(bool isExpansion) { m_isExpansion = isExpansion; }
    ///@}

    /**
     * Return true if the object contains any editorial content
     */
    bool HasEditorialContent();

    /**
     * Return true if the object contains anything that is not editorial content
     */
    bool HasNonEditorialContent();

    /**
     * Saves the object (and its children) using the specified output stream.
     * Creates functors that will parse the tree.
     */
    int SaveObject(SaveParams &saveParams);

    /**
     * Sort the child elements using std::stable_sort
     */
    template <class Compare> void StableSort(Compare comp)
    {
        std::stable_sort(m_children.begin(), m_children.end(), comp);
    }

    virtual void ReorderByXPos();

    Object *FindNextChild(Comparison *comp, Object *start);

    Object *FindPreviousChild(Comparison *comp, Object *start);
    /**
     * Main method that processes functors.
     * For each object, it will call the functor.
     * Depending on the code returned by the functor, it will also process it for all children.
     * The Filters class parameter makes is possible to process only objects of a
     * type that matches the attribute value given in the Comparison object.
     * This is the generic way for parsing the tree, e.g., for extracting one single staff or layer.
     * Deepness specifies how many child levels should be processed. UNLIMITED_DEPTH means no
     * limit (EditorialElement objects do not count).
     * skipFirst does not call the functor or endFunctor on the first (calling) level
     */
    ///@{
    void Process(Functor *functor, FunctorParams *functorParams, Functor *endFunctor = NULL, Filters *filters = NULL,
        int deepness = UNLIMITED_DEPTH, bool direction = FORWARD, bool skipFirst = false);
    void Process(Functor *functor, FunctorParams *functorParams, Functor *endFunctor = NULL, Filters *filters = NULL,
        int deepness = UNLIMITED_DEPTH, bool direction = FORWARD, bool skipFirst = false) const;
    ///@}

    //----------------//
    // Static methods //
    //----------------//

    static void SeedID(unsigned int seed = 0);

    static std::string GenerateRandID();

    static bool sortByUlx(Object *a, Object *b);

    /**
     * Return true if left appears before right in preorder traversal
     */
    static bool IsPreOrdered(const Object *left, const Object *right);

    //----------//
    // Functors //
    //----------//

    /**
     * Add each LayerElements and its children to a flat list
     */
    virtual int AddLayerElementToFlatList(FunctorParams *functorParams) const;

    /**
     * Builds a tree of ints (IntTree) with the staff/layer/verse numbers and for staff/layer to be then processed.
     */
    virtual int InitProcessingLists(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * @name Functors for finding objects
     */
    ///@{

    /**
     * Find a Object with a specified id.
     */
    virtual int FindByID(FunctorParams *functorParams) const;

    /**
     * Find a Object with a Comparison functor.
     */
    virtual int FindByComparison(FunctorParams *functorParams) const;

    /**
     * Find the next child matching the Comparison object passed in the parameters
     */
    virtual int FindNextChildByComparison(FunctorParams *);

    /**
     * Find the previous child matching the Comparison object passed in the parameters
     */
    virtual int FindPreviousChildByComparison(FunctorParams *);

    /**
     * Find a Object with the extreme value with a Comparison functor .
     */
    virtual int FindExtremeByComparison(FunctorParams *functorParams) const;

    /**
     * Find a all Object with an Comparison functor.
     */
    virtual int FindAllByComparison(FunctorParams *functorParams);

    /**
     * Const Functor for Object::FindAllByComparison
     */
    virtual int FindAllConstByComparison(FunctorParams *functorParams) const;

    /**
     * Find a all Object between a start and end Object and with an Comparison functor.
     */
    virtual int FindAllBetween(FunctorParams *functorParams);

    /**
     * Const Functor for Object::FindAllBetween
     */
    virtual int FindAllConstBetween(FunctorParams *functorParams) const;

    /**
     * Find a all Object to which another object points to in the data.
     */
    virtual int FindAllReferencedObjects(FunctorParams *functorParams);

    /**
     * Look if the time / duration passed as parameter overlap with a space in the alignment references.
     */
    virtual int LayerCountInTimeSpan(FunctorParams *) const { return FUNCTOR_CONTINUE; }

    /**
     * Look for all the layer elements that overlap with the time / duration within certain layer passed as parameter.
     */
    virtual int LayerElementsInTimeSpan(FunctorParams *functorParams) const { return FUNCTOR_CONTINUE; }

    /**
     * Retrieve the layer elements spanned by two points
     */
    virtual int FindSpannedLayerElements(FunctorParams *) const { return FUNCTOR_CONTINUE; }

    /**
     * Look for element by ID in StaffDef elements (Clef, KeySig, etc.) of all layers within
     */
    virtual int FindElementInLayerStaffDefsByID(FunctorParams *) const { return FUNCTOR_CONTINUE; }

    /**
     * Retrieve the minimum left and maximum right for an alignment.
     * Used in GraceAligner::GetGraceGroupLeft and GraceAligner::GetGraceGroupRight.
     */
    virtual int GetAlignmentLeftRight(FunctorParams *functorParams) const;

    /**
     * Go through all layer elements of the layer and return next/previous element relative to the specified
     * layer element.
     * It will search recursively through children elements until note, chord or ftrem is found.
     * It can be used to look in neighboring layers for the similar search, but only first element will be checked.
     */
    virtual int GetRelativeLayerElement(FunctorParams *) const { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors for loading and saving the document
     */
    ///@{

    /**
     * Convert top-level all container (section, endings) and editorial elements to milestone elements.
     */
    virtual int ConvertToPageBased(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::ConvertToPageBased
     */
    virtual int ConvertToPageBasedEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Convert mensural MEI into cast-off (measure) segments looking at the barLine objects.
     * Segment positions occur where a barLine is set on all staves.
     */
    virtual int ConvertToCastOffMensural(FunctorParams *functorParams);

    /**
     * Convert cast-off (measure) mensural segments MEI into mensural.
     */
    virtual int ConvertToUnCastOffMensural(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Convert analytical markup (\@fermata, \@tie) to elements.
     * See Doc::ConvertMarkupAnalyticalDoc
     */
    virtual int ConvertMarkupAnalytical(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::ConvertMarkupAnalytical
     */
    virtual int ConvertMarkupAnalyticalEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Convert markup of artic@artic multi value into distinct artic elements.
     * See Doc::ConvertMarkupAnalyticalDoc
     */
    virtual int ConvertMarkupArtic(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::ConvertMarkupArtic
     */
    virtual int ConvertMarkupArticEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Move scoreDef clef, keySig, meterSig and mensur to staffDef.
     * When a staffDef already has one, it is not replaced.
     */
    virtual int ConvertMarkupScoreDef(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::ConvertMarkupScoreDef
     */
    virtual int ConvertMarkupScoreDefEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Save the content of any object by calling the appropriate FileOutputStream method.
     */
    virtual int Save(FunctorParams *functorParams);

    /**
     * End Functor for Object::Save
     */
    virtual int SaveEnd(FunctorParams *functorParams);

    ///@}

    /**
     * @name Functors for aligning and adjusting the content horizontally
     */
    ///@{

    /**
     * Reset the horizontal alignment environment for various types for object.
     */
    virtual int ResetHorizontalAlignment(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Align horizontally the content of a page.
     * For each LayerElement, instanciate its Alignment.
     * It creates it if no other note or event occurs at its position.
     * At the end, for each Layer, align the grace note stacked in GraceAlignment.
     */
    virtual int AlignHorizontally(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AlignHorizontally
     */
    virtual int AlignHorizontallyEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Align the measures by adjusting the m_drawingXRel position looking at the MeasureAligner.
     * At the end, store the width of the system in the MeasureAligner for justification.
     */
    virtual int AlignMeasures(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AlignMeasures
     */
    virtual int AlignMeasuresEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calc the position of the Alignment.
     * Looks at the time difference from the previous Alignment.
     */
    virtual int CalcAlignmentXPos(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calc the drawing position (m_drawingX and m_drawingY) values for objects
     */
    virtual int CalcAlignmentPitchPos(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the drawing stem positions, including for beams.
     */
    virtual int CalcStem(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the note head flipped positions
     */
    virtual int CalcChordNoteHeads(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the drawing dot positions, including for chords.
     */
    virtual int CalcDots(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Resolve spanning beamspans by breaking it into separate parts, each belonging to the corresponding
     * system/measure. BeamSpans get elements reassigned, so that each beamSpan can be drawn as control
     * element. This allows free placement of beamSpan in the MEI tree and ensures that beamSpan will be
     * drawn properly
     */
    virtual int CalcSpanningBeamSpans(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the slur direction
     */
    virtual int CalcSlurDirection(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the Arpeg position
     */
    virtual int AdjustArpeg(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AdjustArpeg
     */
    virtual int AdjustArpegEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the spacing for clef changes.
     */
    virtual int AdjustClefChanges(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of the dots for multiple layers
     */
    virtual int AdjustDots(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AdjustDots
     */
    virtual int AdjustDotsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of notes and chords for multiple layers
     */
    virtual int AdjustLayers(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AdjustLayers
     */
    virtual int AdjustLayersEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the X positions of the grace notes looking at the bounding boxes.
     * The functor is redirected from the MeasureAligner and then from the appropriate
     * alignment to the GraceAligner
     */
    virtual int AdjustGraceXPos(FunctorParams *) { return FUNCTOR_CONTINUE; };

    /**
     * End Functor for Object::AdjustGraceXPos
     */
    virtual int AdjustGraceXPosEnd(FunctorParams *) { return FUNCTOR_CONTINUE; };

    /**
     * Adjust the horizontal position of harms by groups in order to avoid overlapping
     */
    virtual int AdjustHarmGrpsSpacing(FunctorParams *) { return FUNCTOR_CONTINUE; };

    /**
     * End Functor for Object::AdjustHarmGrpsSpacing
     */
    virtual int AdjustHarmGrpsSpacingEnd(FunctorParams *) { return FUNCTOR_CONTINUE; };

    /**
     * Adjust the X position of accidental.
     */
    virtual int AdjustAccidX(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the X position of accidental.
     */
    virtual int AdjustTempo(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the X position of a right barline in order to make sure the is no text content
     * overlflowing in the right margin
     */
    virtual int AdjustXOverflow(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AdjustXOverflow
     */
    virtual int AdjustXOverflowEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the X positions of the staff content looking at the bounding boxes.
     * The functor process by aligned-staff content, that is from a rediction in the
     * MeasureAligner and then staff by staff but taking into account cross-staff elements
     */
    virtual int AdjustXPos(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AdjustXPos
     */
    virtual int AdjustXPosEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the spacing of the syl processing verse by verse
     */
    virtual int AdjustSylSpacing(FunctorParams *) { return FUNCTOR_CONTINUE; };

    /**
     * End Functor for Object::AdjustSylSpacing
     */
    virtual int AdjustSylSpacingEnd(FunctorParams *) { return FUNCTOR_CONTINUE; };

    /**
     * Calculate the X position of tuplet brackets and num
     */
    virtual int AdjustTupletsX(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Cache or restore cached horizontal layout for faster layout redoing
     */
    virtual int CacheHorizontalLayout(FunctorParams *) { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors for aligning the content vertically.
     */
    ///@{

    /**
     * Reset the verticall alignment environment for various types for object.
     */
    virtual int ResetVerticalAlignment(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Align vertically the content of a page.
     * For each Staff, instanciate its StaffAlignment.
     */
    virtual int AlignVertically(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AlignVertically
     */
    virtual int AlignVerticallyEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the note position for each note in ligature
     */
    virtual int CalcLigatureNotePos(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the ledger lines
     */
    virtual int CalcLedgerLines(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::CalcLedgerLines
     */
    virtual int CalcLedgerLinesEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the position of the outside articulations.
     */
    virtual int CalcArtic(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the vertical position adjustment for the beam if it overlaps with layer elements
     */
    virtual int AdjustBeams(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Apply position adjustment that has been calculated previously
     */
    virtual int AdjustBeamsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the postion position of slurs.
     */
    virtual int AdjustSlurs(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position the articulations.
     */
    virtual int AdjustArtic(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position the outside articulations with slur.
     */
    virtual int AdjustArticWithSlurs(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of cross-staff elements after the adjustment of the staves.
     * This is called by chords and tuplets with cross-staff content.
     */
    virtual int AdjustCrossStaffYPos(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of all floating positionners, staff by staff.
     */
    virtual int AdjustFloatingPositioners(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of floating positionners placed between staves
     */
    virtual int AdjustFloatingPositionersBetween(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of all floating positionner that are grouped, staff by staff.
     */
    virtual int AdjustFloatingPositionerGrps(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the overlap of the staff aligmnents by looking at the overflow bounding boxes
     */
    virtual int AdjustStaffOverlap(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the Y position of tuplet brackets and num
     */
    virtual int AdjustTupletsY(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the Y relative position of tupletNum based on overlaps with other elements
     */
    virtual int AdjustTupletNumOverlap(FunctorParams *) const { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the position of the StaffAlignment.
     */
    virtual int AdjustYPos(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust the X/YRel positions taking into account the bounding boxes
     */
    virtual int AdjustXRelForTranscription(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Fill the arrays of bounding boxes (above and below) for each staff alignment for which the box overflows.
     */
    virtual int CalcBBoxOverflows(FunctorParams *functorParams);

    /**
     * Fill the arrays of bounding boxes (above and below) for each staff alignment for which the box overflows.
     */
    virtual int CalcBBoxOverflowsEnd(FunctorParams *functorParams);

    /**
     * Align the system by adjusting the m_drawingYRel position looking at the SystemAligner.
     */
    virtual int AlignSystems(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::AlignSystems
     */
    virtual int AlignSystemsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors for aligning the pages.
     */
    ///@{

    /**
     * Apply the Pixel Per Unit factor of the page to its elements.
     */
    virtual int ApplyPPUFactor(FunctorParams *) { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors setting the current scoreDef.
     */
    ///@{

    /**
     * Replace the drawing values a staffDef.
     * Set the current / drawing clef, key signature, etc. to the StaffDef
     * Called form ScoreDef::ReplaceDrawingValues.
     */
    virtual int ReplaceDrawingValuesInStaffDef(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the Page::m_score and Page::m_scoreEnd pointers
     * Always set a the end of Page (both in BACKWARD and FORWARD directions)
     */
    virtual int ScoreDefSetCurrentPage(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::ScoreDefSetCurrentPage
     */
    virtual int ScoreDefSetCurrentPageEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the current scoreDef wherever need.
     * This is include a scoreDef for each system.
     * It also includes a scoreDef for each measure where a change occured before.
     * A change can be either a scoreDef before or a clef, meterSig, etc. within the previous measure.
     */
    virtual int ScoreDefSetCurrent(FunctorParams *functorParams);

    /**
     * Optimize the scoreDef for each system.
     * For automatic breaks, looks for staves with only mRests.
     */
    virtual int ScoreDefOptimize(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::ScoreDefOptimize
     */
    virtual int ScoreDefOptimizeEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the cautionnary scoreDef wherever need.
     */
    virtual int SetCautionaryScoreDef(FunctorParams *functorParams);

    /**
     * Unset the initial scoreDef of each system and measure
     */
    virtual int ScoreDefUnsetCurrent(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set drawing flags for the StaffDef for indicating whether clefs, keysigs, etc. need
     * to be redrawn.
     * This typically occurs when a new System or a new  ScoreDef is encountered.
     * See implementation and Object::SetStaffDefRedrawFlags for the parameters.
     */
    virtual int SetStaffDefRedrawFlags(FunctorParams *) { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors for preparing the data.
     */
    ///@{

    /**
     * One time member initialization at the very begin
     */
    virtual int PrepareDataInitialization(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set the drawing cue size of all LayerElement
     */
    virtual int PrepareCueSize(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * See cross-staff / layer pointers on LayerElement
     */
    virtual int PrepareCrossStaff(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareCrossStaff
     */
    virtual int PrepareCrossStaffEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Prepare group symbol starting and ending staffDefs for drawing
     * TODO called outside Doc::PrepareData - should maybe be moved to ScoreDef related functors
     */
    virtual int ScoreDefSetGrpSym(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Match @altsym element to the corresponding symbolDef.
     */
    virtual int PrepareAltSym(FunctorParams *functorParams);

    /**
     * Associate LayerElement with @facs to the appropriate zone
     */
    virtual int PrepareFacsimile(FunctorParams *functorParams);

    /**
     * Match linking element (e.g., \@next).
     */
    virtual int PrepareLinking(FunctorParams *functorParams);

    /**
     * Prepare list of elements in the \@plist.
     */
    virtual int PreparePlist(FunctorParams *functorParams);

    /**
     * Match elements of \@plist
     */
    virtual int PrepareProcessPlist(FunctorParams *functorParams);

    /**
     * Extract default duration from scoredef/staffdef
     */
    virtual int PrepareDuration(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Match start for TimePointingInterface elements (such as fermata or harm).
     */
    virtual int PrepareTimePointing(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareTimePointing
     */
    virtual int PrepareTimePointingEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Match start and end for TimeSpanningInterface elements (such as tie or slur).
     * If fillList is set to false, only the remaining elements will be matched.
     * This is used when processing a second time in the other direction
     */
    virtual int PrepareTimeSpanning(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareTimeSpanning
     */
    virtual int PrepareTimeSpanningEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Match start and end for TimeSpanningInterface elements with tstamp(2) attributes.
     * It is performed only on TimeSpanningInterface elements withouth \@startid (or \@endid).
     * It adds to the start (and end) measure a TimeStampAttr to the Measure::m_tstamps.
     */
    virtual int PrepareTimestamps(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareTimestamps
     */
    virtual int PrepareTimestampsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Process by Layer and set drawing pointers.
     * Set Dot::m_drawingNote for Dot elements in mensural mode
     */
    virtual int PreparePointersByLayer(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Set wordpos and connector ends
     * The functor is processed by staff/layer/verse using an Filters class.
     * At the end, the functor is processed by doc at the end of a document of closing opened syl.
     */
    virtual int PrepareLyrics(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareLyrics
     */
    virtual int PrepareLyricsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Functor for setting the artic parts.
     * Splits the artic content into different artic parts if necessary
     */
    virtual int PrepareLayerElementParts(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Functor for setting mRpt drawing numbers (if required)
     * The functor is processed by staff/layer using Filters class.
     */
    virtual int PrepareRpt(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Functor for setting Turn::m_drawingEndNote for delayed turns
     * Need a first pass to fill the map with m_initMap to true
     * Processed by staff/layer after that
     */
    virtual int PrepareDelayedTurns(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Functor for setting Measure of Ending
     */
    virtual int PrepareMilestones(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Functor for grouping FloatingObject by drawingGrpId.
     * Also chains the Dynam and Hairpin
     */
    virtual int PrepareFloatingGrps(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareFloatingGrps
     */
    virtual int PrepareFloatingGrpsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Go through all the TimeSpanningInterface elements and set them a current to each staff
     * where required. For Note with DrawingTieAttr, the functor is redirected to the tie object.
     * At the end, remove the TimeSpanningInterface element from the list when the last measure is reached.
     */
    virtual int PrepareStaffCurrentTimeSpanning(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::PrepareStaffCurrentTimeSpanning
     */
    virtual int PrepareStaffCurrentTimeSpanningEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Resolve Reh time pointing position in case none is set
     */
    virtual int PrepareRehPosition(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Get the list of referenced elements for the beamSpan as well as set referenced
     * object for those elements to beamSpan containing them.
     */
    virtual int PrepareBeamSpanElements(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Reset the drawing values before calling PrepareData after changes.
     */
    virtual int ResetData(FunctorParams *) { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors for justification.
     */
    ///@{

    /**
     * Justify the X positions
     */
    virtual int JustifyX(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Justify the Y positions
     */
    virtual int JustifyY(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust cross staff content after vertical justification
     */
    virtual int JustifyYAdjustCrossStaff(FunctorParams *) { return FUNCTOR_CONTINUE; }

    ///@}

    /**
     * @name Functors for calculating the layout of a document or of a selection.
     */
    ///@{

    /**
     * Fill a page by adding systems with the appropriate length.
     * At the end, add all the pending objects where reaching the end
     */
    virtual int CastOffSystems(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::CastOffSystems
     */
    virtual int CastOffSystemsEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Fill a doc by adding pages with the appropriate length.
     */
    virtual int CastOffPages(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::CastOffPages
     */
    virtual int CastOffPagesEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Cast off the document according to the encoding provided (pb and sb)
     */
    virtual int CastOffEncoding(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Undo the cast of both pages and system.
     * This is used by Doc::ContinuousLayout for putting all pages / systems continously.
     */
    virtual int UnCastOff(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * CastOff a document to selection.
     * Move everything before the selection to the first page, the selection to a second page,
     * and everthing after the selection to a third page.
     */
    virtual int CastOffToSelection(FunctorParams *) { return FUNCTOR_CONTINUE; };

    ///@}

    /**
     * @name Functors for generating MIDI output.
     */
    ///@{

    /**
     * Prepare Note onsets
     */
    virtual int InitOnsetOffset(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::InitOnsetOffset
     */
    virtual int InitOnsetOffsetEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Calculate the maximum duration of each measure.
     */
    virtual int InitMaxMeasureDuration(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::CalcMaxMeasureDuration
     */
    virtual int InitMaxMeasureDurationEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Adjust note timings based on ties
     */
    virtual int InitTimemapTies(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Initialize the MIDI export
     * Captures information (i.e. from control elements) for MIDI interpretation
     * This information is usually required beforehand in GenerateMIDI
     */
    virtual int InitMIDI(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Export the object to a MidiFile
     */
    virtual int GenerateMIDI(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End Functor for Object::GenerateMIDI
     */
    virtual int GenerateMIDIEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Export the object to a JSON timemap file.
     */
    virtual int GenerateTimemap(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * Export the object to a JSON feature file.
     */
    virtual int GenerateFeatures(FunctorParams *functorParams);

    ///@}

    /**
     * Reorder elements by x-position.
     */
    virtual int ReorderByXPos(FunctorParams *);

    /**
     * Transpose the content.
     */
    virtual int Transpose(FunctorParams *) { return FUNCTOR_CONTINUE; }

    /**
     * End functor for Object::Transpose
     */
    virtual int TransposeEnd(FunctorParams *) { return FUNCTOR_CONTINUE; }

private:
    /**
     * Method for generating the id.
     */
    void GenerateID();

    /**
     * Initialisation method taking the class id and a id prefix argument.
     */
    void Init(ClassId classId, const std::string &classIdStr);

    /**
     * Helper methods for functor processing
     */
    ///@{
    void UpdateDocumentScore(bool direction);
    bool SkipChildren(Functor *functor) const;
    bool FiltersApply(const Filters *filters, Object *object) const;
    ///@}

public:
    /**
     * Keep an array of unsupported attributes as pairs.
     * This can be used for writing back data
     */
    ArrayOfStrAttr m_unsupported;

protected:
    //
private:
    /**
     * A vector of child objects.
     * Unless SetAsReferenceObject is set or with detached and relinquished, the children are own by it.
     */
    ArrayOfObjects m_children;

    /**
     * A pointer to the parent object;
     */
    Object *m_parent;

    /**
     * The class id representing the actual (derived) class
     */
    ClassId m_classId;

    /**
     * Members for storing / generating ids
     */
    ///@{
    std::string m_id;
    std::string m_classIdStr;
    ///@}

    /**
     * A reference object do not own children.
     * Destructor will not delete them.
     */
    bool m_isReferenceObject;

    /**
     * Indicates whether the object content is up-to-date or not.
     * This is useful for object using sub-lists of objects when drawing.
     * For example, Beam has a list of children notes and this value indicates if the
     * list needs to be updated or not. Is is mostly an optimization feature.
     */
    mutable bool m_isModified;

    /**
     * Members used for caching iterator values.
     * See Object::IterGetFirst, Object::IterGetNext and Object::IterIsNotEnd
     * Values are set when GetFirst is called (which is mandatory)
     */
    ///@{
    mutable ArrayOfObjects::const_iterator m_iteratorEnd, m_iteratorCurrent;
    mutable ClassId m_iteratorElementType;
    ///@}

    /**
     * A vector for storing the list of AttClassId (MEI att classes) implemented.
     */
    std::vector<AttClassId> m_attClasses;

    /**
     * A vector for storing the list of InterfaceId (group of MEI att classes) implemented.
     */
    std::vector<InterfaceId> m_interfaces;

    /**
     * String for storing a comments attached to the object when printing an MEI element.
     * m_comment is to be printed immediately before the element
     * m_closingComment is to be printed before the closing tag of the element
     */
    std::string m_comment;
    std::string m_closingComment;
    ///@}

    /**
     * A flag indicating if the Object represents an attribute in the original MEI.
     * For example, a Artic child in Note for an original \@artic
     */
    bool m_isAttribute;

    /**
     * A flag indicating if the Object is a copy created by an expanded expansion element.
     */
    bool m_isExpansion;

    //----------------//
    // Static members //
    //----------------//

    /**
     * A static counter for id generation.
     */
    static thread_local unsigned long s_objectCounter;

    /**
     * Pseudo random number engine for ID generation
     */
    static thread_local std::mt19937 s_randomGenerator;
};

//----------------------------------------------------------------------------
// ObjectListInterface
//----------------------------------------------------------------------------

/**
 * This class is an pseudo interface for elements maintaining a flat list of
 * children LayerElement for processing.
 * The list is a flatten list of pointers to children elements.
 * It is not an abstract class but should not be instanciated directly.
 */
class ObjectListInterface {
public:
    // constructors and destructors
    ObjectListInterface(){};
    virtual ~ObjectListInterface(){};
    ObjectListInterface(const ObjectListInterface &listInterface); // copy constructor;
    ObjectListInterface &operator=(const ObjectListInterface &listInterface); // copy assignement;

    /**
     * Look for the Object in the list and return its position (-1 if not found)
     */
    int GetListIndex(const Object *listElement) const;

    /**
     * Gets the first item of type elementType starting at startFrom
     */
    ///@{
    const Object *GetListFirst(const Object *startFrom, const ClassId classId = UNSPECIFIED) const;
    Object *GetListFirst(const Object *startFrom, const ClassId classId = UNSPECIFIED);
    const Object *GetListFirstBackward(const Object *startFrom, const ClassId classId = UNSPECIFIED) const;
    Object *GetListFirstBackward(const Object *startFrom, const ClassId classId = UNSPECIFIED);
    ///@}

    /**
     * Returns the previous object in the list (NULL if not found)
     */
    ///@{
    const Object *GetListPrevious(const Object *listElement) const;
    Object *GetListPrevious(const Object *listElement);
    ///@}

    /**
     * Returns the next object in the list (NULL if not found)
     */
    ///@{
    const Object *GetListNext(const Object *listElement) const;
    Object *GetListNext(const Object *listElement);
    ///@}

    /**
     * Return the list.
     * Before returning the list, it checks that the list is up-to-date with Object::IsModified
     * If not, it updates the list and also calls FilterList.
     * Because this is an interface, we need to pass the object - not the best design.
     */
    ///@{
    const ListOfConstObjects &GetList(const Object *node) const;
    ListOfObjects GetList(const Object *node);
    ///@}

    /**
     * Convenience functions that check if the list is up-to-date
     * If not, the list is updated before returning the result
     */
    ///@{
    bool HasEmptyList(const Object *node) const;
    int GetListSize(const Object *node) const;
    const Object *GetListFront(const Object *node) const;
    Object *GetListFront(const Object *node);
    const Object *GetListBack(const Object *node) const;
    Object *GetListBack(const Object *node);
    ///@}

private:
    mutable ListOfConstObjects m_list;

protected:
    /**
     * Filter the list for a specific class.
     * For example, keep only notes in Beam
     */
    virtual void FilterList(ListOfConstObjects &childList) const {};

public:
    /**
     * Reset the list of children and call FilterList().
     * As for GetList, we need to pass the object.
     */
    void ResetList(const Object *node) const;
};

//----------------------------------------------------------------------------
// TextListInterface
//----------------------------------------------------------------------------

/**
 * This class is an pseudo interface for elements maintaining a flat list of
 * children LayerElement for processing.
 * The list is a flatten list of pointers to children elements.
 * It is not an abstract class but should not be instanciate directly.
 */
class TextListInterface : public ObjectListInterface {
public:
    // constructors and destructors
    TextListInterface(){};
    virtual ~TextListInterface(){};

    /**
     * Returns a contatenated version of all the text children
     */
    std::u32string GetText(const Object *node) const;

    /**
     * Fill an array of lines with concatenated content of each line
     */
    void GetTextLines(const Object *node, std::vector<std::u32string> &lines) const;

protected:
    /**
     * Filter the list for a specific class.
     * For example, keep only notes in Beam
     */
    void FilterList(ListOfConstObjects &childList) const override;

private:
    //
public:
    //
private:
};

//----------------------------------------------------------------------------
// Functor
//----------------------------------------------------------------------------

class Functor {
private:
    int (Object::*obj_fpt)(FunctorParams *functorParams); // pointer to member function
    int (Object::*const_obj_fpt)(FunctorParams *functorParams) const;

public:
    // constructor - takes pointer to a functor method and stores it
    Functor();
    Functor(int (Object::*_obj_fpt)(FunctorParams *));
    Functor(int (Object::*_const_obj_fpt)(FunctorParams *) const);
    virtual ~Functor(){};

    // Call the internal functor method
    void Call(Object *ptr, FunctorParams *functorParams);
    void Call(const Object *ptr, FunctorParams *functorParams);

private:
    //
public:
    /**
     * The return code of the functor.
     * FUNCTOR_CONTINUE: continue processing
     * FUNCTOR_SIBLINGS: process only siblings (do not go deeper)
     * FUNCTOR_STOP: stop the functor (e.g., when an Object or a value is found)
     */
    int m_returnCode;
    /**
     * A flag for indicating if only visible Object have to be processed.
     * The value is true by default.
     */
    bool m_visibleOnly;

private:
};

//----------------------------------------------------------------------------
// ObjectComparison
//----------------------------------------------------------------------------

/**
 * Unary predicate for comparing object types.
 * This is used for example in std::find_if.
 * See Object::GetFirst or Object::GetNext
 */
class ObjectComparison {

public:
    ObjectComparison(const ClassId classId) { m_classId = classId; }

    bool operator()(const Object *object)
    {
        if (m_classId == UNSPECIFIED) {
            return true;
        }
        return (object->GetClassId() == m_classId);
    }

private:
    //
public:
    //
private:
    ClassId m_classId;
};

//----------------------------------------------------------------------------
// ObjectFactory
//----------------------------------------------------------------------------

class ObjectFactory {

public:
    /**
     * A static method returning a static object in order to guarantee initialisation
     */
    static ObjectFactory *GetInstance();

    /**
     * Create the object from the MEI element string name by making a lookup in the register
     */
    Object *Create(std::string name);

    /**
     * Add the name / constructor map entry to the static register
     */
    void Register(std::string name, ClassId classId, std::function<Object *(void)> function);

    /**
     * Get the ClassId from the MEI element string name by making a lookup in the register
     */
    ClassId GetClassId(std::string name);

    /**
     * Get the correspondings ClassIds from the vector of MEI element string names
     */
    void GetClassIds(const std::vector<std::string> &classStrings, std::vector<ClassId> &classIds);

public:
    MapOfStrConstructors s_ctorsRegistry;
    MapOfStrClassIds s_classIdsRegistry;
};

//----------------------------------------------------------------------------
// ClassRegistrar
//----------------------------------------------------------------------------

template <class T> class ClassRegistrar {
public:
    /**
     * The contructor registering the name / constructor map
     */
    ClassRegistrar(std::string name, ClassId classId)
    {
        ObjectFactory::GetInstance()->Register(name, classId, [](void) -> Object * { return new T(); });
    }
};

} // namespace vrv

#endif
