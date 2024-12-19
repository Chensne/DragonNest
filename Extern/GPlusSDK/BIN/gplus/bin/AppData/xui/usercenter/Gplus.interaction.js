var ifdebug = false; //
var CurrentAccessToken = "";
var CurrentSessionToken = "";
var CurrentLoginUserAccountID = "";
var CurrentLoginUserNickName = "";
var CurrentgameNo = "";
var CurrentSeting="";
var IsFirst = true;
var appno = "sandbox";
var secretKey = "123456";
var userser = "http://users.esb.newkoo.com/Services/UserService/{0}";
var passportser="http://passport.esb.newkoo.com/Services/PassportService/{0}";
var authser = "http://auth.esb.newkoo.com/Services/ApplicationAuthenticationService/Authenticate";
var message = "http://messages.esb.newkoo.com/Services/MessageService/{0}";
var userset = "http://settings.esb.newkoo.com/Services/SettingsService/{0}"
var feedback = "http://10.240.248.103:10205/Services/FeedbackService/{0}";
var growthservice = "http://10.240.248.103:10206/Services/growthservice/{0}";
var validation = "http://validation.esb.newkoo.com/Services/ValidationService/{0}"; 
var Global_CurrentRenderPageID = "";   //当前显示页面
var ExternalIA =
{

    isLogin: function() {
        //    if (SessionId != "")

        if (CurrentAccessToken && CurrentSessionToken)
            return true;

        return false;
    },
    GoUserCenter: function(index, length, categories, callback) {
        Other_ClearContainer();

        var data = "";
        if (index) {
            data += ",index:" + index;
        }
        else {
            // data += ",index:1";
            data += ",index:" + MessageMgr.page;
        }
        if (length) {
            data += ",length:" + length;
        }
        else {
            data += ",length:3";
        }
        if (categories) {
            data += ",categories:" + categories;
        }
        else {
            data += ",categories:null";
        }

        CallFunWithCondition(function() {
            GIndexMgr.indexLoad(); //G首页
        },
            function() {


                //   GIndexMgr.indexLoad(); //G首页
                ExternalIA.SendRequest(message, "GetMessageList", data, "GetMessageListNickCallBack", "", true);
                //  ExternalIA.SendRequest(message, "GetMessageCount", data, "GetMessageListNickCallBack", "", true);


            },
            ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_UserCenter); });
    },
    GoGmainPage: function(index, length, categories, callback) {
        Other_ClearContainer();
        var data = "";
        if (index) {
            data += ",index:" + index;
        }
        else {
            // data += ",index:1";
            data += ",index:" + MessageMgr.page;
        }
        if (length) {
            data += ",length:" + length;
        }
        else {
            data += ",length:3";
        }
        if (categories) {
            data += ",categories:" + categories;
        }
        else {
            data += ",categories:null";
        }
        CallFunWithCondition(function() {



            // ExternalIA.SendRequest(message, "GetMessageList", data, "GetMessageListCallBack", "");
            //GetData(message, "GetMessageList", data, GetMessageListNickCallBack, "", true, true);
            GetData(message, "GetMessageCount", data, GetMessageListNickCallBack, "", true, true);



        },
            function() {
                Other_ClearContainer();


                //   GIndexMgr.indexLoad("", callback); //G首页
                ExternalIA.SendRequest(message, "GetMessageList", data, "GetMessageListNickCallBack", "", true);
                //  ExternalIA.SendRequest(message, "GetMessageCount", data, "GetMessageListNickCallBack", "", true);



            },
            ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_Home); });

    },
    GoNameReg: function(nickname) {

        Other_ClearContainer();
        var data = "";
        if (nickname) {
            data += ",nickname:\"" + nickname + "\"";
        }
        CallFunWithCondition(function() {

            GOftenNameMgr.genericoOftenNameSet(null);
            //  GetData(userser, "GetNickNames", data, AvailableUserNickCallBack, "", true, true);
        },
            function() {


                //    GOftenNameMgr.genericoOftenNameSet();
                ExternalIA.SendRequest(userser, "GetNickNames", data, "AvailableUserNickCallBack", "", true);



            },
            ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_NameReg); });
    },


    GoSetPage: function() {
        Other_ClearContainer();
        var data = "";

        data += ",gameNo:\"" + CurrentgameNo + "\"";

        CallFunWithCondition(function() {
            //    setandfeed.loaduserset();
        },
            function() {

                // ExternalIA.SendRequest(userset, "GetUserSettings", data, "GetUserSettingsCallBack", "", true);
                setandfeed.loaduserset();
            },
               ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_Gset); });
    },
    GoFeedBack: function() {
        Other_ClearContainer();
        CallFunWithCondition(function() {
            setandfeed.feedbackpage();
        },
            function() {
                setandfeed.feedbackpage();
            },
               ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_FeedBack); });
    },
    GoMessage: function(index, length, categories) {

        Other_ClearContainer();
        var data = "";
        if (index) {
            data += ",index:" + index;
        }
        else {
            // data += ",index:1";
            data += ",index:" + MessageMgr.page;
        }
        if (length) {
            data += ",length:" + length;
        }
        else {
            data += ",length:" + MessageMgr.newcout;
        }
        if (categories) {
            data += ",categories:" + categories;
        }
        else {
            data += ",categories:null";
        }

        CallFunWithCondition(function() {
            //   MessageMgr.messageappend();

            // ExternalIA.SendRequest(message, "GetMessageList", data, "GetMessageListCallBack", "");
            GetData(message, "GetMessageList", data, GetMessageListCallBack, "", true, true);

        },
            function() {


                ExternalIA.SendRequest(message, "GetMessageList", data, "GetMessageListCallBack", "", true);


            },
               ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_Message); });
    },

    GoMessageDetail: function(msgId, duration) {

        //   Other_ClearContainer();
        var data = "";
        if (msgId) {
            data += ",msgId:" + msgId;
        }
        if (duration) {
            data += ",duration:" + duration;
        }

        CallFunWithCondition(function() {


            //ExternalIA.SendRequest(message, "GetMessage", data, "GetMessageCallBack", "");
            GetData(message, "GetMessage", data, GetMessageCallBack, "", true, true);
        },
            function() {

                ExternalIA.SendRequest(message, "GetMessage", data, "GetMessageCallBack", "", true);
                //  MessageMgr.genericappendmessagedetail();

            },
               ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_MessageDetail); });
    },


    GOVipUser: function() {

        Other_ClearContainer();
        CallFunWithCondition(function() {
            uservip.loaduservip();
        },
            function() {


                uservip.loaduservip();

            },
               ifdebug, true, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_Vip); });
    },
    //用户可用虚名
    GetAvailableUserNick: function(nickname) {


        CallFunWithCondition(function() {



        },
            function() {

                var data = "";
                if (nickname) {
                    data += ",nickname:\"" + nickname + "\"";
                }


                ExternalIA.SendRequest(userser, "GetNickNames", data, "AvailableUserNickCallBack", "");
            },
               ifdebug);
    },
    //修改资料
    UpdateCurrentUser: function(qq, mobile) {

        var data = "";
        if (mobile != "" && mobile.length > 0) {
            data += ",mobile:\"" + mobile + "\"";
        }
        if (qq != "" && qq.length > 0) {
            data += ",qq:\"" + qq + "\"";
        }

        CallFunWithCondition(function() {

            //   ExternalIA.SendRequest(userser, "UpdateCurrentUser", info, "UpdateCurrentUserCallBack", "");
            GetData(userser, "UpdateCurrentUser", data, UpdateCurrentUserCallBack, "", true, true);
        },
            function() {
                var sid = "{\"mobile\":\"" + mobile + "\",\"qq\":\"" + qq + "\"}";


                ExternalIA.SendRequest(userser, "UpdateCurrentUser", data, "UpdateCurrentUserCallBack", sid, true);
            },
               ifdebug, true);
    },
    //获取当前会话用户信息
    GetCurrentUser: function() {


        CallFunWithCondition(function() {
            // uservip.loaduservip();
        },
            function() {


                ExternalIA.SendRequest(userser, "GetCurrentUser", "", "GetCurrentUserCallBack", "");
            },
               ifdebug, false, function() { ExternalIA.currentRenderPage(Setting.Const_PageID_Vip); });
    },

    //发起手机认证
    SendValidationCode: function(mobile) {


        CallFunWithCondition(function() {
            // uservip.loaduservip();
        },
            function() {
                var data = "";
                if (mobile) {
                    data += ",mobile:\"" + mobile + "\"";
                }

                ExternalIA.SendRequest(userser, "SendValidationCode", data, "SendValidationCodeCallBack", "");
            },
               ifdebug);
    },
    //设置虚名
    SetNickName: function(nickName) {
        var data = "";
        if (nickName) {
            data += ",nickName:\"" + nickName + "\"";
        }
        CallFunWithCondition(function() {


            GetData(userser, "SetNickName", data, SetNickNameCallBack, nickName, true, true);



        },
            function() {

                ExternalIA.SendRequest(userser, "SetNickName", data, "SetNickNameCallBack", nickName, true);
            },
               ifdebug, true);
    },
    //验证用户手机
    MobileValidate: function(mobile, captcha) {


        CallFunWithCondition(function() {
            // uservip.loaduservip();
        },
            function() {
                var data = "";
                if (mobile) {
                    data += ",mobile:\"" + mobile + "\"";
                }
                if (captcha) {
                    data += ",captcha:\"" + captcha + "\"";
                }

                ExternalIA.SendRequest(userser, "MobileValidate", data, "MobileValidateCallBack", "");
            },
               ifdebug);
    },

    //获取完整的消息，此接口调用完毕会把消息置为已读
    GetMessage: function(msgId, duration) {


        var data = "";
        if (msgId) {
            data += ",msgId:" + msgId;
        }
        if (duration) {
            data += ",duration:" + duration;
        }

        CallFunWithCondition(function() {


            //ExternalIA.SendRequest(message, "GetMessage", data, "GetMessageCallBack", "");
            //   GetData(message, "GetMessage", data, GetMessageCallBack, "", true, true);
        },
            function() {

                ExternalIA.SendRequest(message, "GetMessage", data, "GetMessageCallBackApeend", ToString(msgId), true);
                //ExternalIA.SendRequest(userser, "SetNickName", data, "SetNickNameCallBack", nickName, true);
                //  MessageMgr.genericappendmessagedetail();

            },
               ifdebug);

    },
    // 获取当前用户消息列表

    GetMessageList: function(index, length, categories) {


        CallFunWithCondition(function() {
            // uservip.loaduservip();
        },
            function() {
                var data = "";
                if (index) {
                    data += ",index:\"" + index + "\"";
                }
                if (length) {
                    data += ",length:" + MessageMgr.newcout;
                }
                if (categories) {
                    data += ",categories:\"" + categories + "\"";
                }

                ExternalIA.SendRequest(message, "GetMessageList", data, "GetMessageListCallBack", "", true);
            },
               ifdebug);
    },
    //添加反馈

    addfeedback: function(title, content, gameNo) {


        CallFunWithCondition(function() {
            // uservip.loaduservip();
            var data = "";
            if (title) {
                data += ",title:\"" + title + "\"";
            }
            if (content) {
                data += ",content:\"" + content + "\"";
            }
            if (gameNo) {
                data += ",gameNo:\"" + gameNo + "\"";
            }

            //  ExternalIA.SendRequest(feedback, "addfeedback", data, "addfeedbackCallBack", "");
            GetData(feedback, "addfeedback", data, addfeedbackCallBack, "", true, true);
        },
            function() {
                var data = "";
                if (title) {
                    data += ",title:\"" + title + "\"";
                }
                if (content) {
                    data += ",content:\"" + content + "\"";
                }
                if (gameNo) {
                    data += ",gameNo:\"" + gameNo + "\"";
                }

                ExternalIA.SendRequest(feedback, "addfeedback", data, "addfeedbackCallBack", "", true);
            },
               ifdebug, true);
    },

    UpdateUserSettings: function() {


        CallFunWithCondition(function() {


        },
            function() {
                var data = "";

                data += ",gameNo:\"" + CurrentgameNo + "\"";
                data += ",options:{Transparent:\"" + setandfeed.transparency + "\"";

                // data += ",content:\"" + setandfeed.transparency + "\"";

                data += ",NoticeMode:\"" + setandfeed.noticeMode + "\"";
                data += "}";


                ExternalIA.SendRequest(userset, "UpdateUserSettings", data, "UpdateUserSettingsCallBack", "", true);
            },
               ifdebug, true);
    },

    RequestToken: function() {


        CallFunWithCondition(function() {


        },
            function() {
                var data = "";

                data += ",actionId:\"UpdateUser\"";




                ExternalIA.SendRequest(validation, "RequestToken", data, "RequestTokenCallBack", "", true);
            },
               ifdebug, true);
    },
    SetAlpha: function(setvalue) {

        if (setvalue > 0) {

            window.external.GPlus.Client.Set.SetAlpha(setvalue);
        }

    },
    GetAlpha: function() {



        return window.external.GPlus.Client.Set.GetAlpha();


    },
    SetEnableMessageTitle: function(enable) {
        window.external.GPlus.Client.Set.SetEnableMessageTitle(enable);

    },
    GetEnableMessageTitle: function() {
        return window.external.GPlus.Client.Set.GetEnableMessageTitle();

    },
    UpdateUserInfo: function() {
        window.external.GPlus.Client.User.UpdateUserInfo();

    },
    UpdateUserSettingsWin: function() {

        window.external.GPlus.Client.Set.UpdateUserSettings();
    },
    //


    /********************其他操作************************************************/
    /********************其他操作************************************************/
    currentRenderPage: function(pageID) {
        Global_CurrentRenderPageID = pageID;
    },

    genericMinWindow: function(msg, extreCss) {
        var html = "<div class=\"remindNote " + extreCss + "\">";
        html += "<div class=\"remindLeft\"></div>";
        html += "<div class=\"remindIn\">";
        html += "<p>" + msg + "</p>";
        html += "</div>";
        html += "</div>";
        return html;
    },
    showMessage: function(content, title, closeInterval, onClosedCall) {
        var extreCss = "";
        if (content) {
            if (content.indexOf("成功") >= 0) {
                extreCss = "remindSuc";
                closeInterval = 1000;
            }
            else if (content.indexOf("失败") >= 0) {
                extreCss = "remindErro";
                closeInterval = 1000;
                content = "网络延迟，请稍后重试";
            }
            else {
                closeInterval = closeInterval || 3000;
            }
        }

        var win = ExternalIA.genericMinWindow(content, extreCss);
        if ($.isFunction(onClosedCall)) {
            var html = $(win);
            $.fn.colorbox({ html: html, open: true, transition: "none", onClosed: onClosedCall });
        }
        else {

            setTimeout("ExternalIA.colorboxshow('" + win + "')", 200);
        }

        if (closeInterval)
            setTimeout("$.fn.colorbox.close(); ", closeInterval);
    },

    boxclose: function() {
        $.fn.colorbox.close();
    },
    genericWindow: function(title) {
        var html = "";
        if (title) {

            html += "<div class=\"zdiv zdivinfo\">";
            html += "<div class=\"titinfo\">";
            html += "<a href=\"javascript:void(0)\" class=\"close\"><img src=\"../skin/default/images/close.jpg\" border=\"0\" /></a>" + title;
            html += "</div>";

            html += "<div class=\"infotxt\" type=\"c\">";

            html += "</div>";
            html += "</div>";
        }
        else {
            html += "<div class=\"zdiv\">";
            html += "<div type=\"c\"></div>";


            html += "<p class=\"bat\"><a href=\"javascript:void(0)\"><input type=\"submit\" name=\"Submit2\" value=\"确定\"  class=\"all\"/>　<input type=\"submit\" name=\"Submit3\" value=\"取消\"  class=\"all\"/></a></p>";
            html += "</div>";
        }

        return html;
    },
    confirmMessage: function(message, title, doFunction, falseFunction, onClosedCall, innerWidth, innerHeight) {
        var confirm = "";
        confirm = $(ExternalIA.genericWindow(title));

        confirm.find("div[type=c]").append(message);
        confirm.find("input[name=Submit2]").click(function() {
            if ($.isFunction(doFunction)) {

                doFunction();
            }

            //  $.fn.colorbox.close();
        });
        confirm.find("input[name=Submit3]").click(function() {
            if ($.isFunction(falseFunction)) {

                falseFunction();
            }
            //   $.fn.colorbox.close();

        });
        confirm.find(".close").click(function() {
            $.fn.colorbox.close();
        });

        var box = "$.fn.colorbox({ html: confirm, open: true, transition: 'none', scrolling: false, opacity: 0.7";
        if ($.isFunction(onClosedCall)) {
            box += ", onClosed: onClosedCall";
        }
        if (innerWidth) {
            box += ", innerWidth: '" + innerWidth + "'";
        }
        if (innerHeight) {
            box += ", innerHeight: '" + innerHeight + "'";
        }
        box += "})";
        eval(box);
        //        if ($.isFunction(onClosedCall)) {

        //            $.fn.colorbox({ html: confirm, open: true, transition: "none", scrolling: false, opacity: 0.7, onClosed: onClosedCall, innerWidth: "340" });

        //           

        //        }
        //        else {


        //            $.fn.colorbox({ html: confirm, open: true, scrolling: false, opacity: 0.7, transition: "none" });
        //           

        //        }



    },
    feedbackDialog: function(message, onClosedCall, closeInterval) {
        var html = "";
        html += "<div class=\"fkdiv\" type=\"c\">";

        html += "</div>";
        var confirm = $(html);


        confirm.append(message);
        $.fn.colorbox({ html: confirm, open: true, overlayClose: false, transition: "none", opacity: 0.9, onClosed: onClosedCall, innerWidth: "380" });

        if (closeInterval)
            setTimeout("$.fn.colorbox.close(); ", closeInterval);

    },

    SendRequest: function(url, app, extre, callback, sid, isdefault) {
        var data = "";
        var time = 10;

        var httpurl = app ? $.format(url, app) : url;

        if (!extre)
            extre = "";
        if (isdefault) {
            data = "{accessToken: \"" + CurrentAccessToken + "\", sessionToken: \"" + CurrentSessionToken + "\"" + extre + "}";
        }
        else {
            data = extre;
        }

        openInterface.PublicRequest(sid, httpurl, data, "POST", time, callback);

    },
    GetLogin: function(data, containerID) {

        // CurrentAccessToken = data.ResultAttachObject;

        //     var passurl = $.format(passportser, "Login");

        var data = "{accessToken: \"" + CurrentAccessToken + "\", authProvider: \"test\"}";

        CallFunWithCondition(function() {


            //            var retString = GetData(passportser, "Login", data, "", "", false, false);


            //            if (retString.ResultNo == 0) {

            //                CurrentSessionToken = retString.ResultAttachObject.SessionToken;
            //                CurrentLoginUserAccountID = retString.ResultAttachObject.UserID;

            //            }
            GetData(passportser, "Login", data, "UserLoginCallBack", "", false, false);

        },
            function() {

                ExternalIA.SendRequest(passportser, "Login", data, "UserLoginCallBack", "", false);
            },
            ifdebug);




    },

    GoUserLogin: function(callback) {

        //ExternalIA.GetAccessToken(ExternalIA.GetLogin);
        // ExternalIA.GetAccessToken("");

        CurrentAccessToken = window.external.GPlus.Client.User.GetAuthenticate();
        CurrentSessionToken = window.external.GPlus.Client.User.GetSessionToken();
        CurrentLoginUserNickName = window.external.GPlus.Client.User.GetNickName();
        callback();

        //        CallFunWithCondition(function() {


        //            ExternalIA.GetAccessToken(ExternalIA.GetLogin, "");



        //        },
        //            function() {

        //                // ExternalIA.GetAccessToken();
        //               

        //            },
        //            ifdebug);


    },
    GetAccessToken: function(callback) {

        var timestamp = currenttime();

        var signedString = $.md5("appNo=" + appno + "&timestamp=" + timestamp + "&secretKey=" + secretKey + "");

        var data = "{appNo: \"" + appno + "\", timestamp: \"" + timestamp + "\", signedString: \"" + signedString + "\"}";



        CallFunWithCondition(function() {


            GetData(authser, null, data, callback, "", false, false);



        },
            function() {

                ExternalIA.SendRequest(authser, null, data, "GetAccessTokenCallBack", "", false);

            },
            ifdebug);

    },
    IsHaveName: function() {

        return CurrentLoginUserNickName != "" && CurrentLoginUserNickName.length > 0;
    },
    appendNavigate: function() {

    },
    pageOnLoadNotify: function(page) {

        ExternalIA.GetCach();

        CallFunWithCondition(function() {


        },
        function() {

            //  alert(window.external.GPlus.Client.User.GetNickName());
            if (page == "main") {

                if (IsFirst) {


                    //   ExternalIA.GoNameReg();
                    page = "help";
                    ExternalIA.pageOnLoadNotify(page);
                    return;

                }
                else {

                    ExternalIA.GoGmainPage();
                }
            }
            else if (page == "vip") {
                ExternalIA.GOVipUser();
            }
            else if (page == "message") {
                ExternalIA.GoMessage();
            }
            else if (page == "feedback") {
                ExternalIA.GoFeedBack();
            }
            else if (page == "seting") {
                ExternalIA.GoSetPage();


            }
            else if (page == "shop") {


                //            Other_ClearContainer();
                //            var html = "";
                //            html += "<div class=\"shop\"></div>";

                //            $("#midlist").append($(html));
                location.href = "http://10.240.248.103:10209/index.aspx?pt=garylei20110316";

            }
            else if (page == "kowo") {
            location.href = "http://player.kuwo.cn/webmusic/kuwodt/sndndt.jsp";
            
            }
            else if (page == "help") {


                Other_ClearContainer();
                var html = "";



                $("#midlist").append($("<div class=\"help\" clicknum=\"0\"></div>"));

                var body = $("#mainpage").parent();
                html += "<div>";
                html += "<a href=\"#\" class=\"helpleft\"></a><a href=\"#\" class=\"helpright\"></a>";
                html += "</div>";
                var item = $(html);


                var next = item.find(".helpright");
                var up = item.find(".helpleft");

                var help = $(".help");
                up.hide();
                next.live("mouseenter", function() {
                    next.css({
                        "background": "url(../skin/default/images/right.png) no-repeat 153px 159px",
                        "_filter": "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='../skin/default/images/right.png')",
                        "_background-image": "none",
                        "_margin": "159px 13px 0 0px",
                        "cursor:pointer": "cursor:pointer"
                    });
                });
                next.live("mouseleave", function() {
                    next.unbind("mouseenter");
                    next.css({
                        "background": "none",
                        "_filter": "none",
                        "_background-image": "none",
                        "_margin": "none",
                        "cursor:pointer": "none"
                    });
                });
                next.click(function() {

                    var clicknum = help.attr("clicknum");
                    ReplaceImg(true, parseInt(clicknum, 10), help, item);
                });
                up.click(function() {

                    var clicknum = help.attr("clicknum");
                    ReplaceImg(false, parseInt(clicknum, 10), help, item);
                });
                body.append(item);
            }
            else if (page == "usercenter") {
                if (ExternalIA.IsHaveName()) {
                    ExternalIA.GoUserCenter();

                }
                else {
                    ExternalIA.GoNameReg();
                }
            }

        },
        ifdebug);
    },
    UpdateMessageInfo: function() {
        window.external.GPlus.Client.User.UpdateMessageInfo();
    },
    GetCach: function() {
        CurrentAccessToken = window.external.GPlus.Client.User.GetAuthenticate();
        CurrentSessionToken = window.external.GPlus.Client.User.GetSessionToken();
        CurrentLoginUserNickName = window.external.GPlus.Client.User.GetNickName();
        CurrentgameNo = window.external.GPlus.Game.GetId();
        IsFirst = window.external.GPlus.Client.User.GetUserNew();
        //        CurrentLoginUserAccountID = window.external.GPlus.Client.User.GetUserId();
        //        alert(CurrentLoginUserAccountID);
    }

}

