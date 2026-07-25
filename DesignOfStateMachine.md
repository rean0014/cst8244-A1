**(Open with Notepad to see tables)**



**Design of State Machine:**



Inputs:

|**EVENT**|**MEANING**|**EXTRA DATA CARRIED**|
|-|-|-|
|ls|left scan|person\_id|
|rs|right scan|person\_id|
|ws|weight scale reading|weight|
|lo|left door opened (sensor)|--|
|ro|right door opened (sensor)|--|
|lc|left door closed (sensor)|--|
|rc|right door closed (sensor)|--|
|glu|guard unlocks left|--|
|gll|guard locks left|--|
|gru|guard unlocks right|--|
|grl|guard locks right|--|



Outputs:

* left\_locked, left open: Booleans for left door status.
* right\_locked, right\_open: Booleans for right door status.
* lock\_occupied: True whenever current\_state != IDLE.
* current\_person\_id, current\_weight
* String for status messages.



Conditions/events causing transitions :

ST=Start · ID=IDLE · SC=SCANNED · AU=A\_UNLOCKED · AO=A\_OPEN · WE=WEIGHED · AC=A\_CLOSED · AL=A\_LOCKED · BU=B\_UNLOCKED · BO=B\_OPEN · BC=B\_CLOSED

|**FROM/TO**|**ST**|**ID**|**SC**|**AU**|**AO**|**WE**|**AC**|**AL**|**BU**|**BO**|**BC**|
|-|-|-|-|-|-|-|-|-|-|-|-|
|**ST**|-|default|-|-|-|-|-|-|-|-|-|
|**ID**|-|-|ls/rs|-|-|-|-|-|-|-|-|
|**SC**|-|-|-|glu/gru|-|-|-|-|-|-|-|
|**AU**|-|-|-|-|lo/ro|-|-|-|-|-|-|
|**AO**|-|-|-|-|-|ws|-|-|-|-|-|
|**WE**|-|-|-|-|-|-|lc/rc|-|-|-|-|
|**AC**|-|-|-|-|-|-|-|gll/grl|-|-|-|
|**AL**|-|-|-|-|-|-|-|-|gru/glu|-|-|
|**BU**|-|-|-|-|-|-|-|-|-|ro/lo|-|
|**BO**|-|-|-|-|-|-|-|-|-|-|rc/lc|
|**BC**|-|grl/gll|-|-|-|-|-|-|-|-|-|





States:

IDLE, OPENING\_A, WIGHED, CLOSING\_A, OPENING\_B, CLOSING\_B

(6 states, 6 handlers functions, each returning a function pointer to the next handler once its exit condition is met).

