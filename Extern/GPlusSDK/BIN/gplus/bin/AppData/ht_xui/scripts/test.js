function CallScripts(msg,callBack) {
    if (msg)
        alert("触发了js 函数CallScripts,传入参数" + msg);
    else {
        alert("触发了js 函数CallScripts");
    }

    //回调
    if (callBack) {
        try {
            callBack(msg);
        }
        catch (e) {
            alert("调用回调函数异常,"+e);
        }
    }
    else {
        alert("未定义回调函数");
    }
}
//c9ec8503f5164d69accc1f2bec8a3c71
//"83276647"UserAccountId=10001

var curUserID = "10172";  //"37eafa6bf4224a06bdca851ff23840f9";  //"10129"; //"954310878";

try {
    $(function() {
        CurrentLoginUserAccountID = curUserID;
    });
}
catch (e) {
}






function getdata() {
//    var appno = "sandbox";
//    var timestamp = currenttime();
//    var secretKey = "123456";
//    var signedString = $.md5("appNo=sandbox&timestamp=" + timestamp + "&secretKey=123456");
//  
//    var data = "{ appNo: \"" + appno + "\", timestamp: \"" + timestamp + "\", signedString: \"" + signedString + "\" }";
    //    // var data = "{ accessToken: \"332b12ee47674bd99c4675aef480747a\", authProvider: \"snda\"}";
    
    ExternalIA.GoUserLogin();


   
}




function GetGetWonderfulPhotoType() {
   // GetData("Blog", "GetWonderfulPhotoType", "", "", "");
}

//function GetData(interfaceName, CommandName, takeCount, StartTick, callBack, extre, containerID) {
function GetData(curr_url,app, extre, callBack, containerID, isasync,isdefault) {


    var httpurl = app ? $.format(curr_url, app) : curr_url;
    var Parameter = "";
    if (!extre)
        extre = "";
    if (isdefault) {
         Parameter = "{accessToken: \"" + CurrentAccessToken + "\", sessionToken: \"" + CurrentSessionToken + "\"" + extre + "}";
    }
    else {
        Parameter = extre;
    }
    
    var getdata = "";
   //  var url = URLEncode("http://10.240.248.79:7077/ClientInterface.axd?Interface=" + interfaceName + "&SerializationType=Json&SessionId=" + curUserID); //116.211.2.18:83-116.211.2.21:83
  //  var url = URLEncode("http://localhost:12419/ClientInterface.axd?Interface=" + interfaceName + "&SerializationType=Json&SessionId=" + curUserID); //116.211.2.18:83-116.211.2.21:83
 //   var url = URLEncode("http://116.211.2.17:83/ClientInterface.axd?Interface=" + interfaceName + "&SerializationType=Json&SessionId=31c220e2008b4917b6aaf61f942a636e");
   // var url = URLEncode("http://auth.esb.newkoo.com/Services/ApplicationAuthenticationService/Authenticate");
    var url = URLEncode(httpurl);


    $.ajax({
    async: isasync,
        type: "post",
        dataType: "json",
        contentType: "application/json",
        url: "/ajax/StreamingProxy.ashx?u=" + url,

        data: Parameter,

        success: function(data) {
          
            if ($.isFunction(callBack))
                callBack(data, containerID);
            else
                getdata = data;

            //   ExtreGlobalWork();
        },
        error: function(data) {
            return null;
        }

    });
   
    return getdata;
}


function URLEncode (clearString)
{
    var output = '';
    var x = 0;
    clearString = clearString.toString();
    var regex = /(^[a-zA-Z0-9_.]*)/;
    while (x < clearString.length)
    {
        var match = regex.exec(clearString.substr(x));
        if (match != null && match.length > 1 && match[1] != '')
        {
            output += match[1];
            x += match[1].length;
        } 
        else
        {
            if (clearString[x] == ' ')
            {
                output += '+';
            }
            else
            {
                var charCode = clearString.charCodeAt(x);
                var hexVal = charCode.toString(16);
                output += '%' + ( hexVal.length < 2 ? '0' : '' ) + hexVal.toUpperCase();
            }
            x++;
        }
    }
    return output;
}

function GoUserBackTest() {
   // Other_ClearContainer();

    //GetBlogList(5, "", BlogMgr.appendOld);
    getdata();
  //  userctrMgr.fillUserBackPage();

}
function GoSetPageTest() {
    getdata();
  //  Other_ClearContainer();

    //GetBlogList(5, "", BlogMgr.appendOld);
  //  userctrMgr.fillSetPagePage();

}


function GoUserCenterTest() {
    getdata();

   // Other_ClearContainer();
  //  userctrMgr.fillUserCenter();
    
}


