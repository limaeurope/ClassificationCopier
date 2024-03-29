﻿// *****************************************************************************

// *****************************************************************************

// ---------------------------------- Includes ---------------------------------

#include	"APIEnvir.h"
#include	"ACAPinc.h"					// also includes APIdefs.h
#include	"APICommon.h"

#include	"ClassificationCopier.h"
#include	"DG.h"
#include	"Algorithms.hpp"

using namespace std;

// ---------------------------------- Types ------------------------------------

struct IdNamePair {
	GS::UniString	id;
	GS::UniString	name;
};

 struct CntlDlgData {
	Int32 iSource;
	Int32 iTarget;

	GS::HashTable<UInt16, API_ClassificationSystem> systems;
	Int32 iSuccessCheckBox = 1;
	Int32 iSuccessMappedCheckBox = 1;
	Int32 iNameMismtchCheckBox = 1;
	Int32 iIdMismtchCheckBox = 1;
	Int32 iNotFoundCheckBox = 1;
	Int32 iNotNeededCheckBox = 1;
	Int32 iNotApplicableCheckBox = 1;

	//FIXME include Classification system id for various classification systems
	const GS::HashTable<GS::UniString, IdNamePair> translations{
		{	{GS::UniString("Ss_20_20_75_15")/*, GS::UniString("Concrete beam systems")*/}, 
			{GS::UniString("Ss_20_20_75_70"), GS::UniString("Reinforced concrete beam systems")}},
		{	{GS::UniString("Ss_20_30_75_15")/*, GS::UniString("Concrete column systems")*/}, 
			{GS::UniString("Ss_20_30_75_70"), GS::UniString("Reinforced concrete column systems")}},
		{	{GS::UniString("Ss_30_12_85_70")/*, GS::UniString("Reinforced concrete floor, roof or balcony deck systems")*/}, 
			{GS::UniString("Ss_30_12_85_70"), GS::UniString("Reinforced concrete deck systems")}},
	};
} ;

#define OK_BUTTON				 1
#define SOURCE_POPUP			 2
#define TARGET_POPUP			 3
#define SUCCESS_CHECKBOX		 4
#define SUCCESS_MAPPED_CHECKBOX	 5
#define NAME_MISMTCH_CHECKBOX	 6
#define ID_MISMTCH_CHECKBOX		 7
#define NOT_FOUND_CHECKBOX		 8
#define NOT_NEEDED_CHECKBOX		 9
#define NOT_APPLICABLE_CHECKBOX 10

// ---------------------------------- Variables --------------------------------

static CntlDlgData			cntlDlgData;
static bool					isAddonInitialized = false;

// ---------------------------------- Prototypes -------------------------------

void InitializeAddonData()
{
	GS::Array<API_ClassificationSystem> systems{};

	GSErrCode err = ACAPI_Classification_GetClassificationSystems(systems);
	UInt16 i = 1;

	const GS::UniString UNICLASS1_NAME = "Uniclass 2015 - LIMA";
	const GS::UniString UNICLASS1_VER = "v 1.0";
	const GS::UniString UNICLASS2_NAME = "Uniclass 2015";
	const GS::UniString UNICLASS2_VER = "LIMA - v2.0";

	for (auto sys : systems)
	{
		cntlDlgData.systems.Add(++i, sys);

		if (sys.name == UNICLASS1_NAME
			&& sys.editionVersion == UNICLASS1_VER)
			cntlDlgData.iSource = i;

		if (sys.name == UNICLASS2_NAME
			&& sys.editionVersion == UNICLASS2_VER)
			cntlDlgData.iTarget = i;
	}

	isAddonInitialized = true;
}

