Economizer Integration Enhancements
================

**Lixing Gu**

**Florida Solar Energy Center**


 - Design document
 - First revision based on the conference call on 10/20/15 (10/23/15) 
 - Original version on 10/15/15
 - 
 

## Justification for New Feature ##

The EnergyPlus economizer simulation does not align fully with ASHRAE Standard 90.1-2013 requirements for typical DX and variable speed fan systems. Pacific Northwest National Laboratory (PNNL) developed EnergyPlus Energy Management System (EMS) code to better match these requirements from the ASHRAE Standard1. This NFP proposes an approach to implement the same EMS functionality in C++ and adapted to become functionally equivalent internal control capabilities within EnergyPlus.

## E-mail and  Conference Call Conclusions ##

###E-mails###

There are lot of E-mail communications before and after the conference call. It is better to provide summary based on conclusions.

###Conference call###

The first conference call was held on 10/20/15. Mike Witte, Brent Griffith, Reid Hart, Weiming Wang and Lixing Gu attended the conference call.

###Communication Summary###

1 First draft NFP

The first draft NPF mainly presented approaches how to implement the EMS code exactly in EnergyPlus. The review team thought it would be better to take advantages of native code to make better approaches.

2 Consensus

Implementation of same EMS functionality may not be enough to show native code. It is better to implement the code based on original standard and algorithms. 

3 Multiple mode in a single time step

The EMS code assume multiple modes, such as cooling and economizer, may occur in a single time. It is suggested that when a smaller time step is used, a single mode is sufficient. This can be accomplished using small time steps. PNNL agreed that if a smaller system time step is performed, a single mode approach in a time step should be OK. 

4 Fan power calculation

The exponent function of fan power ratio used in the ASHRAE paper can be achieved by exiting Fan:OnOff object. The object has a field to calculate power ration based on an exponential function.     

5 Economizer operation

The EMS code uses the economizer effectiveness regression curves to calculate the maximum outdoor air fraction to comply 90.1 code requirements:

<span style="color:red;">the outdoor air damper is at the 100% open position when mechanical cooling is on, and the outdoor air damper does not begin to close to prevent coil freezing due to minimum compressor run time until the leaving air temperature is less than 45°F.</span>

The team thinks to use native code to accomplish the goal to ensure the minimum leaving air temperature is 45F by using a modified SetpointManager:MixedAir. 
 

###Actions after the first conference call###

Revise approaches to make native code to accomplish the scope.

####Comments received after the first revision####

Nagappan Chidambaram, Trane


Thanks Gu. I think so this is a good change but I see the proposal focusses only on Multi Speed Unitary Object. It would be great if we can extend to built-up  Airloop HVAC objects. I am assuming this would work with Variable Speed DX objects too because the same principle can be applied. 


Gu's reply

It is assumed that AirLoopHVAC:UnitarySystem is able to take over other up  Airloop HVAC objects, so that any effort will be focused on the AirLoopHVAC:UnitarySystem. Regarding your concern of Variable Speed DX objects, since I need to deliver this new feature in Dec., it will be added later. 


## Overview ##

This new feature will implement two major capabilities to calculate fan power more accurately with multiple modes in a single time step and reduces outdoor air flow rate when the temperature at the cooling coil outlet is less than 45F.

1 Economizer operation

The main goal is to meet code requirement defined in Section 6.5.1.3 a of 90.1 to reduce outdoor air flow rate when the temperature at the cooling coil outlet is less than 45F. 

6.5.1.3 
a. Unit controls shall have the mechanical cooling capacity control interlocked with the air economizer controls such that the outdoor air damper is at the 100% open position when mechanical cooling is on, and the outdoor air damper does not begin to close to prevent coil freezing due to minimum compressor run time until the leaving air temperature is less than 45°F.

The EMS code and ASHRAE paper uses economizer effectiveness regression curves to calculate the maximum outdoor air fraction.

After extensive discussion, it is agreed that a modified setpoint manager will do a better job to perform native code way to meet 90.1 requirements. Based on the outlet temperature of a cooling coil or an AirLoop, the mixed air temperature at the outlet of an OA mixer is calculated as a setpoint temperature. Based on the setpoint temperature of the OA mixer outlet, the maximum outdoor air fraction can be calculated. Therefore, no economizer effectiveness regression curves are needed. It is guaranteed that coil freezing can be prevented.

2 Fan power calculation

The existing EMS code calculates fan power use by assuming different modes are applied in a single time step.      

