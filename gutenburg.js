var XMLHttp;

function lookup(){
	if(navigator.appName == "Microsoft Internet Explorer") {
		XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
    } else {
		XMLHttp = new XMLHttpRequest();
    }
	
	var search = document.getElementById('search').value;
	
	console.log(search);
	
	XMLHttp.open("GET", "/cgi-bin/ogilviethompsonh_gutenburgAjax.cgi?"
						 + "&search=" + search
						 ,true);
						
	console.log('called cgi');
						
	XMLHttp.onreadystatechange=function(){
		if(XMLHttp.readyState == 4){
			document.getElementById('searchresults').innerHTML = XMLHttp.responseText;
		}
	}
	XMLHttp.send(null);
}