void GetClassificationSystems(short dialID)
{
	GS::Array<API_ClassificationSystem> systems{};

	GSErrCode err = ACAPI_Classification_GetClassificationSystems(systems);

	DGPopUpDeleteItem(dialID, SOURCE_POPUP, DG_ALL_ITEMS);
	DGPopUpDeleteItem(dialID, TARGET_POPUP, DG_ALL_ITEMS);
	DGPopUpInsertItem(dialID, SOURCE_POPUP, DG_LIST_BOTTOM);
	DGPopUpInsertItem(dialID, TARGET_POPUP, DG_LIST_BOTTOM);

	for (auto i : cntlDlgData.systems.Keys())
	{
		auto sys = cntlDlgData.systems[i];
		auto src = cntlDlgData.systems.ContainsKey(cntlDlgData.iSource) ? &cntlDlgData.systems[cntlDlgData.iSource] : nullptr;
		auto trg = cntlDlgData.systems.ContainsKey(cntlDlgData.iTarget) ? &cntlDlgData.systems[cntlDlgData.iTarget] : nullptr;

		DGPopUpInsertItem(dialID, SOURCE_POPUP, DG_LIST_BOTTOM);
		DGPopUpSetItemText(dialID, SOURCE_POPUP, DG_LIST_BOTTOM, sys.name + " " + sys.editionVersion);
		DGPopUpInsertItem(dialID, TARGET_POPUP, DG_LIST_BOTTOM);
		DGPopUpSetItemText(dialID, TARGET_POPUP, DG_LIST_BOTTOM, sys.name + " " + sys.editionVersion);
	}
}


void RefreshUI(short dialID)
{
	for (auto i : cntlDlgData.systems.Keys())
	{
		if (cntlDlgData.iSource > 0)
			DGPopUpSetItemStatus(dialID, SOURCE_POPUP, i, DG_IS_ENABLE);
			DGPopUpSelectItem(dialID, SOURCE_POPUP, cntlDlgData.iSource);

		if (cntlDlgData.iTarget > 0)
			DGPopUpSetItemStatus(dialID, TARGET_POPUP, i, DG_IS_ENABLE);
			DGPopUpSelectItem(dialID, TARGET_POPUP, cntlDlgData.iTarget);

		if (cntlDlgData.iTarget == i)
			DGPopUpSetItemStatus(dialID, SOURCE_POPUP, i, DG_IS_DISABLE);
		else
			DGPopUpSetItemStatus(dialID, SOURCE_POPUP, i, DG_IS_ENABLE);

		if (cntlDlgData.iSource == i)
			DGPopUpSetItemStatus(dialID, TARGET_POPUP, i, DG_IS_DISABLE);
		else
			DGPopUpSetItemStatus(dialID, TARGET_POPUP, i, DG_IS_ENABLE);
	}

	DGSetCheckBoxState(dialID, SUCCESS_CHECKBOX, cntlDlgData.iSuccessCheckBox);
	DGSetCheckBoxState(dialID, SUCCESS_MAPPED_CHECKBOX, cntlDlgData.iSuccessMappedCheckBox);
	DGSetCheckBoxState(dialID, NAME_MISMTCH_CHECKBOX, cntlDlgData.iNameMismtchCheckBox);
	DGSetCheckBoxState(dialID, ID_MISMTCH_CHECKBOX, cntlDlgData.iIdMismtchCheckBox);
	DGSetCheckBoxState(dialID, NOT_FOUND_CHECKBOX, cntlDlgData.iNotFoundCheckBox);
	DGSetCheckBoxState(dialID, NOT_NEEDED_CHECKBOX, cntlDlgData.iNotNeededCheckBox);
	DGSetCheckBoxState(dialID, NOT_APPLICABLE_CHECKBOX, cntlDlgData.iNotApplicableCheckBox);
}


