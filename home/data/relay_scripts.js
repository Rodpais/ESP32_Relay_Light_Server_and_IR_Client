function turnOnRelay(relay){
  requst = 
  if (relay == 1){
    buttonStateChange(relay);
  }
  if (relay == 2){
    buttonStateChange(relay);
  }
  if (relay == 3){
    buttonStateChange(relay);
  }
  if (relay == 4){
    buttonStateChange(relay);
  }
  if (relay == "all"){
    buttonStateChange(relay);
  }
}

function buttonStateChange(relay){

  if(relay == 1){
    if ($("#relay1").text() == "On"){
      $("#relay1").html("Off");
    }
    else if ($("#relay1").text() == "Off"){
      $("#relay1").html("On");
    }
  }

  else if(relay == 2){
    if ($("#relay2").text() == "On"){
      $("#relay2").html("Off");
    }
    else if ($("#relay2").text() == "Off"){
      $("#relay2").html("On");
    }
  }

  else if(relay == 3){
    if ($("#relay3").text() == "On"){
      $("#relay3").html("Off");
    }
    else if ($("#relay3").text() == "Off"){
      $("#relay3").html("On");
    }
  }

  else if(relay == 4){
    if ($("#relay4").text() == "On"){
      $("#relay4").html("Off");
    }
    else if ($("#relay4").text() == "Off"){
      $("#relay4").html("On");
    }
  }

  else if(relay == "all"){
    if ($("#totalControl").text() == "All On"){
      $("#totalControl").html("All Off");
    }
    else if ($("#totalControl").text() == "All Off"){
      $("#totalControl").html("All On");
    }
  }
}


