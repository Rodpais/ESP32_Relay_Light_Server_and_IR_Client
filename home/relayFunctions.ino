// Get the state of the relay.... Will need to write this to a JSON file. 
byte getRelayStatus(int relay){

  if( relay < 1 || relay > 4) 
    return INCORR_PARAM; 

  int relay_state = quadRelay.getState(relay);
  return relay_state; 
  
}

byte turnOnRelay(int relay){
  
  if( relay < 1 || relay > 5) 
    return INCORR_PARAM; 

  quadRelay.turnRelayOn(relay);
  int relay_state = quadRelay.getState(relay);
  // Write JSON here....

  if (relay == 5){
    quadRelay.turnAllRelaysOn();
    return SUCCESS;
  }
  
  return SUCCESS;
}

byte turnOffRelay(int relay){

  if( relay < 1 || relay > 5) 
    return INCORR_PARAM; 

  quadRelay.turnRelayOff(relay);
  int relay_state = quadRelay.getState(relay);
  // Write JSON here...

  if (relay == 5){
    quadRelay.turnAllRelaysOff();
    return SUCCESS;
  }

  return SUCCESS;
}