static short DGCALLBACK CntlDlgCallBack(short message, short dialID, short item, DGUserData userData, DGMessageData msgData)
{
	short result = 0;

	switch (message) {
	case DG_MSG_INIT:
	{
		if (!isAddonInitialized) InitializeAddonData();
		GetClassificationSystems(dialID);
		RefreshUI(dialID);

		break;
	}
	case DG_MSG_CLICK:
		switch (item) {
		case DG_OK:
			result = item;
			break;
		}

		break;
	case DG_MSG_CLOSE:
		result = item;
		if (item == DG_OK) {
		}
		break;
	case DG_MSG_CHANGE:
		switch (item) {
		case SOURCE_POPUP:
		{
			cntlDlgData.iSource = DGPopUpGetSelected(dialID, SOURCE_POPUP);

			RefreshUI(dialID);
			break;
		}
		case TARGET_POPUP:
		{
			cntlDlgData.iTarget = DGPopUpGetSelected(dialID, TARGET_POPUP);

			RefreshUI(dialID);
			break;
		}
		case SUCCESS_CHECKBOX:
		case SUCCESS_MAPPED_CHECKBOX:
		case NAME_MISMTCH_CHECKBOX:
		case ID_MISMTCH_CHECKBOX:
		case NOT_FOUND_CHECKBOX:
		case NOT_NEEDED_CHECKBOX:
		case NOT_APPLICABLE_CHECKBOX:
			cntlDlgData.iSuccessCheckBox = DGGetCheckBoxState(dialID, SUCCESS_CHECKBOX);
			cntlDlgData.iSuccessMappedCheckBox = DGGetCheckBoxState(dialID, SUCCESS_MAPPED_CHECKBOX);
			cntlDlgData.iNameMismtchCheckBox = DGGetCheckBoxState(dialID, NAME_MISMTCH_CHECKBOX);
			cntlDlgData.iIdMismtchCheckBox = DGGetCheckBoxState(dialID, ID_MISMTCH_CHECKBOX);
			cntlDlgData.iNotFoundCheckBox = DGGetCheckBoxState(dialID, NOT_FOUND_CHECKBOX);
			cntlDlgData.iNotNeededCheckBox = DGGetCheckBoxState(dialID, NOT_NEEDED_CHECKBOX);
			cntlDlgData.iNotApplicableCheckBox = DGGetCheckBoxState(dialID, NOT_APPLICABLE_CHECKBOX);

			break;
		}

		break;
	}

	ACAPI_KeepInMemory(true);
	return result;
}

static GSErrCode	DisplaySettingsWindow()
{
	GSErrCode err = DGModalDialog(ACAPI_GetOwnResModule(), 32400, ACAPI_GetOwnResModule(), CntlDlgCallBack, (DGUserData)&cntlDlgData);

	return err;
}

static bool IsClassificationApplicableForElement(const API_ElemTypeID &i_elementType)
{
	return	i_elementType == API_WallID
		||	i_elementType == API_ColumnID
		||	i_elementType == API_BeamID
		||	i_elementType == API_WindowID
		||	i_elementType == API_DoorID
		||	i_elementType == API_ObjectID
		||	i_elementType == API_LampID
		||	i_elementType == API_SlabID
		||	i_elementType == API_RoofID
		||	i_elementType == API_MeshID
		//||	i_elementType == API_DimensionID
		//||	i_elementType == API_RadialDimensionID
		//||	i_elementType == API_LevelDimensionID
		//||	i_elementType == API_AngleDimensionID
		//||	i_elementType == API_TextID
		//||	i_elementType == API_LabelID
		||	i_elementType == API_ZoneID
		//||	i_elementType == API_HatchID
		//||	i_elementType == API_LineID
		//||	i_elementType == API_PolyLineID
		//||	i_elementType == API_ArcID
		//||	i_elementType == API_CircleID
		//||	i_elementType == API_SplineID
		//||	i_elementType == API_HotspotID
		//||	i_elementType == API_CutPlaneID
		//||	i_elementType == API_CameraID
		//||	i_elementType == API_CamSetID
		//||	i_elementType == API_SectElemID
		//||	i_elementType == API_DrawingID
		//||	i_elementType == API_PictureID
		//||	i_elementType == API_DetailID
		//||	i_elementType == API_ElevationID
		//||	i_elementType == API_InteriorElevationID
		//||	i_elementType == API_WorksheetID
		//||	i_elementType == API_HotlinkID
		||	i_elementType == API_CurtainWallID
		||	i_elementType == API_CurtainWallSegmentID
		||	i_elementType == API_CurtainWallFrameID
		||	i_elementType == API_CurtainWallPanelID
		||	i_elementType == API_CurtainWallJunctionID
		||	i_elementType == API_CurtainWallAccessoryID
		||	i_elementType == API_ShellID
		||	i_elementType == API_SkylightID
		||	i_elementType == API_MorphID
		//||	i_elementType == API_ChangeMarkerID
		||	i_elementType == API_StairID
		||	i_elementType == API_RiserID
		||	i_elementType == API_TreadID
		||	i_elementType == API_StairStructureID
		||	i_elementType == API_RailingID
		||	i_elementType == API_RailingToprailID
		||	i_elementType == API_RailingHandrailID
		||	i_elementType == API_RailingRailID
		||	i_elementType == API_RailingPostID
		||	i_elementType == API_RailingInnerPostID
		||	i_elementType == API_RailingBalusterID
		||	i_elementType == API_RailingPanelID
		||	i_elementType == API_RailingSegmentID
		||	i_elementType == API_RailingNodeID
		||	i_elementType == API_RailingBalusterSetID
		||	i_elementType == API_RailingPatternID
		||	i_elementType == API_RailingToprailEndID
		||	i_elementType == API_RailingHandrailEndID
		||	i_elementType == API_RailingRailEndID
		||	i_elementType == API_RailingToprailConnectionID
		||	i_elementType == API_RailingHandrailConnectionID
		||	i_elementType == API_RailingRailConnectionID
		||	i_elementType == API_RailingEndFinishID
		||	i_elementType == API_BeamSegmentID
		||	i_elementType == API_ColumnSegmentID
		||	i_elementType == API_OpeningID;
}


