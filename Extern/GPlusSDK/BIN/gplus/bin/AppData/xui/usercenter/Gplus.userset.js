var setandfeed = {
    contentpage: "midlist",
    mainpage: "mainpage",
    bannerPlaceHolder: "bannerPlaceHolder",

    transparency: 50,
    noticeMode: false,

    getcontentpage: function() {
        return $("#" + GIndexMgr.contentpage);
    },
    getmainpage: function() {
        return $("#" + GIndexMgr.mainpage);
    },
    getbanner: function() {
        return $("#" + GIndexMgr.bannerPlaceHolder);
    },
    loaduserset: function() {
        Other_ClearContainer();
        var container = setandfeed.getcontentpage();
        setandfeed.mergeuserset(container);
    },
    mergeuserset: function(container) {


        setandfeed.transparency = ExternalIA.GetAlpha();
        setandfeed.noticeMode = ExternalIA.GetEnableMessageTitle();

        setandfeed.bindtransparency(container);
        setandfeed.bindmessagereminder(container);
    },
    bindtransparency: function(container) {
        var setvalue = Math.floor(((setandfeed.transparency - 127) / 127) * 100);

        var html = "";

        html += "<div class=\"titleA1\">透明度</div>";

        html += "<div class=\"tran\">";
        html += "<div class=\"bgimg\">";
        html += "<span class=\"Perce\">";
        html += "<p>" + setvalue + "%</p>";
        html += "<p class=\"p2\"></p>";
        html += "</span>";
        html += "</div>";
        //  html += "<p class=\"p1\"><span class=\"q1\">半透明</span><span class=\"q2\"></span><span class=\"q3\">不透明</span></p>";
        html += "</div>";
        var item = $(html);


        var d = "";
        var slider = item.find(".bgimg");

        slider.bind('mousedown', function(e) {

            $(this).bind('mousemove', "", $.drag).bind('mouseup', "", $.drop).bind('mouseleave', "", $.drop);
        });

        var width = Math.min(Math.floor((setvalue / 100) * 268), 268);

        slider.find(".Perce").css({ "left": width + 22 });




        container.append(item);



    },

    bindmessagereminder: function(container) {
        var html = "";
        html += "<div class=\"titleA1\">消息提醒</div>";

        html += "<div class=\"VIP\">";
        html += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td width=\"7%\" height=\"30\"><div align=\"center\">";
        if (setandfeed.noticeMode == false) {
            html += " <input type=\"radio\" name=\"radiobutton\" value=\"0\" checked=true/>";
        }
        else {
            html += " <input type=\"radio\" name=\"radiobutton\" value=\"0\" />";
        }
        html += "</div></td>";
        html += "<td width=\"93%\" type=\"radioselect\">仅G+图标闪烁提醒</td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"30\"><div align=\"center\">";
        if (setandfeed.noticeMode == true) {
            html += " <input type=\"radio\" name=\"radiobutton\" value=\"1\" checked=true/>";
        }
        else {
            html += " <input type=\"radio\" name=\"radiobutton\" value=\"1\"/>";
        }

        html += "</div></td>";
        html += "<td type=\"radioselect\">G+图标闪烁和标题文字提醒</td>";
        html += "</tr>";
        html += "</table>";
        html += "</div>";
        var item = $(html);
        var radio = item.find("input[name=radiobutton]");
        if (setandfeed.noticeMode == true) {

            radio.eq(1).attr("checked", "true");

        }
        else {

            radio.eq(0).attr("checked", "true");
        }
        radio.click(function() {
            if ($(this).attr("value") == "0") {
                ExternalIA.SetEnableMessageTitle(false);
                setandfeed.noticeMode = false;
            }
            else {
                ExternalIA.SetEnableMessageTitle(true);
                setandfeed.noticeMode = true;
            }
        });
        var radioselect = item.find("td[type=radioselect]");
        radioselect.click(function() {
            var td = $(this).parent();
            td.find("input[name=radiobutton]").click();
        });
        container.append(item);
    },
    feedbackpage: function() {
        Other_ClearContainer();
        var container = setandfeed.getcontentpage();

        if (!container.hasClass("midlisfk")) {
            container.addClass("midlisfk");
        }
        setandfeed.appendfeedbackdetail(container);

    },
    appendfeedbackdetail: function(container) {
        var html = "";
        html += "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
        html += "<tr>";
        html += "<td height=\"40\" colspan=\"2\" valign=\"top\"><span class=\"tit14\">你好，欢迎您为我们的产品提供反馈和意见</span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td width=\"11%\" valign=\"top\"><div align=\"right\">标题：&nbsp;</div></td>";
        html += "<td width=\"89%\"><textarea onkeyup=\"setandfeed.EidtWordChanges(this,$('span[name=try1]'),30)\" name=\"textfield\" type=\"text\" class=\"namep namep300\">请输入标题</textarea></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"30\">&nbsp;</td>";
        html += "<td><span style=\"color:#A0A0A0\" name=\"try1\">还可输入30字</span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td valign=\"top\"><div align=\"right\">内容：&nbsp;</div></td>";

        html += "<td><textarea  onkeyup=\"setandfeed.EidtWordChanges(this,$('span[name=try2]'),1000)\" name=\"textfield2\" type=\"text\" class=\"namep namep310\">请输入内容</textarea></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"30\">&nbsp;</td>";
        html += "<td><span style=\"color:#A0A0A0\" name=\"try2\">还可输入1000个字</span></td>";
        html += "</tr>";
        html += "<tr>";
        html += "<td height=\"60\">&nbsp;</td>";
        html += "<td><a href=\"javascript:void(0)\">";
        html += "<input type=\"submit\" name=\"Submit22\" value=\"提交\"  class=\"all\"/>";
        html += "</a></td>";
        html += "</tr>";
        html += "</table>";
        var item = $(html);
        var titlec = item.find(".namep300");
        var contentc = item.find(".namep310");

        titlec.focus(function() {
            var title = $(this).val()
            if (title == "请输入标题") {
                titlec.val("");
            }
            this.setCaretPosition(0);
        });
        titlec.blur(function() {
            var title = $(this).val()

            if (title == "" && title.length == 0) {
                $(this).css({ "color": "#A0A0A0" });
                titlec.val("请输入标题");
            }
        });
        titlec.keyup(function() {
            $(this).css({ "color": "#ffffff" });
        });

        contentc.focus(function() {
            var title = $(this).val()
            if (title == "请输入内容") {
                contentc.val("");
            }
        });
        contentc.blur(function() {
            var title = $(this).val()

            if (title == "" && title.length == 0) {
                $(this).css({ "color": "#A0A0A0" });
                contentc.val("请输入内容");
            }
        });
        contentc.keyup(function() {
            $(this).css({ "color": "#ffffff" });
        });
        item.find("input[name=Submit22]").click(function() {

            if (ExternalIA.IsHaveName()) {

                var flag = true;
                var titlecontent = $.trim(titlec.val());
                var feedcontent = $.trim(contentc.val());
                if (titlecontent != "请输入标题") {

                    if ($.trim(titlec.val()).length == 0) {
                        flag = false;
                        var viewDiv = $('span[name=try1]');
                        viewDiv.html("请输入标题");
                        viewDiv.css({ "color": "#FF0000" });
                    }
                    if (titlecontent.length > 30) {

                        flag = false;
                    }
                } else {
                    flag = false;
                    var viewDiv = $('span[name=try1]');
                    viewDiv.html("请输入标题");
                    viewDiv.css({ "color": "#FF0000" });
                }

                if (feedcontent != "请输入内容") {
                    if (feedcontent.length == 0) {

                        flag = false;
                        var viewDiv = $('span[name=try2]');
                        viewDiv.html("请输入内容");
                        viewDiv.css({ "color": "#FF0000" });
                    }
                    if (feedcontent.length > 1000) {

                        flag = false;
                    }
                } else {
                    flag = false;
                    var viewDiv = $('span[name=try2]');
                    viewDiv.html("请输入内容");
                    viewDiv.css({ "color": "#FF0000" });
                }

                if (flag) {


                    ExternalIA.addfeedback(titlecontent, feedcontent, "1");
                }
            }
            else {

                ExternalIA.GoNameReg();
            }




        });
        if (ExternalIA.IsHaveName()) {
            titlec.removeAttr("disabled");
            titlec.val("");
            titlec.setCaretPosition(0);

        }
        else {
            titlec.val("");
            titlec.attr("disabled", "disabled");
            ExternalIA.feedbackDialog(setandfeed.feedbacknoname(), "");
        }
        //
        container.append(item);
        if ($(".namep300").is("textarea")) {
          //  alert(1);
            $(".namep300").setCaretPosition(0);
        }

    },
    feedbackSuccess: function() {
        var html = "";
        html += "<div class=\"tips tipsre\"><span><img src=\"../skin/default/images/fk.jpg\" width=\"33\" height=\"30\" /></span><span>感谢您的反馈，我们会尽快处理</span><p>3秒后返回至反馈主页</p></div>";
        var item = $(html);
        return item;
    },
    feedbacknoname: function() {
        var html = "";
        html += "<div class=\"tips tipsre tipsrd\"><span><img src=\"../skin/default/images/fk1.jpg\" /></span><span>我们发现您还未注册虚名，为了便于我们更方便更及时的处理您的反馈信息，建议您前往个人中心注册虚名，更可获得G+VIP特权.</span>";
        html += "<p>麻烦您再注册虚名之后，再将您的宝贵意见反馈给我们~</p>";
        html += "<p><a href=\"javascript:void(0)\"><input class=\"alllong\" type=\"submit\" value=\"立即注册虚名>\" name=\"Submit222\"></a></p></div>";
        var item = $(html);
        item.find("input[name=Submit222]").click(function() {
            ExternalIA.boxclose();
            ExternalIA.GoNameReg();

        });
        return item;
    },
    EidtWordChanges: function(editor, viewDiv, maxLen) {

        maxLen = maxLen || 140;
        var totalWords = maxLen - setandfeed.GetContentLengthByTinyUrl($.trim($(editor).val()));

        if (totalWords < 0) {
            $(viewDiv).html("已超出" + Math.abs(totalWords).toString() + "字");
            $(viewDiv).css({ "color": "#FF0000" });
        }
        else {
            $(viewDiv).html("还可输入" + totalWords.toString() + "字");

            $(viewDiv).css({ "color": "#A0A0A0" });
        }

    },
    GetContentLengthByTinyUrl: function(inputValue) {
        if (!inputValue)
            return 0;

        //([\w-]+\.)+[\w-]+.([^a-z])(/[\w- ./?%&=]*)?|[a-zA-Z0-9\-\.][\w-]+.([^a-z])(/[\w- ./?%&=]*)?
        var patt = new RegExp("https?\:\/\/[A-Za-z0-9\.\?\&\/\_\=\%\-]+", "gi");
        inputValue = inputValue.replace(patt, "12345678901234567890");

        return inputValue.Trim().length;
    }



}