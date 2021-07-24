/* ************************************************************************ */
/*!
   \file

   \brief          VT client demo application

   \author         Erwin Hammerl
   \date           Created 17.09.2007

   \copyright  Wegscheider Hammerl Ingenieure Partnerschaft

   \par HISTORY:

*/
/* **************************  includes ********************************** */

#include "IsoDef.h"

#ifdef _LAY6_  /* compile only if VT client is enabled */


#include "VIEngine.h"
#include "App_VTClientLev2.h"   // -> Object defines

#include "MyProject1.iop.h"
#include "MyProject1.c.h"

#include "settings.h"

#include "esp_log.h"

static const char *TAG = "App_VTClientLev2";


//********************************************************************************************
// Each command has several Unique Features. here they are encapsulated !
//********************************************************************************************
typedef struct {
	//
	//! The Commandnumber is here redundant, however a Testdriver can check for Validity !!!
	//
	iso_u16 objectIdOfAuxObject; // objectIdOfAuxObject
	iso_u16 objectIdOfButtonObject; // objectIdOfButtonObject

} CommandTranslateObject;

CommandTranslateObject CommandTranslate[] = {
	{ AuxFunction2_PlusPlus,		SoftKey_PlusPlus		}, //	0
	{ AuxFunction2_MinusMinus,		SoftKey_MinusMinus		} //	1
};


//make this Defines (Right side)
const int FIRST_AUX = AuxFunction2_PlusPlus;
const int  LAST_AUX = AuxFunction2_MinusMinus;
//do not Change this.
const int  NUM_AUX_FUNCTIONS = ((LAST_AUX-FIRST_AUX)+1);
iso_s32 InputSignalData_old_value1[20] = {AUX_PRESS_OFF};



// called from AppPoolSettings()
void VTC_setPoolManipulation(const ISOVT_EVENT_DATA_T* psEvData)
{
   iso_u16  u16DM_Scal  = 10000u;          // Scaling factor * 10000
   iso_u16  u16SKM_Scal = 10000u;

   // ------------------------------------------------------------------------------

   // IsoVtcPoolSetIDRangeMode(u8Instance, 0, 60000, 10000, NoScaling);          // Switch off automatic scaling

   u16DM_Scal = (iso_u16)IsoVtcPoolReadInfo(psEvData->u8Instance, PoolDataMaskScalFaktor);       // Call only after PoolInit !!
   u16SKM_Scal = (iso_u16)IsoVtcPoolReadInfo(psEvData->u8Instance, PoolSoftKeyMaskScalFaktor);

   IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 5100u, 5300u, u16SKM_Scal, Centering);       // Scale and center Keys
   IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 20700u, 20799u, u16SKM_Scal, Scaling);         // Scale Pictures in keys


   // ------------------------------------------------------------------------------


   IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 0u,     0u, u16SKM_Scal, Centering);  // Working set object
   IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 20000u, 20000u, u16SKM_Scal, Scaling);    // Working set designator
   IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 29000u, 29099u, u16SKM_Scal, Centering);  // Auxiliary function
   IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 20900u, 20999u, u16SKM_Scal, Scaling);    // Auxiliary bitmaps
   (void)u16DM_Scal;


	if (IsoVtcGetStatusInfo(psEvData->u8Instance, VT_VERSIONNR) == VT_V2_FE)
	{
		// Transforming Auxiliary function Type 2 into Type 1
		IsoVtcPoolSetIDRangeMode(psEvData->u8Instance, 29000, 29999, 0, AuxToV2);
	}
}





//lokale Variablen, NICHT Spannungsausfallsicher !
iso_u32 Tageszaehler = 0;
iso_u32 Gesamtzaehler = 0;
iso_u32 Tagesziel = 30;
iso_u32 Gesamtziel = 80;



//Das ist meine Favorisierte Schreibweise. man muss sich aber das ? und den : für WENN und SONST einprägen.
void CheckZiel(iso_u8 u8Instance) {

	IsoVtcCmd_NumericValue(u8Instance, ObjectPointer_Tagesziel,  Tageszaehler  >= Tagesziel  ? OutputString_ZielErreicht : ID_NULL);
	IsoVtcCmd_NumericValue(u8Instance, ObjectPointer_Gesamtziel, Gesamtzaehler >= Gesamtziel ? OutputString_ZielErreicht : ID_NULL);

}


//ALTERANTIVE Schreibweise. diese entspricht mehr der englischen Sprache.
void CheckZielAlternativ(iso_u8 u8Instance) {

	if (Tageszaehler >= Tagesziel) { //WENN Bedingung
		IsoVtcCmd_NumericValue(u8Instance, ObjectPointer_Tagesziel, OutputString_ZielErreicht); //DANN Fall
	} else {
		IsoVtcCmd_NumericValue(u8Instance, ObjectPointer_Tagesziel, ID_NULL); //SONST Fall
	}

	if(Gesamtzaehler >= Gesamtziel) {
		IsoVtcCmd_NumericValue(u8Instance, ObjectPointer_Gesamtziel, OutputString_ZielErreicht); //DANN Fall
	} else {
		IsoVtcCmd_NumericValue(u8Instance, ObjectPointer_Gesamtziel, ID_NULL); //SONST Fall
	}



}