static void GatherAllDescendantOfClassification(const API_ClassificationItem& item, GS::HashTable<GS::UniString, API_ClassificationItem>& allDescendant)
{
	GS::Array<API_ClassificationItem> directChildren;
	GSErrCode err = ACAPI_Classification_GetClassificationItemChildren(item.guid, directChildren);

	for (UIndex i = 0; i < directChildren.GetSize(); ++i) {
		allDescendant.Add(directChildren[i].id, directChildren[i]);
		GatherAllDescendantOfClassification(directChildren[i], allDescendant);
	}
}


void CopyClassifications(const bool i_writeReport /*= false*/, const bool i_onlyErrors /*= false*/)
{
	GSErrCode					err;
	API_SelectionInfo			selectionInfo;
	GS::Array<API_Neig>			selNeigs;
	GS::UniString				dialogString;
	API_Guid					elemGuid;
	API_ClassificationSystem	system, system2;
	API_Element					element;

	if (!isAddonInitialized) InitializeAddonData();

	system = cntlDlgData.systems[cntlDlgData.iSource];
	system2 = cntlDlgData.systems[cntlDlgData.iTarget];

	const API_Guid uniclass1 = system.guid;
	const API_Guid uniclass2 = system2.guid;
	
	err = ACAPI_Selection_Get(&selectionInfo, &selNeigs, false);

	if (err == APIERR_NOSEL || selectionInfo.typeID == API_SelEmpty) {
		ACAPI_WriteReport("Nothing is selected", true);
		return;
	}

	if (err != APIERR_NOSEL && err != NoError) {
		ACAPI_WriteReport("Error in ACAPI_Selection_Get: %s", true, ErrID_To_Name(err));
		return;
	}

	GS::HashTable<GS::UniString, API_ClassificationItem> allItems;
	GS::Array<API_ClassificationItem> rootItems;

	err = (ACAPI_Classification_GetClassificationSystemRootItems(system2.guid, rootItems));

	for (UIndex i = 0; i < rootItems.GetSize(); ++i) {
		allItems.Add(rootItems[i].id, rootItems[i]);
		GatherAllDescendantOfClassification(rootItems[i], allItems);
	}

	GS::Array<API_Neig> secondSelection{};

	for (Int32 i = 0; i < selectionInfo.sel_nElem; i++) {
		bool isFound = false;
		elemGuid = element.header.guid = selNeigs[i].guid;
		err = ACAPI_Element_Get(&element);
#if ACVER >= 26
		if (IsClassificationApplicableForElement(element.header.type.typeID))
#else
		if (IsClassificationApplicableForElement(element.header.typeID))
#endif
		{
			GS::Array<GS::Pair<API_Guid, API_Guid>>	systemItemPairs;

			err = ACAPI_Element_GetClassificationItems(elemGuid, systemItemPairs);

			auto is_uniclass2 = [uniclass2](GS::Pair<API_Guid, API_Guid> p) { return p.first == uniclass2; };
			bool hasUniclass2 = find_if(begin(systemItemPairs), end(systemItemPairs), is_uniclass2) != end(systemItemPairs);

			if (!hasUniclass2)
			{
				for (UInt32 j = 0; j < systemItemPairs.GetSize(); j++) {
					if (systemItemPairs[j].first == uniclass1) {
						API_ClassificationItem item, item2;
						item.guid = systemItemPairs[j].second;
						err = ACAPI_Classification_GetClassificationItem(item);
						isFound = true;

						if (cntlDlgData.translations.ContainsKey(item.id) && cntlDlgData.iSuccessMappedCheckBox)
						{
							err = ACAPI_Element_AddClassificationItem(elemGuid, allItems[cntlDlgData.translations[item.id].id].guid);

							dialogString += "Success: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ( " + item.name + " ) was mapped to " + cntlDlgData.translations[item.id].id + " ( " + cntlDlgData.translations[item.id].name + " ) \n";
						}
						else if (allItems.ContainsKey(item.id))
						{
							if (allItems[item.id].name == item.name)
							{
								err = ACAPI_Element_AddClassificationItem(elemGuid, allItems[item.id].guid);

								if (err == NoError)
									if (!i_onlyErrors && cntlDlgData.iSuccessCheckBox)
									{
										dialogString += "Success: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ( " + item.name + " ) " + "\n";
									}
								secondSelection.Push(selNeigs[i]);
							}
							else if (cntlDlgData.iNameMismtchCheckBox)
							{
								dialogString += "Name mismatch: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ' " + item.name + " ' differs from: ' " + allItems[item.id].name + " ' " + "\n";
							}
						}
						else if (cntlDlgData.iIdMismtchCheckBox)
						{
							dialogString += "Id mismatch: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ( " + item.name + " ) has no matching ID in Uniclass 2015 2.0" + "\n";
						}
					}
				}

				if (!isFound && cntlDlgData.iNotFoundCheckBox)
					dialogString += "Not found: element " + APIGuidToString(elemGuid) + " has no Uniclass 2015 1.0 ID\n";
			}
			else if (cntlDlgData.iNotNeededCheckBox)
			{
				dialogString += "Not needed: element " + APIGuidToString(elemGuid) + " already has Uniclass 2015 2.0 ID\n";
			}
		}
		else if (cntlDlgData.iNotApplicableCheckBox)
		{
			dialogString += "Not applicable: element " + APIGuidToString(elemGuid) + " cannot have a classification\n";
		}
	}

	if (i_writeReport)
		ACAPI_WriteReport(dialogString, false);

	//err = ACAPI_Element_DeselectAll();

	err = ACAPI_Element_Select(secondSelection, false);
}


