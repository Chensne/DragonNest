//99. 清空指定面板
//containerID  面板ID


function AvailableUserNickCallBack(taskid, sid, lasterror, statecode, response) {
        var http = window.external.System.IO.Http;
        var existtask = http.FindTask(taskid);

        var retInfo = response.getJSON();

        if (retInfo.ResultNo == 0) {

            var retobject = retInfo.ResultAttachObject;
            GOftenNameMgr.genericoOftenNameSet(retobject);
        }
        else {
            if (retInfo.ResultNo == -16777217) {
                ExternalIA.GoUserLogin();
                preNeedDealWork = ExternalIA.GoNameReg();
            }
        }
       
  

    }


    function RequestTokenCallBack(taskid, sid, lasterror, statecode, response) {
    }

    function SetNickNameCallBack(taskid, sid, lasterror, statecode, response) {

        var http = window.external.System.IO.Http;
        var existtask = http.FindTask(taskid);

        var retInfo = response.getJSON();
        if (retInfo.ResultNo == 0) {

            // UserModel.setname(sid);
            ExternalIA.UpdateUserInfo();


            if (!Setting.currentIsUserCenter()) {
                CurrentLoginUserNickName = UserModel.getname();
                var items = GOftenNameMgr.fillConfirmUserInfo();


                ExternalIA.GoGmainPage(ExternalIA.confirmMessage(items, "填写个人资料", function() {
                    var QQ = items.find("input[name=textfield2]").val().Trim();

                    var call = items.find("input[name=textfield22]").val().Trim();


                    ExternalIA.UpdateCurrentUser(QQ, call);
                    ExternalIA.boxclose();

                }, function() { ExternalIA.boxclose(); }));
            } else {
                $(".login").find("li").eq(0).html(sid);
                ExternalIA.boxclose();
            }


        }
        else {

            ExternalIA.boxclose();
            CurrentLoginUserNickName = window.external.GPlus.Client.User.GetNickName();

            GOftenNameMgr.regmessage(retInfo.ResultDescription);
        }




        //    if (ExternalIA.IsHaveName()) {


        //        if (!Setting.currentIsUserCenter()) {
        //            var items = GOftenNameMgr.fillConfirmUserInfo();


        //            ExternalIA.GoGmainPage(ExternalIA.confirmMessage(items, "填写个人资料", function() {
        //                var QQ = items.find("input[name=textfield2]").val();
        //               
        //                var call = items.find("input[name=textfield22]").val();
        //               

        //                ExternalIA.UpdateCurrentUser(QQ, call);
        //                ExternalIA.boxclose();

        //            }, function() { ExternalIA.boxclose(); }));
        //        } else {
        //        ExternalIA.boxclose();
        //        }

        //    } else {
        //        ExternalIA.GoGmainPage();
        //    }


    }

function addfeedbackCallBack(taskid, sid, lasterror, statecode, response) {
    ExternalIA.feedbackDialog(setandfeed.feedbackSuccess(), function() {
        ExternalIA.GoFeedBack();


    }, 3000);
}

function UpdateCurrentUserCallBack(taskid, sid, lasterror, statecode, response) {

  
     var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);

    var retInfo = response.getJSON();
    if (retInfo.ResultNo == 0) {
      
        var info = sid.getJSON();
        var call = info.mobile;
        var QQ = info.qq;
      
        if (Setting.currentIsUserCenter()) {
            $("span[name=qqnum]").html(QQ);
            $("span[name=callnum]").html(call);
        }
       // UserModel.setcall(call);
        // UserModel.setqq(QQ);
        ExternalIA.UpdateUserInfo();
    } else { 
    }
}

function GetMessageListNickCallBack(taskid, sid, lasterror, statecode, response) {

    var retobject = "";
    var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);

    var retInfo = response.getJSON();
    if (retInfo.ResultNo == 0) {
        retobject = retInfo.ResultAttachObject.Items;
       

    }
    else {
        retobject = null;
    }
    GIndexMgr.indexLoad(retobject); //G首页
}