void VTC_handleSoftkeysAndButtons_RELEASED(const struct ButtonActivation_S *pButtonData) {

	// what button was released
	switch (pButtonData->objectIdOfButtonObject) {


	// SoftKey mit Namen  SoftKey_PlusPlus mit ID 5000 wurde losgelassen.
	case SoftKey_PlusPlus:
	// Button mit Namen  Button_PlusPlus mit ID 6000 wurde losgelassen.
	case Button_PlusPlus:
		Tageszaehler++;
		Gesamtzaehler++;
		break;

	// SoftKey mit Namen  SoftKey_MinusMinus mit ID 5003 wurde losgelassen.
	case SoftKey_MinusMinus:
	// Button mit Namen  Button_MinusMinus mit ID 6003 wurde losgelassen.
	case Button_MinusMinus:
		//bei Zählerstand 0 soll er nicht mehr -- machen.
		if (Tageszaehler > 0)
			Tageszaehler--;
		//bei Zählerstand 0 soll er nicht mehr -- machen.
		if (Gesamtzaehler > 0)
			Gesamtzaehler--;
		break;

	case SoftKey_Reset_Gesamtzaehler:
	case Button_Reset_Gesamtzaehler:
		Gesamtzaehler = 0;
		break;

	case SoftKey_Reset_Tageszaehler:
	case Button_Reset_Tageszaehler:
		Tageszaehler = 0;
		break;

	default:
		break;
	}
	// Senden des Wertes der lokalen Variable Tageszaehler an die NumberVariable_Tageszaehler
	IsoVtcCmd_NumericValue(pButtonData->u8Instance, NumberVariable_Tageszaehler, Tageszaehler);
	// Senden des Wertes der lokalen Variable Gesamtzaehler an die NumberVariable_Gesamtzaehler
	IsoVtcCmd_NumericValue(pButtonData->u8Instance, NumberVariable_Gesamtzaehler, Gesamtzaehler);

	// Speichern des Tageszaehler; non-volatile; nicht flüchtig; spannungsausfallsicher gespeichert
	setU32("CF-A", "Tageszaehler", Tageszaehler);
	// Speichern des Gesamtzaehler; non-volatile; nicht flüchtig; spannungsausfallsicher gespeichert
	setU32("CF-A", "Gesamtzaehler", Gesamtzaehler);

	CheckZiel(pButtonData->u8Instance);
}

void VTC_handleNumericValues(const struct InputNumber_S * pInputNumberData) {
	// what number was entered
	switch (pInputNumberData->objectIdOfInputNumber) {


	case NumberVariable_Gesamtziel:
		// Variable mit dem Namen NumberVariable_Gesamtziel wurde vom Benutzer am UT Verändert
		Gesamtziel = pInputNumberData->newValue;
		ESP_LOGI(TAG, "you typed Gesamtziel: %i", pInputNumberData->newValue); //der eingegebene Wert findet sich in pInputNumberData->newValue
		// Speichern des Gesamtziel; non-volatile; nicht flüchtig; spannungsausfallsicher gespeichert
		setU32("CF-A", "Gesamtziel", Gesamtziel);
		break;

	case NumberVariable_Tagesziel:
		// Variable mit dem Namen NumberVariable_Tagesziel wurde vom Benutzer am UT Verändert
		Tagesziel = pInputNumberData->newValue;
		ESP_LOGI(TAG, "you typed Tagesziel: %i", pInputNumberData->newValue); //der eingegebene Wert findet sich in pInputNumberData->newValue
		// Speichern des Tagesziel; non-volatile; nicht flüchtig; spannungsausfallsicher gespeichert
		setU32("CF-A", "Tagesziel", Tagesziel);
		break;


	default:
		break;
	}

	CheckZiel(pInputNumberData->newValue);
}


void VTC_setNewVT(const ISOVT_EVENT_DATA_T* psEvData)
{
   // ------------------------------------------------------------------------------

   PARAM_UNUSED(psEvData->u8Instance);


   // ------------------------------------------------------------------------------
}

