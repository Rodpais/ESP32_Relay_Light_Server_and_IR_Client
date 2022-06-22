function turnOnRelay(relay){
  
  if (relay < 1 || relay > 5){ 
    console.log("Parametro Incorreto.");
    return;
  }

  console.log("Talvez liguemos um relay.");
  request =  new XMLHttpRequest();   // Starts request
  home_page = "/relay.html"
  request_data = home_page + "/RELAY_" + relay + "_ON"; // Formats the request - remember it's all text
  console.log(request_data);
  // request.open(method, url, async);
  request.open("GET", request_data, true); // "GET" command ran in background
  request.send(null);                     // Sends request
  console.log("AJAX Request Sent.");
  buttonStateChange(relay);

}

function buttonStateChange(relay){

  if (relay < 1 || relay > 5){ 
    console.log("Parametro Incorreto.");
    return; 
  } 
 
  id_text = "#relay" + relay;

  if (relay <= 4){
    if ($(id_text).text() == "On"){
      $(id_text).html("Off");
    }
    else if ($(id_text).text() == "Off"){
      $(id_text).html("On");
    }
  }

  else {
    if ($("#totalControl").text() == "Tudo On"){
      $("#totalControl").html("Tudo Off");
    }
    else if ($("#totalControl").text() == "Tudo Off"){
      $("#totalControl").html("Tudo On");
    }
  }
}