var UserModel = {
    UserID: "", //long	用户在G+服务端唯一的ID
    NickName: "", //string	用户设置的虚名（如果有的话）
    NickNameValidated: "", //bool	虚名是否已认证
    Mobile: "", //string	手机，中间四位加星
    MobileValidated: "", //bool	是否为已认证手机
    QQ: "", //string	QQ号
    VCULevel: "", //int	VCU等级，0=普通用户，1=VCU用户
    setname: function(name) {
        if (!name)
            name = "";
        UserModel.NickName = name;
        window.external.GPlus.Client.User.SetNickName(ToString(name));
    },
    //    setcall: function(call) {
    //        if (!call)
    //            call = "";
    //        UserModel.Mobile = call;

    //        window.external.GPlus.Client.User.SetMobile(ToString(call));
    //    },
    //    setqq: function(qq) {
    //        if (!qq)
    //            qq = "";
    //        UserModel.QQ = qq;
    //        window.external.GPlus.Client.User.SetQQ(ToString(qq));
    //    },
    getname: function() {
        UserModel.NickName = window.external.GPlus.Client.User.GetNickName();

        return UserModel.NickName;
    },
    getcall: function() {
        
        UserModel.Mobile = window.external.GPlus.Client.User.GetMobile();

        return UserModel.Mobile;
    },
    getqq: function() {
        UserModel.QQ = window.external.GPlus.Client.User.GetQQ();

        return UserModel.QQ;
    }
}