void VTC_setPoolReady(const ISOVT_EVENT_DATA_T* psEvData)
{

	// beim nächsten Anstecken des Gerätes muss der letzte gespeicherte Wert auf das Display gesendet werden.

	// Laden aus dem Spannungsausfallsicheren Speicher ins RAM
	// STANDARD-Wert = 0; wenn nichts abgespeichert.
	Tageszaehler = getU32("CF-A", "Tageszaehler", 0);
	// Laden aus dem Spannungsausfallsicheren Speicher ins RAM
	// STANDARD-Wert = 0; wenn nichts abgespeichert.
	Gesamtzaehler = getU32("CF-A", "Gesamtzaehler", 0);
	// Laden aus dem Spannungsausfallsicheren Speicher ins RAM
	// STANDARD-Wert = 30; wenn nichts abgespeichert.
	Tagesziel = getU32("CF-A", "Tagesziel", 30);
	// Laden aus dem Spannungsausfallsicheren Speicher ins RAM
	// STANDARD-Wert = 80; wenn nichts abgespeichert.
	Gesamtziel = getU32("CF-A", "Gesamtziel", 80);


	// Senden des Wertes der lokalen Variable Tageszaehler an die NumberVariable_Tageszaehler
	IsoVtcCmd_NumericValue(psEvData->u8Instance, NumberVariable_Tageszaehler, Tageszaehler);
	// Senden des Wertes der lokalen Variable Gesamtzaehler an die NumberVariable_Gesamtzaehler
	IsoVtcCmd_NumericValue(psEvData->u8Instance, NumberVariable_Gesamtzaehler, Gesamtzaehler);
	// Senden des Wertes der lokalen Variable Tagesziel an die NumberVariable_Tagesziel
	IsoVtcCmd_NumericValue(psEvData->u8Instance, NumberVariable_Tagesziel, Tagesziel);
	// Senden des Wertes der lokalen Variable Gesamtziel an die NumberVariable_Gesamtziel
	IsoVtcCmd_NumericValue(psEvData->u8Instance, NumberVariable_Gesamtziel, Gesamtziel);



	CheckZiel(psEvData->u8Instance);
}







void VTC_handleSoftkeysAndButtons(const struct ButtonActivation_S *pButtonData)
{
	switch (pButtonData->keyActivationCode)
	{
	case BUTTON_STATE_RELEASED:
		VTC_handleSoftkeysAndButtons_RELEASED(pButtonData);
		break;
	case BUTTON_STATE_PRESSED:
		//BUTTON_InputSignalCallback_PRESSED(pButtonData);
		break;
	case BUTTON_STATE_HELD:
		//BUTTON_InputSignalCallback_HELD(pButtonData);
		break;
	case BUTTON_STATE_ABORTED:
		//BUTTON_InputSignalCallback_ABORTED(pButtonData);
		break;
	}
}

void VTC_handleAux(const struct AUX_InputSignalData_T *InputSignalData) {
	struct ButtonActivation_S pButtonData;


	// Application may decide to lookup the object ID of the function assigned to this input
	// and then check the status data reported...
	//functionObjID = <maybe you have a lookup to get function object Id from (InputSignalData.inputObjectID)>;

	// Store the sprintf format string in HUGE ROM.
	//static SprinthfFormatChar_T format[] = "Pressed = %s, Status= %i\n";


	if (InputSignalData->objectIdOfAuxObject < FIRST_AUX)
	{
		return;
	}



	pButtonData.objectIdOfButtonObject = CommandTranslate[InputSignalData->objectIdOfAuxObject - FIRST_AUX].objectIdOfButtonObject;
	pButtonData.wPara2 = (iso_u16)InputSignalData->value1;
	pButtonData.u8Instance = InputSignalData->u8Instance;

	switch (InputSignalData->value1) {
	case AUX_PRESS_OFF:
		switch (InputSignalData_old_value1[InputSignalData->objectIdOfAuxObject - FIRST_AUX]) {
		case AUX_PRESS_OFF:
			//NOOP!
			break;
		default:
			pButtonData.keyActivationCode = BUTTON_STATE_RELEASED;
			VTC_handleSoftkeysAndButtons(&pButtonData);
			break;
		}
		break;
	case AUX_PRESS_MOMENTARY:
		pButtonData.keyActivationCode = BUTTON_STATE_PRESSED;
		VTC_handleSoftkeysAndButtons(&pButtonData);
		break;
	case AUX_PRESS_HELD:
		switch (InputSignalData_old_value1[InputSignalData->objectIdOfAuxObject - FIRST_AUX]) {
		case AUX_PRESS_OFF:
			pButtonData.keyActivationCode = BUTTON_STATE_PRESSED;
			VTC_handleSoftkeysAndButtons(&pButtonData);
			break;
		default:
			pButtonData.keyActivationCode = BUTTON_STATE_HELD;
			VTC_handleSoftkeysAndButtons(&pButtonData);
			break;
		}
		break;
	default:
		break;

	}

	InputSignalData_old_value1[InputSignalData->objectIdOfAuxObject - FIRST_AUX] = InputSignalData->value1;
}



/* ************************************************************************ */
#endif /* _LAY6_ */
/* ************************************************************************ */
