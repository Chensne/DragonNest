var MessageMgr = {
    contentpage: "midlist",
    mainpage: "mainpage",
    newcout: 4,
    page: 0,
    curentstat: 0,
    pagecout: 1,
    bannerPlaceHolder: "bannerPlaceHolder",
    getcontentpage: function() {

        var page = $("#" + GIndexMgr.contentpage);
        if (!page.hasClass("midlistop")) {
            page.addClass("midlistop");
        }
        return $("#" + GIndexMgr.contentpage);
    },
    getmainpage: function() {
        return $("#" + GIndexMgr.mainpage);
    },
    getbanner: function() {
        return $("#" + GIndexMgr.bannerPlaceHolder);
    },
    appendmessageimage: function(img, messageid) {
        var html = "";

        html += "<img src=\"" + img + "\" class=\"img\" / >";
        var item = $(html);
        var newsid = $("div[newsid=" + messageid + "]");
        if (newsid.is("div")) {

            var right = newsid.find(".right");
            right.html("");
            right.append(item);

        }
        //        item.css({
        //            "filter": "gray"
        //        });

    },
    appendmessagedetail: function(message, container, callback) {
        //        if (Setting.currentIsMessage()) {
        //            Other_ClearContainer();
        //        }
        var html = "";
        html += "<div class=\"news\" newsid=\"" + message.ID + "\">";
        html += "<div class=\"list\">";
        html += "<ul>";
        html += "<li class=\"right\">";
        if (message.IconUrl) {
            html += "<img src=\"" + message.IconUrl + "\" class=\"img\" / >";
        }
        //"<img src=\"../skin/default/images/img.jpg\" class=\"img\" / >";
        // html += "<p><img src=\"../skin/default/images/Heart.jpg\" /></p>";
        // html += "<p class=\"red\">132</p>";
        // html += "<p class=\"peple\">人</p>";
        html += "</li>";
        html += "<li class=\"detail\">";
        if (message.Title) {
            if (message.Title.length > 28) {

                html += "<a href=\"#\" class=\"h1\" type=\"detail\" newsid=\"" + message.ID + "\">" + message.Title.substring(0, 28) + "......";
            } else {
                html += "<a href=\"#\" class=\"h1\" type=\"detail\" newsid=\"" + message.ID + "\">" + message.Title;
            }
        } else {
            html += "<a href=\"#\" class=\"h1\" type=\"detail\" newsid=\"" + message.ID + "\">" + message.Title;
        }
        if (message.Summary) {
            if (message.Summary.length > 80) {

                html += "<p class=\"h2\">" + message.Summary.substring(0, 80) + "......</p></a>";
            }
            else {
                html += "<p class=\"h2\">" + message.Summary + "</p></a>";
            }
        } else {
            html += "<p class=\"h2\">" + message.Summary + "</p></a>";
        }

        html += "<p class=\"time\">" + DateDeserialize(message.UpdateTime) + "</p>";
        html += "</li>";

        html += "</ul>";
        html += "</div>";
        html += "</div>";
        var item = $(html);
        if (message.IsNew == true) {
            item.find("a[newsid='" + message.ID + "']").css({ "color": "#F39800" });
            item.find(".h2").css({ "color": "#d2d2d2" });
        }
        else {
            item.find("a[newsid='" + message.ID + "']").css({ "color": "#9D9D9D" });
            item.find(".h2").css({ "color": "#959595" });
            item.find(".img").css({
                "filter": "gray"
            });


        }
        item.mouseenter(function() {
            if (message.IsNew == true) {
                //  alert(1);
                item.find(".h2").css({ "color": "#FFF" });
            }
            else {
                //   alert(2);
                item.find(".h2").css({ "color": "#959595" });
            }
        });
        item.mouseleave(function() {
            if (message.IsNew == true) {
                item.find(".h2").css({ "color": "#d2d2d2" });
            }
            else {
                item.find(".h2").css({ "color": "#959595" });
            }
        });
        item.find("a[type=detail]").click(function() {

            ExternalIA.GoMessageDetail($(this).attr("newsid"), 0);
        });
        container.append(item);
        if (callback) {
            eval(callback);
        }

    },
    messageappend: function(source, callback) {
        Other_ClearContainer();
        var container = MessageMgr.getcontentpage();
        var last = "";
        if (source) {
            if (source.length > 0) {
                $.each(source, function() {
                    MessageMgr.appendmessagedetail(this, container); //, "ExternalIA.GetMessage(" + this.ID + ", 0)");

                    last = this;
                });
                // if (source.length < MessageMgr.newcout) {
                MessageMgr.pagination(last, container);

                //   }

            } else {
                container.append($("<div><br/>&nbsp;&nbsp;暂时没有消息</<div>"));
            }
        } else {
            container.append($("<div><br/>&nbsp;&nbsp;暂时没有消息</<div>"));
        }


    },
    pagination: function(source, container, css) {
      //  alert(MessageMgr.curentstat);
        var num = 1;
        var html = "";

        if (css) {
            html += "<div class=\"nextpage " + css + "\">";
        } else {
            html += "<div class=\"nextpage\">";
        }


        if (Setting.currentIsMessage()) {
            html += "<a href=\"javascript:void(0)\" page=" + MessageMgr.page + " type=\"up\" class=\"uplist\" newsid=\"" + source.ID + "\">";
            num = Math.floor(MessageMgr.page / 4) + 1;
            html += "第" + num + "页";
        }
        if (Setting.currentIsMessageDetail()) {
            if (MessageMgr.curentstat != 0) {
                html += "<a href=\"javascript:void(0)\" type=\"up\" class=\"uplist\" newsid=\"" + source.ID + "\">";
                html += "上一条";
            }
        }

        html += "</a><a class=\"x\" href=\"javascript:void(0)\">/</a>";

        if (Setting.currentIsMessage()) {
            html += "<a class=\"nextlist\" page=" + MessageMgr.page + " type=\"next\" href=\"javascript:void(0)\" newsid=\"" + source.ID + "\">";
            //   num = Math.floor(MessageMgr.page / 4) + 2;
            //   html += "第" + num + "页";
            html += "共" + MessageMgr.pagecout + "页";
        }
        if (Setting.currentIsMessageDetail()) {
            if (MessageMgr.curentstat != 2) {
                html += "<a class=\"nextlist\" type=\"next\" href=\"javascript:void(0)\" newsid=\"" + source.ID + "\">";
                html += "下一条";
            }
        }
        html += "</a></div>";
        var item = $(html);
        //  ExternalIA.GoMessage();
        container.append(item);

    },
    genericappendmessagedetail: function(source, callback) {
        var container = $("#" + GIndexMgr.contentpage);
        var html = "<div class=\"titleA2\"><a type=\"backpage\" href=\"javascript:void(0)\">返回</a>";
        if (source.Title.length > 24) {
            html += source.Title.substring(0, 24) + ".....";
        }
        else {
            html += source.Title;
        }

        html += "<span class=\"from\">" + DateDeserialize(source.UpdateTime) + "</span>";
        html += "</div>";
        var item = $(html);
        item.find("a[type=backpage]").click(function() {
            ExternalIA.GoMessage();
        });
        container.append(item);
        var containerID = $("<div class=\"newmid\">");

        if (source.Content) {
            MessageMgr.genericmessagecontent(source.Content, containerID);
        }
        container.append(containerID);
        ExternalIA.UpdateMessageInfo();
        MessageMgr.pagination(source, container, "nextpagebg");
    },
    genericmessagecontent: function(content, container) {
        var html = "";

        //        html += "<p>各位亲爱的玩家：</p>";
        //        html += "<p> “绿龙·灾变”手动补丁包现已开放下载！由于本次版本更新内容较多，手动补丁包较大（150MB以上），为避免更新当日自动更新较慢，请各位谷迷提前下载，并在21日版本更新过程中或完成后进行更新，以便第一时间体验“绿龙·灾变”版本的精彩！</p>";
        //        html += "<p> 12月21日版本更新后可用的“绿龙·灾变”最新客户端下载信息（具体可用时间请参考所在大区维护计划）：</p>";
        //        html += "<p> 完整客户端大小：2.49 GB</p>";
        //        html += "<p> DragonNest_Setup_2.0.7.100.exe文件MD5码：2cc5a42e91bf32368a9b6ab425db1282</p>";
        //        html += "<p> dndata100.zip文件MD5码：48c136f4a4a62f5ff100b3ac321db420</p>";
        //        html += "<p> HTTP分卷一(DragonNest_Setup_2.0.7.100.exe)&gt;&gt;&gt;      HTTP分卷二(dndata100.zip)&gt;&gt;&gt;</p>";
        //        html += "<p> HTTP下载方式说明：请务必将下载好的所有文件（EXE文件和ZIP文件）全部下载至一个目录文件夹，安装时请直接运行“DragonNest_Setup_x.x.x.x.exe”（x.x.x.x代表文件版本号），无需解压dndataXX.zip（XX代表文件版本号）文件。</p>";
        //        html += "<p> BT下载地址&gt;&gt;&gt;</p>";
        //        html += "<p> 12月21日版本更新后可用的“绿龙·灾变”最新手动补丁包下载信息（具体可用时间请参考所在大区维护计划）：</p>";
        //        html += "<p> 下载地址&gt;&gt;&gt;</p>";
        //        html += "<p>------------------------------------------------------------</p>";
        //        html += "<p> 当前可用的“灾变·前夜”客户端下载地址：</p>";
        //        html += "<p> 下载地址&gt;&gt;&gt;</p>";
        //        html += "<p> 感谢大家对《龙之谷》的支持与理解，祝各位玩家游戏愉快！</p>";
        //        html += "<p>";
        html += "<p>";

        html += content;
        html += "</p>";
        var item = $(html);
        container.append(item);

    }
}