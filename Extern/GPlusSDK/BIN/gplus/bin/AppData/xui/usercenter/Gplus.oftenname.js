var GOftenNameMgr = {
    contentpage: "midlist",
    mainpage: "mainpage",
    getcontentpage: function() {
        return $("#" + GIndexMgr.contentpage);
    },
    getmainpage: function() {
        return $("#" + GIndexMgr.mainpage);
    },
    genericoOftenNameSet: function(source, callback) {
        Other_ClearContainer();
        GOftenNameMgr.initIfNecessary(source);
        if ($.isFunction(callback))
            callBack(callback);
       

    },
    initIfNecessary: function(source) {
        var textfield = "";
        var html = "";
        var check = true;
        html += "<div class=\"titleA1\">虚名</div>";
        html += "<div class=\"name\">";
        html += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td height=\"30\" colspan=\"4\"><span class=\"tit\">注册虚名，获得G+ VIP特权。更多商城折扣，更多优惠!</span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td width=\"7%\" height=\"24\"><div align=\"center\"><img src=\"../skin/default/images/name.jpg\" width=\"16\" height=\"15\" /></div></td>";
        html += "<td colspan=\"2\">";
        if (source.length > 0) {

            html += "选择您尚未被占用的虚名：";
        } else {
            html += "请填写您的虚名：        ";
        }
        html += "</td>";
        html += "</tr>";

        if (source) {

            if (source.length > 0) {

                $.each(source, function() {
                    html += GOftenNameMgr.appendoftenlist(this.NickName);

                });

            }
        }




        html += "<tr>";
        html += "<td height=\"50\">&nbsp;</td>";
        html += "<td><div align=\"center\">";
        if (source.length > 0) {
            html += "<input type=\"radio\" name=\"radiobutton\" value=\"custom\" />";
        }
        html += "</div></td>";
        html += "<td width=\"36%\"><input name=\"textfield\" type=\"text\" class=\"namep\" maxlength=\"16\" value=\"自定义\" /></td>";
        //   html += "<td width=\"50%\"><span class=\"h12\">4-16个字符，支持中英文和数字，还支持火星文哦~。</span></td>";
        html += "<td width=\"50%\"><span class=\"h12\"></span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"32\">&nbsp;</td>";
        html += "<td>&nbsp;</td>";
        html += "<td valign=\"bottom\"><a href=\"javascript:void(0)\" type=\"sure\"><input type=\"submit\" name=\"Submit\" value=\"确定\"  class=\"all\"/></a></td>";
        html += "</tr>";
        html += "</table>";
        html += "</div>";
        var container = GOftenNameMgr.getcontentpage();
        var item = $(html);
        var cusinput = item.find("input[name=textfield]");
        var td = cusinput.parent();
        var h12 = item.find(".h12");
        var cusinputcontent = "";
        cusinput.keyup(function(e) {

          
            cusinputcontent = $.trim($(this).val());

            if (cusinputcontent.indexOf("自定义") >= 0) {
                $(this).val(cusinputcontent.replace("自定义", ""));
            }
           
            if (cusinputcontent == "") {
                $(this).val("自定义");
                cusinput.setCaretPosition(0);
            }





           
            if (e.keyCode == 13) {
                item.find("[valign=bottom]").find("a input").click();
            }
        });
        item.find("[valign=bottom]").find("a input").click(function() {
            if (source.length > 0) {
                var checked = item.find("input[name=radiobutton]:checked");
                if (checked) {
                    if (checked.attr("value") != "custom") {
                        textfield = $.trim(checked.attr("value"));
                    }
                    else {
                        textfield = $.trim(cusinput.val());
                    }
                }
            } else {
                textfield = $.trim(cusinput.val());
            }

            check = GOftenNameMgr.checknickname(textfield, h12);

            if (textfield.length > 0 && textfield != "自定义" && check) {
                var content = "<p>虚名在创建后，只允许修改一次。</p><p>您确定要以<span>" + textfield + "</span>作为您的虚名吗？</p>";
                ExternalIA.confirmMessage($(content), "", function() {

                    ExternalIA.SetNickName(textfield);

                }, function() {

                    ExternalIA.boxclose();
                }, "", "340px");
            }
        });

        item.find("[name=radiobutton]").click(function() {
            var custom = $(this);
            check = true;
            if (custom.attr("value") == "custom") {


                cusinput.removeAttr("readonly");
                cusinput.css({ "background": "none repeat scroll 0 0 #FFFFFF", "color": "#000000" });
                h12.css({ "color": "#9D9D9D" });
                h12.html("4-16个字符，支持中英文和数字，还支持火星文哦~。");

                cusinput.setCaretPosition(0);



            }
            else {

                cusinput.val("自定义");
                cusinput.attr("readonly", "true");

                cusinput.css({ "background": "none repeat scroll 0 0 #424242", "color": "#A0A0A0" });
                h12.html("");

            }
        });

        if (source.length == 0) {
            cusinput.removeAttr("readonly");
            cusinput.css({ "background": "none repeat scroll 0 0 #FFFFFF", "color": "#000000" });
            h12.css({ "color": "#9D9D9D" });
            h12.html("4-16个字符，支持中英文和数字，还支持火星文哦~。");
            cusinput.setCaretPosition(0);

        }
        container.append(item);
        GOftenNameMgr.loadVipPage(container);
        //var cusinput = page.find("input[name=textfield]");
        cusinput.setCaretPosition(0);
    },
    regmessage: function(msg) {
        var container = GOftenNameMgr.getcontentpage();
        var span = container.find(".h12");
        if (span.is("span")) {
            span.css({ "color": "#FF0000" });
            span.html(msg);
        }
    },
    checknickname: function(nickname, container) {
        var check = true;

        if (nickname && nickname != "") {

            if (nickname.length < 4 || nickname.length > 16) {
                check = false;
                container.css({ "color": "#FF0000" });
                container.html("请输入4-16个字符");

            }
            else {
                container.css({ "color": "#9D9D9D" });
                container.html("4-16个字符，支持中英文和数字，还支持火星文哦~。");

                if (isChinaLettNum(nickname)) {
                    check = false;
                    container.css({ "color": "#FF0000" });
                    container.html("请输入中英文和数字，还支持火星文");

                }
                else {
                    container.css({ "color": "#9D9D9D" });
                    container.html("4-16个字符，支持中英文和数字，还支持火星文哦~。");
                }

            }


        }
        else {
            check = false;
            container.css({ "color": "#FF0000" });
            container.html("输入不能为空");
        }
        return check;
    },
    appendoftenlist: function(nickname) {
        var html = "";
        html += "<tr>";
        html += "<td height=\"24\">&nbsp;</td>";
        html += "<td width=\"7%\"><div align=\"center\">";
        html += "<input type=\"radio\" name=\"radiobutton\" value=\"" + nickname + "\" />";
        html += "</div></td>";
        html += "<td width=\"86%\">" + nickname + "</td>";
        html += "</tr>";
        return html;
    },
    loadVipPage: function(container) {
        var html = "";
        html += "<div class=\"titleA1\">您当前的VIP等级可享受的特权</div>";

        html += "<div class=\"VIP\">";
        html += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td width=\"8%\" height=\"26\"><div align=\"center\"><img src=\"../skin/default/images/p1.jpg\" width=\"22\" height=\"20\" /></div></td>";
        html += "<td colspan=\"3\"><span class=\"red\">您尚未激活VIP,无法享受VIP2特权。马上注册虚名享受VIP特权吧!</span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"26\" valign=\"top\"><div align=\"center\"><img src=\"../skin/default/images/p2.jpg\" width=\"22\" height=\"20\" /></div></td>";
        html += "<td colspan=\"3\"><div align=\"left\" title=\"点击查看G+成长规则\"><img src=\"../skin/default/images/p3.jpg\" width=\"239\"/></div></td>";

        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"60\" colspan=\"4\">";
        html += "<div class=\"tips\"><span class=\"iptit\">VIP2</span><span class=\"iptita1\">1.</span>在线经验获得+60%<span class=\"iptita1\">2.</span>商城购物8折优惠</div>";
        html += "</td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td colspan=\"2\"><div align=\"center\"><a href=\"javascript:void(0)\" type=\"tit\" class=\"tit\">查看所有G+VIP特权 &gt;</a></div></td>";
        html += "<td width=\"38%\"><div align=\"center\"><a href=\"javascript:void(0)\">查看所有G+成长规则 &gt;</a></div></td>";
        html += "<td width=\"29%\"><div align=\"right\" type=\"nextpage\"><a href=\"javascript:void(0)\"><img src=\"../skin/default/images/next.jpg\" width=\"78\" height=\"27\" />"; //<input type=\"submit\" name=\"Submit222\" value=\"下次再说&gt;\"  class=\"alllong\"/>
        html += "</a></div></td>";


        html += "</tr>";
        html += "</table>";
        html += "</div>";
        var item = $(html);

        container.append(item);



    },
    fillConfirmUserInfo: function() {
        var html = "";
        html += "<table width=\"340px\" border=\"0\" cellspacing=\"0\" height=\"200px\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td height=\"40\" colspan=\"3\"><div align=\"center\">请填写您的个人资料，可获得<span class=\"c\">200点G+</span>经验值。</div></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td width=\"28%\" height=\"40\"><div align=\"right\"><span class=\"c\">QQ号码：</span></div></td>";
        html += "<td height=\"40\" colspan=\"2\"><input name=\"textfield2\"  type=\"text\" class=\"namep namep165\" value=\"" + UserModel.getqq() + "\" /></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"40\"><div align=\"right\"><span class=\"c\">手机号码：</span></div></td>";
        html += "<td height=\"40\" colspan=\"2\"><input name=\"textfield22\" num=\"" + UserModel.getcall() + "\" type=\"text\" class=\"namep namep165\" value=\"" + UserModel.getcall() + "\" />&nbsp;<a href=\"javascript:void(0)\" showinpuit=\"true\" class=\"tit\">获取验证码</a></td>";
        html += "</tr>";




        html += "<tr>";

        html += "<td height=\"50\">&nbsp;</td>";
        html += "<td width=\"18%\" ><a href=\"javascript:void(0)\">";
        html += "<input type=\"submit\" name=\"Submit2\" value=\"确定\"  class=\"all\"/>";
        html += "</a></td>";
        html += "<td width=\"54%\" ><a href=\"javascript:void(0)\">";
        html += "<input type=\"submit\" name=\"Submit3\" value=\"取消\"  class=\"all\"/>";
        html += "</a></td>";

        html += "</tr>";
        html += "</table>";
        var item = $(html);
        //item.
        GOftenNameMgr.genericbindnuminput(item.find("input[name=textfield2]"));
        GOftenNameMgr.genericbindnuminput(item.find("input[name=textfield22]"));
        item.find(".tit").hide();
        item.find(".tit").click(function() {


            var enableClick = $(this).attr("showinpuit") || "true";

            if (enableClick == "true") {
                ExternalIA.RequestToken();
                var container = $(this).parent().parent();
                var input = "";
                input += "<tr>";
                input += "<td height=\"40\"><div align=\"right\"><span class=\"c\">验证码：</span></div></td>";
                input += "<td colspan=\"2\" ><a href=\"javascript:void(0)\"></a><a href=\"javascript:void(0)\">";
                input += "<input name=\"textfield222\" type=\"text\" class=\"namep namep55\" value=\" \" />";
                input += "</a></td>";
                input += "</tr>";

                $(input).insertAfter(container);
                $(this).attr("showinpuit", "false");
            }
        });
        var callinput = item.find("input[name=textfield22]");
        if (UserModel.Mobile != "") {
            callinput.keyup(function() {
                var num = $(this).attr("num") || "";
                if ($(this).val() != num) {
                    item.find(".tit").show();
                }
                else {
                    item.find(".tit").hide();
                }
            });
        }



        return item;


    },
    genericbindnuminput: function(container) {
        var view = $(container);
        $(view).css({ "color": "#FFFFFF" });
        view.bind("keypress", function(event) {
            if (event.keyCode == 46) {
                if (this.value.indexOf(".") != -1) {
                    return false;
                }
            } else {
                return event.keyCode >= 46 && event.keyCode <= 57;
            }
        });
        view.bind("blur", function() {
            if (this.value.lastIndexOf(".") == (this.value.length - 1)) {
                this.value = this.value.substr(0, this.value.length - 1);
            } else if (isNaN(this.value)) {
                this.value = "";
            }
        });
        view.bind("paste", function() {
            var s = clipboardData.getData('text');
            //  if (!//D/.test(s));
            value = s.replace(/^0*/, '');
            return false;
        });
        view.bind("dragenter", function() {
            return false;
        });
        view.bind("keyup", function() {
            $(this).css({ "color": "#FFFFFF" });
            if (/(^0+)/.test(this.value)) {
                this.value = this.value.replace(/^0*/, '');
            }
        });
    }
}