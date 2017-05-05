var XMLhttp;


function init() {
    if(navigator.appName == "Microsoft Internet Explorer") {
	XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
    } else {
	XMLHttp = new XMLHttpRequest();
    }
	
}

function lookup(){
	
	var search = document.getElementById('search').value;
	XMLHttp.open("GET", "/cgi-bin/iduma1_gutenburg.cgi?"
						 + "&search=" + search
						 ,true);
						 
	XMLHttp.onreadystatechange=function(){
		if(XMLHttp.readyState == 4){
			var response = XMLHttp.responseText
			if (response === "Sorry, but that username has already been taken"){
				document.getElementById('searchresults').innerHTML = response;
				
			}
		}
	}
	XMLHttp.send(null);
}

