#include <DataPrecisionGlobals.hh>
#include <OutputProcessor.hh>
#include <SpawnSurgicalTools.hh>

namespace SpawnSurgicalTools {
	Real64 reportingValue; // this is just the integrated value
	Real64 derivative; // this is the current derivative
	Real64 integrateDummyODE( Real64 initialValue, Real64 timestep )
	{
		static bool oneTimeFlag( true );
		if ( oneTimeFlag ) {
			EnergyPlus::SetupOutputVariable( "Spawn Surgery Dummy Integrated Value[]", reportingValue, "System", "Average", "Edwin" );
			EnergyPlus::SetupOutputVariable( "Spawn Surgery Dummy Time Derivative[]", derivative, "System", "Average", "Edwin" );
			oneTimeFlag = false;
		}
		// euler integration
		derivative = getTimeDerivative( initialValue );
		reportingValue = initialValue + derivative * timestep;;
		return reportingValue;
	}

	Real64 getTimeDerivative( Real64 initialValue )
	{
		Real64 A = 0.0005;
		Real64 B = 4;
		return A * initialValue + B;
	}
}
