var uservip = {
    contentpage: "midlist",
    mainpage: "mainpage",
    bannerPlaceHolder: "bannerPlaceHolder",
    getcontentpage: function() {
        return $("#" + uservip.contentpage);
    },
    getmainpage: function() {
        return $("#" + uservip.mainpage);
    },
    getbanner: function() {
        return $("#" + uservip.bannerPlaceHolder);
    },
    loaduservip: function() {
        uservip.appendheaduservip();
    },
    appendheaduservip: function() {
        var container = uservip.getcontentpage();
        var head = $("<div class=\"titleA1\">G+ VIP特权 <span class=\"b12\">（已有</span><span class=\"tit12\">10000</span><span class=\"b12\">人享有VIP特权）</span></div>");

        container.append(head);
        if (!ExternalIA.IsHaveName()) {
            uservip.nonameprompteffect(container);
        }
        uservip.appendvipdetailall(container)
    },
    nonameprompteffect: function(container) {

        var prompt = $("<div class=\"tips\"><span class=\"iptita1\">你尚未激活VIP特权，无法享受VIP特权。</span><a href=\"javascript:void(0)\" class=\"tit\">马上注册虚名享受VIP特权></a></div>");
        prompt.find("a").click(function() {
            ExternalIA.GoNameReg();
        });
        container.append(prompt);
    },
    appendvipdetailall: function(container) {
        var html = "";
        if (ExternalIA.IsHaveName()) {
            html += "<div class=\"VIPmid\">";
        } else {
            html += "<div class=\"VIPmid VIPmid1\">";
        }

        html += "<ul>";

        var items = [1];
        $.each(items, function() {
            html += uservip.genericappendvipdetail();
        });
        html += "<ul>";
        html += "</div>";
        var item = $(html);
        item.find("a[type=receive]").find(".all").click(function() {
            $(this).removeClass("all");
            $(this).addClass("allh");
        });
        container.append(item);

    },
    genericappendvipdetail: function() {
        //allh
        var html = "";
        //  html += "<li><span class=\"color color1\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color1\"></span><div class=\"txt\"><span class=\"m1\">VIP1</span>1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color2\"></span>";
        html += "<div class=\"txt\"><span class=\"m2 tit\">VIP2</span><span  class=\"lw\"><img src=\"../skin/default/images/img.jpg\" width=\"27\" height=\"27\" /></span><span class=\"tit lq\">虚拟熊猫一只</span><span class=\"b14\">";
        if (ExternalIA.IsHaveName()) {
            html += "<a href=\"javascript:void(0)\" type=\"receive\" title=\"领取对应的奖励\">";
        }
        else {
            html += "<a href=\"javascript:void(0)\" title=\"您尚未激活VIP,无法领取VIP2特权。注册虚名可激活VIP\">";
        }
        if (ExternalIA.IsHaveName()) {
            html += "<input type=\"submit\" name=\"Submit2\" value=\"领取\"  class=\"all\"/>";
        } else {
        html += "<input type=\"submit\" name=\"Submit2\" value=\"领取\"  class=\"allh\"/>";
        }
       
        html += "</a></span></div>";
        html += "</li>";
        html += "<li><span class=\"color color1\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color2\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        html += "<li><span class=\"color color3\"></span><div class=\"txt\">1. 在线经验值获得+50%  2. 商城购物7折优惠</div></li>";
        return html;
    }
}