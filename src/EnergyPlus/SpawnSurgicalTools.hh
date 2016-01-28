#ifndef SPAWN_SURGICAL_TOOLS_HH
#define SPAWN_SURGICAL_TOOLS_HH

#include <DataPrecisionGlobals.hh>

namespace SpawnSurgicalTools {
	Real64 integrateDummyODE( Real64 initialValue, Real64 timestep );
	Real64 getTimeDerivative();
}

#endif //SPAWN_SURGICAL_TOOLS_HH