var Setting =
{
    Const_PageID_Home: "1", //G+主页
    Const_PageID_UserCenter: "2", //个人中心
    Const_PageID_NameReg: "3", //虚名注册
    Const_PageID_Message: "4", //消息
    Const_PageID_MessageDetail: "41", //消息详细
    Const_PageID_Gset: "5", //设置
    Const_PageID_FeedBack: "6", //反馈
    Const_PageID_Vip:"7",//VIP
    init: function() {
        if (Setting.currentIsNameReg()) {
            selectli(1);
        }
        else if (Setting.currentIsUserCenter()) { selectli(1); }
        else if (Setting.currentIsHome()) { selectli(1); }
        else if (Setting.currentIsMessage()) { selectli(2); }
        else if (Setting.currentIsMessageDetail()) { selectli(2); }
        else if (Setting.currentIsVip()) { selectli(3); }
        else if (Setting.currentIsGset()) { selectli(4); }
        else if (Setting.currentIsFeedBack()) { selectli(5); }
         
        else
        { selectli(0); }
    },
    currentIsHome: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_Home;
    },
    currentIsUserCenter: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_UserCenter;
    },
    currentIsNameReg: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_NameReg;
    },
    currentIsMessage: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_Message;
    },
    currentIsGset: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_Gset;
    },
    currentIsMessageDetail: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_MessageDetail;
    },
    currentIsFeedBack: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_FeedBack;
    },
    currentIsVip: function() {
        return Global_CurrentRenderPageID == Setting.Const_PageID_Vip;
    }



}