After extensive discussion, it is agreed that if a smaller system time step, such 1 - 6 minutes, is used, it is not necessary to have multiple modes in a single time step. If a Fan:OnOff is used, the existing fan power curve can make power ratio calculation correctly during coil operation. In addition, when a flow rate with no cooling and heating is set correctly, it should provide more accurate power use during coil off time. In this way, a single operation mode is assume in a time step. The existing capability can provide such performance with modifications.

## Approach ##

This section covers approaches to calculate fan power more accurately with multiple modes in a single time step and reduces outdoor air flow rate when the temperature at the cooling coil outlet is less than 45F.
 
1	Economizer operation

The proposed modification of a setpoint manager is SetpointManager:MixedAir. The current object provides a setpoint manager that takes an already established setpoint (usually the supply air outlet node setpoint temperature), subtracts the supply fan heat gain, and applies the result as the setpoint temperature at the mixed air node (or any other node the user specifies). Three optional fields will be added to input cooling coil information and a limit of the cooling coil outlet temperature.

  	A7, \field Cooling Coil Inlet Node Name
 	A8, \field Cooling coil Outlet Node Name
 	N1; \field Minimum Temperature at Cooling Coil Outlet Node

When a draw-thro supply fan is used:  
Setpoint = MAX (  (reference node setpoint  + coil depression), (N1 + coil depression)  )

When a blow-thro supply fan is used:  
Setpoint = MAX (  (reference node setpoint – fan rise + coil depression), (N1 – fan rise + coil depression)  )

The proposed method is simpler and more native code way to meet 90.1 requirements.


2 Fan power calculation

We propose a revision to make more accurate fan power calculation by adding a new filed in the  UnitarySystemPerformance:Multispeed object called by the AirLoopHVAC:UnitarySystem object. The main function is to ensure a single mode operation is applied with given speed number, so that fan power calculation during cooling operation mode is accomplished. The existing code calculate fan power based on two adjacent speeds. 

Multiple speed coils

When zone loads vary with time, it is better to have multiple speed coil to provide energy efficient HVAC operation. The current coil control is based on two categories. If the zone load is less than lowest speed capacity, the coil will be on/off to meet loads by keeping the same fan flow rate, similar performance to a single mode operation coil. When the zone load is above the minimum capacity, the coil will require two consecutive speed operation. The proposed change will allow each speed has its independent operation with given speed number as single mode operation. For example, when the zone load is between capacities between speed 2 and speed 3, the speed 3 alone operation is called.   


## Testing/Validation/Data Sources ##

Compare simulation results with spread sheet calculations.

## Input Output Reference Documentation ##

insert text

## Input Description ##

### Existing objects ###

Revisions to the IDD are noted as **<span style="color:red;">bold red</span>** non-blocked insertions at the appropriate location throughout the input data dictionary description. 

1	SetpointManager:MixedAir

Three optional fields are added to set the OA mixer outlet temperature to prevent cooling coil freezing.
 
	SetpointManager:MixedAir,
     \memo The Mixed Air Setpoint Manager is meant to be used in conjunction
     \memo with a Controller:OutdoorAir object. This setpoint manager is used
     \memo to establish a temperature setpoint at the mixed air node.
  	A1 , \field Name
       \required-field
  	A2 , \field Control Variable
       \type choice
       \key Temperature
       \default Temperature
  	A3 , \field Reference Setpoint Node Name
       \required-field
       \type node
  	A4 , \field Fan Inlet Node Name
       \required-field
       \type node
  	A5 , \field Fan Outlet Node Name
       \required-field
       \type node
  	A6 , \field Setpoint Node or NodeList Name
       \required-field
       \type node
       \note Node(s) at which the temperature will be set
 <span style="color:red;">**A7 , \field Cooling Coil Inlet Node Name**</span>

       \type node
 <span style="color:red;">**A8 , \field Cooling coil Outlet Node Name**</span>

       \type node
 <span style="color:red;">**N1 ; \field Minimum Temperature at Cooling Coil Outlet Node**</span>

        \type real
        \units C
        \minimum> 0.0
        \default 7.2

 
Output:

When a draw-thro supply fan is used:  
Setpoint = MAX (  (reference node setpoint  + coil depression), (N1 + coil depression)  )

When a blow-thro supply fan is used:  
Setpoint = MAX (  (reference node setpoint – fan rise + coil depression), (N1 – fan rise + coil depression)  )

2 UnitarySystemPerformance:Multispeed