function GetMessageListCallBack(taskid, sid, lasterror, statecode, response) {
   
    var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);
    var retInfo = response.getJSON();
    var retobject = "";
    if (retInfo.ResultNo == 0) {
        if (retInfo.ResultAttachObject.TotalCount % 4 == 0) {
            MessageMgr.pagecout = Math.floor(retInfo.ResultAttachObject.TotalCount / 4);
        } else {
        MessageMgr.pagecout = Math.floor(retInfo.ResultAttachObject.TotalCount / 4)+1;
        }
        retobject = retInfo.ResultAttachObject.Items;


    }
    else {
        retobject = null;
    }
    MessageMgr.messageappend(retobject);




}

function GetMessageCallBackApeend(taskid, sid, lasterror, statecode, response) {
 
    var imghead = -1;
    var imgend = -1;
    var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);

    var retInfo = response.getJSON();
    var retobject = "";
    if (retInfo.ResultNo == 0) {
        retobject = retInfo.ResultAttachObject;


    }
    else {
        retobject = null;
    }
    var content = retobject.Content;
     imghead=content.indexOf("http");
     imgend = content.indexOf(".JPG");
    
     if (imghead > -1 && imgend > -1) {

         var messageid = sid;
         var img = content.substring(imghead, imgend + 4);
        
         MessageMgr.appendmessageimage(img, messageid);
     }
}

function UpdateUserSettingsCallBack(taskid, sid, lasterror, statecode, response) {
    ExternalIA.UpdateUserSettingsWin();
}

function GetMessageCallBack(taskid, sid, lasterror, statecode, response) {
    var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);

    var retInfo = response.getJSON();
    var retobject = "";
   
        if (retInfo.ResultNo == 0) {
            retobject = retInfo.ResultAttachObject;
           

        }
        else {
            retobject = null;
        }

        if (retobject) {
            Other_ClearContainer();
            MessageMgr.curentstat = 1;
            MessageMgr.genericappendmessagedetail(retobject);
        } else { 
        }
}


function UserLoginCallBack(taskid, sid, lasterror, statecode, response) {
    //SessionToken = retString.ResultAttachObject.SessionToken;
    // CurrentLoginUserAccountID = retString.ResultAttachObject.UserID;NickName

   // var Error = ConnectAbnormal();
  //  if (Error == "成功") {
        var http = window.external.System.IO.Http;
        var existtask = http.FindTask(taskid);

        var retInfo = response.getJSON();
        if (retInfo.ResultNo == 0) {
            CurrentSessionToken = retInfo.ResultAttachObject.SessionToken;
            CurrentLoginUserAccountID = retInfo.ResultAttachObject.UserID;
            Other_Init();
        }
        else {
            alert(retInfo.InnerAOPResult);
        }


//    }
//    else {
//        alert(Error);
//    }
    
}

function GetAccessTokenCallBack(taskid, sid, lasterror, statecode, response) {
    //    var Error = ConnectAbnormal();
    //    if (Error == "成功") {
    var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);

    var retInfo = response.getJSON();
    if (retInfo.ResultNo == 0) {
        CurrentAccessToken = retInfo.ResultAttachObject.length > 0 ? retInfo.ResultAttachObject : "";
        ExternalIA.GetLogin();
    }
    else {
        alert(retInfo.InnerAOPResult);
    }


    //    }
    //    else {
    //        alert(Error);
    //    }
}


function GetUserSettingsCallBack(taskid, sid, lasterror, statecode, response) {

    var http = window.external.System.IO.Http;
    var existtask = http.FindTask(taskid);

    var retInfo = response.getJSON();
    var retobject = "";
    if (retInfo.ResultNo == 0) {
        retobject = retInfo.ResultAttachObject;


    }
    else {
        retobject = null;
    }


    setandfeed.loaduserset();

}

function ConnectAbnormal(lasterror,statecode) {
    if (lasterror != "0") {
        return "本地网络异常";

    }
    if (statecode != "200") {
        return "连接远程服务异常";
    }
    return "成功";
}


function Other_Init() {

 //   Other_SetCurrentRenderPage(pageID);
    
 //   Setting.init();
  //  ExternalIA.renderNavigate(pageID);

    if (preNeedDealWork) {
        if ($.isFunction(preNeedDealWork)) {
           // if (ExternalIA.isLogin()) {
            preNeedDealWork();
           
              
       //    }

            preNeedDealWork = null;
           
        }
    }
    if (nextcallback) {
        if ($.isFunction(nextcallback)) {
            // if (ExternalIA.isLogin()) {
            nextcallback();

            //    }

            
            nextcallback = null;
        }
    }
}