$("[type='nextpage']").live("click", function() {
  
//    if (ExternalIA.IsHaveName()) {

//        ExternalIA.GoUserCenter();
//    } else {

//        ExternalIA.GoGmainPage();
//    }
ExternalIA.GoGmainPage();


});
$("[type='tit']").live("click", function() {
    ExternalIA.GOVipUser();
});

$("[type='up']").live("click", function() {
    var newsid = $(this).attr("newsid");
    if (Setting.currentIsMessageDetail()) {
        MessageMgr.curentstat = 0;
        ExternalIA.GoMessageDetail(newsid, -1);
       
    }
    else if (Setting.currentIsMessage()) {
        var page = $(this).attr("page") || 0;
        var up = parseInt(page, 10) - MessageMgr.newcout;
        if (up < 0) {
            up = 0;
        }
        MessageMgr.page = up;
        ExternalIA.GoMessage(up, MessageMgr.newcout);
    }
    else { }
});
$("[type='next']").live("click", function() {
    var newsid = $(this).attr("newsid");
    if (Setting.currentIsMessageDetail()) {
        MessageMgr.curentstat = 2;
        ExternalIA.GoMessageDetail(newsid, 1);
       
    }
    else if (Setting.currentIsMessage()) {
        var page = $(this).attr("page") || 1;
         var currentpage=Math.floor(page / 4) + 1;
        var back = 0;
        if (currentpage == MessageMgr.pagecout) {
            back = page;
        }
        else {
             back = parseInt(page, 10) + MessageMgr.newcout;
        }
        MessageMgr.page = back;
        ExternalIA.GoMessage(back, MessageMgr.newcout);
    }
    else { }
});


