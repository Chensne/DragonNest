var GIndexMgr = {
    contentpage: "midlist",
    mainpage: "mainpage",
    bannerPlaceHolder: "bannerPlaceHolder",
    //    qqnum: "",
    //    callnum: "",
    getcontentpage: function() {
        return $("#" + GIndexMgr.contentpage);
    },
    getmainpage: function() {
        return $("#" + GIndexMgr.mainpage);
    },
    getbanner: function() {
        return $("#" + GIndexMgr.bannerPlaceHolder);
    },
    indexLoad: function(source, callback) {
        Other_ClearContainer();
        GIndexMgr.editpageoftenuserchange();

        var banner = $("<div class=\"banner\"><img src=\"../skin/default/images/banner.jpg\" width=\"436\" /></div>");
        var newstitle = $("<div class=\"title\"><a href=\"javascript:void(0)\">更多</a>消息</div>");
        newstitle.find("a").click(function() {
            ExternalIA.GoMessage();
        });
        var container = GIndexMgr.getcontentpage();
        // container.html("");

        if (Setting.currentIsUserCenter()) {
            GIndexMgr.fillusercenter(container);
        }
        else {
            banner.insertBefore(container);
        }


        container.append(newstitle);
        var num = 0;

        if (source) {

            if (source.length > 0) {

                $.each(source, function() {
                    if (num > 3) {
                        return;
                    }
                   //if (this.IsNew == true) {
                        MessageMgr.appendmessagedetail(this, container);
                        num++;
                 //   }


                });
                if (num == 0) {
                    container.append($("<div><br/>&nbsp;&nbsp;暂时没有消息</<div>"));
                }

            }
            else {
                container.append($("<div><br/>&nbsp;&nbsp;暂时没有消息</<div>"));
            }
        }
        else {

            container.append($("<div><br/>&nbsp;&nbsp;暂时没有消息</<div>"));
        }

        //IsFirst = false;
        if ($.isFunction(callback)) {

            callback();
        }
        num = 0;
    },

    editpageoftenuserchange: function() {
        var title = "请注册虚名";
        if (ExternalIA.IsHaveName()) {
            title = CurrentLoginUserNickName;
        }
        var banner = GIndexMgr.getbanner();
        var namebanner = banner.find("ul >li").eq(0);
        var adiv = namebanner.find("a div");
        adiv.html(title);
    },


    fillusercenter: function(container) {



        var html = "";
        html += "<div class=\"namein\">";
        html += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td colspan=\"3\">";
        html += "<div class=\"login\">";
        html += "<ul>";
        html += "<li>" + CurrentLoginUserNickName + "</li>";
        html += "<li></li>";
        html += "<li><a href=\"javascript:void(0)\">修改虚名&gt;</a></li>";
        html += "</ul>";
        html += "</div>";

        html += "</td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td colspan=\"3\"><span>QQ</span><span name=\"qqnum\">" + UserModel.getqq() + "</span><span>|</span><span>手机</span><span name=\"callnum\">" + UserModel.getcall() + "</span><a href=\"javascript:void(0)\" type=\"userinfolink\">";

        html += ($.trim(UserModel.getqq()).length > 0 || $.trim(UserModel.getcall()).length > 0 ? "修改个人资料" : "填写个人资料");
        html += "&gt;</a> </td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td width=\"6%\" valign=\"top\"><div align=\"center\"><img src=\"../skin/default/images/p2.jpg\" width=\"22\" height=\"20\" /></div></td>";
        if (ExternalIA.IsHaveName()) {
            html += "<td width=\"61%\"><img src=\"../skin/default/images/p3.jpg\" title=\"点击查看G+成长规则\"/></td>";
        } else {
            html += "<td width=\"61%\"><img src=\"../skin/default/images/p3.jpg\" title=\"点击查看G+成长规则\"/></td>";
        }
        html += "<td width=\"33%\"><a href=\"javascript:void(0)\" type=\"tit\">查看所有G+VIP特权&gt;</a></td>";
        html += "</tr>";
        html += "</table>";
        html += "</div>";
        var item = $(html);
        var login = item.find(".login");

        login.find("ul >li").eq(2).click(function() {
            if (ExternalIA.IsHaveName()) {
                var sure = false;
                var html = GIndexMgr.changenamedialog();
                var dialog = $(html);

                var textfield = dialog.find("input[name='textfield22']");
                textfield.css({ "color": "#FFFFFF" });
                if (textfield.is("input")) {
                    
                    ExternalIA.confirmMessage(dialog, "虚名修改", function() { sure = true; ExternalIA.SetNickName(textfield.val().Trim()); }, function() { sure = false; ExternalIA.boxclose(); }, function() {
                        if (sure) {
                            ExternalIA.confirmMessage(GIndexMgr.surechangnamedialog(), "虚名修改", function() { ExternalIA.boxclose(); });
                        }

                    }, "405px","300px");
                    textfield.setCaretPosition(CurrentLoginUserNickName.length);
                }
            } else {
                ExternalIA.GoNameReg();
            }
        });
        item.find("a[type=userinfolink]").click(function() {
            if (ExternalIA.IsHaveName()) {
                var userinfo = GOftenNameMgr.fillConfirmUserInfo();
                var title = "";
                if ($(this).html().indexOf("填写") >= 0) {
                    title = "填写个人资料";
                }
                else {
                    title = "修改个人资料";
                }
                ExternalIA.confirmMessage(userinfo, title, function() {
                    var QQ = userinfo.find("input[name=textfield2]").val().Trim();

                    var call = userinfo.find("input[name=textfield22]").val().Trim();

                    var a = $.trim(UserModel.getqq()).length > 0 || $.trim(UserModel.getcall()).length > 0 ? "修改个人资料" : "填写个人资料";
                    item.find("a[type=userinfolink]").html(a + "&gt;");
                    ExternalIA.UpdateCurrentUser(QQ, call);
                    ExternalIA.boxclose();
                    //alert(a);
                }, function() {
                    ExternalIA.boxclose();
                }, "340px");
            } else {
                ExternalIA.GoNameReg();
            }
        });


        container.append(item);

    },
    changenamedialog: function() {
        var html = "";
        html += "<table width=\"340px\" height=\"200px\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";

        html += "<tr>";
        html += "<td height=\"40\" colspan=\"3\">虚名只能修改一次，系统在收到请求后的三天内回复确认消息。</td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td width=\"31%\" height=\"40\"><div align=\"right\"><span class=\"c\">请输入新虚名：</span></div></td>";
        html += "<td height=\"40\" colspan=\"2\"><input name=\"textfield22\" maxlength=\"16\" type=\"text\" class=\"namep namep165\" value=\"" + CurrentLoginUserNickName + "\" />&nbsp;<a href=\"javascript:void(0)\" class=\"tit\"></a></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td colspan=\"3\" class=\"h12\">允许4-16个字符，支持中英文和数字，还支持火星文哦~建议使用游戏角色名</td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"50\">&nbsp;</td>";
        html += "<td width=\"18%\" ><a href=\"javascript:void(0)\">";
        html += "<input type=\"submit\" name=\"Submit2\" value=\"确定\"  class=\"all\"/>";
        html += "</a></td>";
        html += "<td width=\"51%\" ><a href=\"javascript:void(0)\">";
        html += "<input type=\"submit\" name=\"Submit3\" value=\"取消\"  class=\"all\"/>";
        html += "</a></td>";
        html += "</tr>";
        html += "</table>";

        return html;
    },
    surechangnamedialog: function() {
        var html = "";
        html += "<table width=\"320px\" height=\"100px\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td width=\"18%\" valign=\"top\"><div align=\"center\"><img src=\"../skin/default/images/tip.jpg\" width=\"38\" height=\"30\" /></div></td>";
        html += "<td width=\"82%\"><span class=\"c\">您的虚名修改请求已经成功提交，系统将在三天内处理您的请求，请耐心等待。</span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"50\" colspan=\"2\" valign=\"bottom\"><div align=\"center\"><a href=\"javascript:void(0)\">";
        html += "<input type=\"submit\" name=\"Submit2\" value=\"确定\"  class=\"all\"/>";
        html += "</a></div></td>";
        html += "</tr>";
        html += "</table>";
        var item = $(html);

        return item;
    }

}