// -----------------------------------------------------------------------------
// Elements: Solid Operations Functions
// -----------------------------------------------------------------------------

GSErrCode __ACENV_CALL ClassificationCopier (const API_MenuParams *menuParams)
{
	return ACAPI_CallUndoableCommand ("Copy Classifications",
		[&] () -> GSErrCode {
			switch (menuParams->menuItemRef.itemIndex) {
				case 1:		CopyClassifications();				break;
				case 2:		CopyClassifications(true);			break;
				case 3:		CopyClassifications(true, true);	break;
				case 4:		DisplaySettingsWindow();			break;
					
				default:										break;
			}

			return NoError;
		});
}		/* ClassificationCopier */


// -----------------------------------------------------------------------------
// Dependency definitions
// -----------------------------------------------------------------------------
API_AddonType __ACENV_CALL	CheckEnvironment (API_EnvirParams* envir)
{
	RSGetIndString (&envir->addOnInfo.name, 32000, 1, ACAPI_GetOwnResModule ());
	RSGetIndString (&envir->addOnInfo.description, 32000, 2, ACAPI_GetOwnResModule ());

	return APIAddon_Preload;
}		/* RegisterAddOn */


// -----------------------------------------------------------------------------
// Interface definitions
// -----------------------------------------------------------------------------
GSErrCode __ACENV_CALL	RegisterInterface (void)
{
	GSErrCode	err;

	//
	// Register menus
	//
	err = ACAPI_Register_Menu (32506, 0, MenuCode_UserDef, MenuFlag_Default);

	return err;
}		/* RegisterInterface */


// -----------------------------------------------------------------------------
// Called when the Add-On has been loaded into memory
// to perform an operation
// -----------------------------------------------------------------------------
GSErrCode __ACENV_CALL	Initialize (void)
{
	GSErrCode err;

	//
	// Install menu handler callbacks
	//

	err = ACAPI_Install_MenuHandler (32506, ClassificationCopier);

	return err;
}		/* Initialize */


// -----------------------------------------------------------------------------
// Called when the Add-On is going to be unloaded
// -----------------------------------------------------------------------------

GSErrCode __ACENV_CALL	FreeData (void)
{
	return NoError;
}		/* FreeData */