function ReplaceImg(flag, num, container, item) {


    if (flag == true) {
        if (num == 3) {
            item.remove();
            ExternalIA.GoGmainPage();
           
            return;
        }
        num += 1;
    }
    else {
        num -= 1;
    }
    num = Math.max(Math.min(num, 3), 0);
 
    if (num == 0) {
        container.unbind();
        container.css({ "background": "url(../skin/default/images/01.jpg) no-repeat" });
      container.attr("clicknum", ""+num+"");
      item.find(".helpright").show();
      item.find(".helpleft").hide();
    }
    else if (num == 1) {
    container.unbind();
    container.css({ "background": "url(../skin/default/images/02.jpg) no-repeat" });
    container.attr("clicknum", ""+num+"");
    item.find(".helpright").show();
    item.find(".helpleft").show();
    } else if (num == 2) {
    container.unbind();
    container.css({ "background": "url(../skin/default/images/03.jpg) no-repeat" });
  container.attr("clicknum", ""+num+"");
  item.find(".helpright").show();
  item.find(".helpleft").show();
    } else if (num == 3) {
    container.unbind();
   container.css({ "background": "url(../skin/default/images/04.jpg) no-repeat" });
  
   container.attr("clicknum", "" + num + "");
   item.find(".helpright").show();
   item.find(".helpright").show();
 
   container.click(function() { ExternalIA.GoGmainPage(); });
    }
    else {
      
    }

 
}