A new optional field is added to ensure a single mode operation at any given speed number. The object is called by the AirLoopHVAC:UnitarySystem object. 

	UnitarySystemPerformance:Multispeed,
       \memo The UnitarySystemPerformance object is used to specify the air flow ratio at each
       \memo operating speed. This object is primarily used for multispeed DX and water coils to allow
       \memo operation at alternate flow rates different from those specified in the coil object.
       \extensible:2 - repeat last two fields, remembering to remove ; from "inner" fields.
  	A1 , \field Name
       \required-field
       \reference UnitarySystemPerformaceNames
  	N1 , \field Number of Speeds for Heating
       \required-field
       \type integer
       \minimum 0
       \maximum 10
       \note Used only for Multi speed coils
       \note Enter the number of the following sets of data for air flow rates.
  	N2 , \field Number of Speeds for Cooling
       \required-field
       \type integer
       \minimum 0
       \maximum 10
       \note Used only for Multi speed coils
       \note Enter the number of the following sets of data for air flow rates.

<span style="color:red;">**A2 , \field Single Mode Operation**</span>

       \type choice
       \key Yes
       \key No
       \default No
  	N3 , \field Heating Speed 1 Supply Air Flow Ratio
       \required-field
       \type real
       \autosizable
       \minimum> 0
       \begin-extensible
       \note Used only for Multi speed coils
       \note Enter the lowest operating supply air flow ratio during heating
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N4 , \field Cooling Speed 1 Supply Air Flow Ratio
       \required-field
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the lowest operating supply air flow ratio during cooling
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N5 , \field Heating Speed 2 Supply Air Flow Ratio
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the next highest operating supply air flow ratio during heating
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N6 , \field Cooling Speed 2 Supply Air Flow Ratio
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the next highest operating supply air flow ratio during cooling
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N7 , \field Heating Speed 3 Supply Air Flow Ratio
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the next highest operating supply air flow ratio during heating
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N8 , \field Cooling Speed 3 Supply Air Flow Ratio
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the next highest operating supply air flow ratio during cooling
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N9 , \field Heating Speed 4 Supply Air Flow Ratio
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the next highest operating supply air flow ratio during heating
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.
  	N10; \field Cooling Speed 4 Supply Air Flow Ratio
       \type real
       \autosizable
       \minimum> 0
       \note Used only for Multi speed coils
       \note Enter the next highest operating supply air flow ratio during cooling
       \note operation or specify autosize. This value is the ratio of air flow
       \note at this speed to the maximum air flow rate.

if a test example may not work properly with multiple coils under AirLoopHVAC:UnitarySystem before modification, AirLoopHVAC:UnitaryHeatPump:AirToAir:MultiSpeed with mutlispeed coils will be used to test single mode operation. Possible bug fix effort should not be included in this new feature effort.  


## Outputs Description ##

insert text

## Engineering Reference ##

insert text


## Example File and Transition Changes ##

insert text

##Design Document##

This new feature will revise several modules: SetpointManager, MixedAir, HVACUnitarySystem and DXCoils. 

###SetpointManager###

The revision is focused on SetpointManager:MixedAir.

####Add variables in SetpointManager:MixedAir####

Since three new fields are proposed to meet requirements of cooling coil outlet temperature, three new variables are added in the struct DefineMixedAirSetPointManager:

	struct DefineMixedAirSetPointManager
	{
		...
		int CoolCoilInNode; // Cooling coil inlet node number
		int CoolCoilOutNode; // Cooling coil outlet node number
		Real64 MinCoolCoilOutTemp; // The minimum temperature at cooling coil outlet node
		...
	};


####Revise DefineMixedAirSetPointManager::calculate####

When the inputs of cooling coil nodes are given, the mixed air setpoint is calculated based on fan location:

	If draw-through then  
		Setpoint = MAX (  (reference node setpoint  + coil depression), (N1 + coil depression)  )
	Else (blow through)
		Setpoint = MAX (  (reference node setpoint – fan rise + coil depression), (N1 – fan rise + coil depression)  )
	End If

It should be pointed out that the fan placement can be found by comparing the temperature difference between the reference node and cooling coil outlet node. When cooling is required and the temperature difference is zero, the fan placement is blow through, and vice versa.


###MixedAir###

The revision is focused on the OA Controller. The maximum amount of outdoor air will be calculated based on the mixed air setpoint when the cooling coil outlet temperature is less than the setpoint temperature. The Maximum Fraction of Outdoor Air in the Controller:OutdoorAir object will be revised based on the maximum amount of outdoor air.

