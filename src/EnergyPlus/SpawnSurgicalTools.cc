#include <DataPrecisionGlobals.hh>
#include <EMSManager.hh>
#include <EnergyPlus.hh>
#include <OutputProcessor.hh>
#include <SpawnSurgicalTools.hh>

namespace SpawnSurgicalTools {

	// Ugly namespace level variables to expedite the process here
	Real64 reportingValue( 0.0 ); // this is just the integrated value
	Real64 derivative( 0.0 ); // this is the current derivative
	bool EMSActive( false );
	Real64 EMSTimeDerivative( 0.0 );

	// Integration function
	Real64 integrateDummyODE( Real64 initialValue, Real64 timestep )
	{
		static bool oneTimeFlag( true );
		if ( oneTimeFlag ) {
			EnergyPlus::SetupOutputVariable( "SPAWN SURGERY DUMMY INTEGRATED VALUE [J]", reportingValue, "SYSTEM", "AVERAGE", "EDWIN" );
			EnergyPlus::SetupOutputVariable( "SPAWN SURGERY DUMMY TIME DERIVATIVE [W]", derivative, "SYSTEM", "AVERAGE", "EDWIN" );
			EnergyPlus::SetupEMSActuator( "BRANCH", "EDWIN", "TIME DERIVATIVE", "[W]", EMSActive, EMSTimeDerivative );
			oneTimeFlag = false;
		}
		// euler integration
		derivative = getTimeDerivative( initialValue );
		reportingValue = initialValue + derivative * timestep;;
		return reportingValue;
	}

	// Time derivative function
	Real64 getTimeDerivative( Real64 initialValue )
	{
		if ( EMSActive ) {
			return EMSTimeDerivative;
		} else {
			Real64 A = 0.0005;
			Real64 B = 4;
			return A * initialValue + B;
		}
	}

}