function ToString(value) {
    if (value)
        return value.toString();

    return value;
}

function selectli(index) {
    var banner = $("#bannerPlaceHolder");
    banner.find("a").removeClass("on");
    if (index == 1) {

        banner.find(".l1").parent().addClass("on");
    }
    else if (index == 2) {
        banner.find(".l2").parent().addClass("on");
    }
    else if (index == 3) {
        banner.find(".l3").parent().addClass("on");
    }
    else if (index == 4) {
    banner.find(".Feedback").addClass("on");
    }
    else if (index == 5) {
    banner.find(".Setup").addClass("on");
    }
    else { 
    }
}




function Other_ClearContainer(pageID, containerID) {
    if (!containerID) {
        containerID = "midlist";
        // containerID = Global_CurrtentPage != Setting.Const_PageID_NewSearch ? "maincontent" : "searchcontent";
    }
  
  
    
    var banner = $("#bannerPlaceHolder");
    var a = banner.find(".l3").parent();

    if (ExternalIA.IsHaveName()) {

        a.attr("title", "您正在享用VIP特权");
    } else {
        a.attr("title", "您尚未激活VIP，注册虚名可激活VIP");

    }
    
    $(".banner").remove();
    var container = $("#" + containerID);
    container.removeClass();
    container.addClass("midlist");
    container.html("");
    $(".helpleft").remove();
    $(".helpright").remove();
}
//$(function() {
//    var banner = $("#bannerPlaceHolder");

