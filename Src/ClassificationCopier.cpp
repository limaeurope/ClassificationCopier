// *****************************************************************************

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


// ---------------------------------- Variables --------------------------------


// ---------------------------------- Prototypes -------------------------------


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

	const API_Guid UNICLASS1 = APIGuidFromString("86FA0C53-4087-4D0A-9C03-495633C570FB");
	const API_Guid UNICLASS2 = APIGuidFromString("22F9077D-DA34-42BB-8930-C1179B93D7E3");

	system.guid  = UNICLASS1;
	system2.guid = UNICLASS2;

	err = ACAPI_Classification_GetClassificationSystem(system);
	err = ACAPI_Classification_GetClassificationSystem(system2);

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

			auto is_uniclass2 = [UNICLASS2](GS::Pair<API_Guid, API_Guid> p) { return p.first == UNICLASS2; };
			bool hasUniclass2 = find_if(begin(systemItemPairs), end(systemItemPairs), is_uniclass2) != end(systemItemPairs);

			if (!hasUniclass2)
			{
				for (UInt32 j = 0; j < systemItemPairs.GetSize(); j++) {
					if (systemItemPairs[j].first == UNICLASS1) {
						API_ClassificationItem item, item2;
						item.guid = systemItemPairs[j].second;
						err = ACAPI_Classification_GetClassificationItem(item);
						isFound = true;

						if (allItems.ContainsKey(item.id))
						{
							if (allItems[item.id].name == item.name)
							{
								err = ACAPI_Element_AddClassificationItem(elemGuid, allItems[item.id].guid);

								if (err == NoError)
									if (!i_onlyErrors)
									{
										dialogString += "Success: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ( " + item.name + " ) " + "\n";
									}
								secondSelection.Push(selNeigs[i]);
							}
							else
							{
								dialogString += "Name mismatch: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ' " + item.name + " ' differs from: ' " + allItems[item.id].name + " ' " + "\n";
							}
						}
						else
						{
							dialogString += "Id mismatch: element " + APIGuidToString(elemGuid) + " Uniclass 2015 1.0 ID: " + item.id + " ( " + item.name + " ) has no matching ID in Uniclass 2015 2.0" + "\n";
						}
					}
				}

				if (!isFound)
					dialogString += "Not found: element " + APIGuidToString(elemGuid) + " has no Uniclass 2015 1.0 ID\n";
			}
			else
			{
				dialogString += "Not needed: element " + APIGuidToString(elemGuid) + " already has Uniclass 2015 2.0 ID\n";
			}
		}
		else
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

GSErrCode __ACENV_CALL ElementsSolidOperation (const API_MenuParams *menuParams)
{
	return ACAPI_CallUndoableCommand ("Copy Classifications",
		[&] () -> GSErrCode {
			switch (menuParams->menuItemRef.itemIndex) {
				case 1:		CopyClassifications();				break;
				case 2:		CopyClassifications(true);			break;
				case 3:		CopyClassifications(true, true);	break;

				default:										break;
			}

			return NoError;
		});
}		/* ElementsSolidOperation */


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
	GSErrCode err = NoError;

	//
	// Install menu handler callbacks
	//

	err = ACAPI_Install_MenuHandler (32506, ElementsSolidOperation);

	return err;
}		/* Initialize */


// -----------------------------------------------------------------------------
// Called when the Add-On is going to be unloaded
// -----------------------------------------------------------------------------

GSErrCode __ACENV_CALL	FreeData (void)
{
	return NoError;
}		/* FreeData */