###DXCoils###

In order to perform single mode operation, an optional argument will be added in the SimDXCoilMultiSpeed function. 

	void
	SimDXCoilMultiSpeed(
		std::string const & CompName, // name of the fan coil unit
		Real64 const SpeedRatio, // = (CompressorSpeed - CompressorSpeedMin) /
		Real64 const CycRatio, // cycling part load ratio for variable speed
		int & CompIndex,
		Optional_int_const SpeedNum, // Speed number for multispeed cooling coil onlyn
		Optional_int_const FanOpMode, // Fan operation mode
		Optional_int_const CompOp, // Compressor on/off; 1=on, 0=off

<span style="color:red;">Optional_bool_const SingleModeFlag // Single mode operation </span>

	)

The additional argument will be carried by two sub-functions: CalcMultiSpeedDXCoilCooling and CalcMultiSpeedDXCoilHeating.  

	void
	CalcMultiSpeedDXCoilCooling(
		int const DXCoilNum, // the number of the DX heating coil to be simulated
		Real64 const SpeedRatio, // = (CompressorSpeed - CompressorSpeedMin) / (CompressorSpeedMax - CompressorSpeedMin)
		Real64 const CycRatio, // cycling part load ratio
		int const SpeedNum, // Speed number
		int const FanOpMode, // Sets fan control to CycFanCycCoil or ContFanCycCoil
		int const CompOp, // Compressor on/off; 1=on, 0=off
<span style="color:red;">Optional_bool_const SingleModeFlag // Single mode operation </span>

	)

	void
	CalcMultiSpeedDXCoilHeating(
		int const DXCoilNum, // the number of the DX heating coil to be simulated
		Real64 const SpeedRatio, // = (CompressorSpeed - CompressorSpeedMin) / (CompressorSpeedMax - CompressorSpeedMin)
		Real64 const CycRatio, // cycling part load ratio
		int const SpeedNum, // Speed number
		int const FanOpMode // Fan operation mode
<span style="color:red;">Optional_bool_const SingleModeFlag // Single mode operation </span>

	)
 
The calculation procedures are provided below:

If ( SingleModeFlag ) {

	SpeedRatio = 1.0
	SpeedNumer is given and fixed
	CycRatio varies

Load = SensibleCapacity(SpeedNume) * CycRatio

}

Note: When the speed number is equal to 1, the original calculation is kept. When the load is greater than the capacity of the highest speed, no change will be made.  

###AirLoopHVAC:UnitarySystem###

The main purpose is to read a new optional field of single mode operation and pass it into the sub-functions. 

An additional field of Single Mode Operation will be added in the UnitarySystemPerformance:Multispeed object to accommodate the single mode operation. If this field is entered as Yes, a single mode operation is required with the fixed speed number. The performance is similar to a single capacity cooling coil whose capacity is determined by the speed number. In this way, the fan power calculation can be achieved by using exponent curve based on fan flow ratio.

Based on the additional field, a bool variable is added in the struct UnitarySystemData  

	struct UnitarySystemData 
	{
		....
		bool SingleModeFlag; // Single mode operation
		....
	};

####ControlUnitarySystemOutput####

One more dimension is added in the Par variable to have information on single mode operation.

				Par( 12 ) = 0.0; // FLAG, 0.0 with No single mode, 1.0 with single mode operation

####CalcUnitarySystemLoadResidual####

Add an optional argument to represent a single mode operation

		CalcUnitarySystemToLoad( UnitarySysNum, AirLoopNum, FirstHVACIteration, CoolPLR, HeatPLR, OnOffAirFlowRatio, SensOutput, LatOutput, HXUnitOn, _, _, CompOp, SingleModeFlag );

The optional argument of SingleModeFlag will be passed into the following two functions: CalcUnitaryCoolingSystem and CalcUnitaryHeatingSystem. The optional argument is only valid for two type of coils: CoilDX_MultiSpeedCooling and CoilDX_MultiSpeedHeating. When these two functions call a function of SimDXCoilMultiSpeed in the DXCoils, the optional argument is passed in the SimDXCoilMultiSpeed function to perform single mode operation with given speed number.  
 

## References ##

PNNL ENergyPlus input file: ASHRAE90.1_RetailStandalone_STD2013_Chicago.idf

Reid Hart, PE; Rahul Athalye; Weimin Wang, Ph.D, 2013. "Improving Simulation of Outside Air Economizer and Fan Control for Unitary Air Conditioners," DE-13-C058, ASHRAE Transactions