//    banner.find(".l1").parent().click(function() {
////        if (ExternalIA.IsHaveName()) {

////            ExternalIA.GoUserCenter();
////        } else {

////            ExternalIA.GoNameReg();
//    //        }
//    window.location.href = "/usercenter/default.html";
//        ExternalIA.boxclose();
//    });
//    banner.find(".Feedback").click(function() {
//    window.location.href = "/usercenter/default.html?load=seting";
//     //   ExternalIA.GoSetPage();
//        ExternalIA.boxclose();
//    });
//    banner.find(".l2").parent().click(function() {

//    window.location.href = "/usercenter/default.html?load=message";
//        ExternalIA.boxclose();
//    });
//    banner.find(".Setup").click(function() {
//    window.location.href = "/usercenter/default.html?load=feedback";
//      //  ExternalIA.GoFeedBack();

//    });
//    banner.find(".l3").parent().click(function() {

//    //  ExternalIA.GOVipUser();
//    window.location.href = "/usercenter/default.html?load=vip";
//        ExternalIA.boxclose();
//    });
//    var a = banner.find(".l3").parent();


//    if (ExternalIA.IsHaveName()) {

//        a.attr("title", "您正在享用VIP特权");
//    } else {
//        a.attr("title", "您尚未激活VIP，注册虚名可激活VIP");

//    }
//    //

//});


