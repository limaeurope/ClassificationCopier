// *****************************************************************************
// Source code for the Element Test Add-On
// API Development Kit 24; Mac/Win
//
//	Main and common functions
//
// Namespaces:		Contact person:
//		-None-
//
// [SG compatible] - Yes
// *****************************************************************************

// ---------------------------------- Includes ---------------------------------

#include	"APIEnvir.h"
#include	"ACAPinc.h"					// also includes APIdefs.h
#include	"APICommon.h"
//
#include	"ClassificationCopier.h"
#include	"DG.h"

// ---------------------------------- Types ------------------------------------


// ---------------------------------- Variables --------------------------------


// ---------------------------------- Prototypes -------------------------------

static void GatherAllDescendantOfClassification(const API_ClassificationItem& item, GS::HashTable<GS::UniString, API_ClassificationItem>& allDescendant)
{
	GS::Array<API_ClassificationItem> directChildren;
	GSErrCode err = ACAPI_Classification_GetClassificationItemChildren(item.guid, directChildren);

	for (UIndex i = 0; i < directChildren.GetSize(); ++i) {
		allDescendant.Add(directChildren[i].id, directChildren[i]);
		GatherAllDescendantOfClassification(directChildren[i], allDescendant);
	}
}

void CopyClassifications(const bool i_writeReport /*= false*/)
{
	const API_Guid UNICLASS1 = APIGuidFromString("86FA0C53-4087-4D0A-9C03-495633C570FB");
	const API_Guid UNICLASS2 = APIGuidFromString("22F9077D-DA34-42BB-8930-C1179B93D7E3");

	GSErrCode				err;
	API_SelectionInfo		selectionInfo;
	GS::Array<API_Neig>		selNeigs;
	GS::UniString dialogString;
	API_Guid elemGuid;
	API_ClassificationSystem system, system2;

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

	//err = ACAPI_Element_Select(selNeigs, false);		//Empty selection 

	GS::HashTable<GS::UniString, API_ClassificationItem> allItems;
	GS::Array<API_ClassificationItem> rootItems;

	err = (ACAPI_Classification_GetClassificationSystemRootItems(system2.guid, rootItems));

	for (UIndex j = 0; j < rootItems.GetSize(); ++j) {
		allItems.Add(rootItems[j].id, rootItems[j]);
		GatherAllDescendantOfClassification(rootItems[j], allItems);
	}

	for (Int32 i = 0; i < selectionInfo.sel_nElem; i++) {
		GS::Array<GS::Pair<API_Guid, API_Guid>>	systemItemPairs;

		err = ACAPI_Element_GetClassificationItems(elemGuid, systemItemPairs);

		for (UInt32 i = 0; i < systemItemPairs.GetSize(); i++) {
			if (systemItemPairs[i].first == UNICLASS1) {
				API_ClassificationItem item, item2;
				item.guid = systemItemPairs[i].second;
				err = ACAPI_Classification_GetClassificationItem(item);

				//dialogString += system.name + " " + APIGuidToString(system.guid) + " " + system.editionVersion + " - " + item.id + " " + item.name + "\n\n";

				if (allItems.ContainsKey(item.id))
				{
					if (allItems[item.id].name == item.name)
						err = ACAPI_Element_AddClassificationItem(elemGuid, allItems[item.id].guid);

						if (err == NoError)
							dialogString += system.name + " " + APIGuidToString(system.guid) + " " + system.editionVersion + " - " + item.id + " " + item.name + "\n\n";
				}
			}
		}
	}

	if (i_writeReport)
		ACAPI_WriteReport(dialogString, false);
}

// -----------------------------------------------------------------------------
// Elements: Solid Operations Functions
// -----------------------------------------------------------------------------

GSErrCode __ACENV_CALL ElementsSolidOperation (const API_MenuParams *menuParams)
{
	return ACAPI_CallUndoableCommand ("Element Test API Function",
		[&] () -> GSErrCode {

			switch (menuParams->menuItemRef.itemIndex) {
				case 1:		CopyClassifications();				break;
				case 2:		CopyClassifications(true);			break;

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
