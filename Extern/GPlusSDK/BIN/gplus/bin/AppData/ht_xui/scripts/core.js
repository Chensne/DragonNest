var preNeedDealWork = null;
var nextcallback = null;
function CallFunWithCondition(funSuccessCall, funFalseCall, condition, chkLoginState, preCall) {
    try {

        if (chkLoginState) {

            if (!ExternalIA.isLogin()) {
                ExternalIA.GoUserLogin(function() {
                    preNeedDealWork = function() {
                        if (preCall) {
                            if ($.isFunction(preCall))
                                preCall();
                        }

                    }
                    nextcallback = function() {
                        if (preCall) {
                            if ($.isFunction(preCall)) {
                                condition ? funSuccessCall() : funFalseCall();
                            }
                        }
                    }
                });

                return;
            }
        }

        if (preCall) {
            if ($.isFunction(preCall))
                preCall();
        }

        if (condition) {
            funSuccessCall();
        }
        else {
            funFalseCall();
        }


    }
    catch (e) {
        alert(e);
    }
}


function transjson(pageID, containerID, isfree) {
    var strjson = "";
    if (isfree) {
        //strjson = "{\"pageID\":'" + pageID + "',\"containerID\":'" + containerID + "'}";
        strjson = "{\"pageID\":'" + pageID + "'," + containerID + "}";
        return strjson;
    }
    strjson = "{\"pageID\":'" + pageID + "',\"containerID\":'" + containerID + "'}";
    return strjson;
}

function currenttime() {
    var curr_date = Date.parse(new Date());
    var date = new Date(curr_date);
    var formatdate = date.pattern("yyyyMMddHHmmssS");
    return formatdate;
}
function getUrlParam(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = window.location.search.substr(1).match(reg);
    if (r != null) return unescape(r[2]); return null;
}

function isChinaLettNum(s) {
    
    var re = new RegExp("[ ,\\`,\\~,\\!,\\@,\#,\\$,\\%,\\^,\\+,\\*,\\&,\\\\,\\/,\\?,\\|,\\:,\\.,\\<,\\>,\\{,\\},\\(,\\),\\',\\;,\\=,\"]");


    if (!re.test(s)) {
        return false;
    }
    return true;

}



(function($) {



    //external.Util.Iconv

    String.prototype.getJSON = function() {


        var json = null;
        var trans = "";
        try {
            //  var tostr = this.UrlEncode();
            // alert(this);
            //    trans = window.external.Util.Iconv.Convert("utf-8", "", ToString(this));
            // alert(trans);
            eval("json=" + ToString(this));
        }
        catch (e) { }

        return json;
    }
    String.prototype.transgb = function() {
        var trans = "";
        alert(this);
        trans = window.external.Util.Iconv.Convert("utf-8", "", this);
        alert(trans);

        return trans;

    }
    String.prototype.transutf8 = function() {
        var trans = "";
        trans = window.external.Util.Iconv.Convert("", "utf-8", ToString(this));

        return trans;
    }

    String.prototype.Trim = function() {
        return this.replace(/(^\s*)|(\s*$)/g, "");
    }

    $.format = function(source, params) {
        if (arguments.length == 1)
            return function() {
                var args = $.makeArray(arguments);
                args.unshift(source);
                return $.format.apply(this, args);
            };
        if (arguments.length > 2 && params.constructor != Array) {
            params = $.makeArray(arguments).slice(1);
        }
        if (params.constructor != Array) {
            params = [params];
        }
        $.each(params, function(i, n) {
            source = source.replace(new RegExp("\\{" + i + "\\}", "g"), n);
        });
        return source;
    };
    $.drag = function(e) {

        var l = Math.min(Math.max(e.pageX, 102) - 102, 268);
        $(this).find(".Perce").css({ "left": l });
        var value = Math.floor((l / 268) * 100);
        var setvalue = Math.min(Math.floor(((value / 100) * 127) + 127), 255);
        setandfeed.transparency = setvalue;
        $("p").eq(0).html(value + "%");
        //  var setvalue = Math.min(Math.floor((value / 100) * 255)-125, 255);

        ExternalIA.SetAlpha(setvalue);
    }
    $.drop = function(e) {

    $(document).find(".bgimg").unbind('mousemove', $.drag).unbind('mouseup', $.drop).unbind('mouseleave', $.drop);
        // $(this).unbind('mousemove', $.drag).unbind('mouseup', $.drop);
    }



    Date.prototype.pattern = function(fmt) {
        var o = {
            "M+": this.getMonth() + 1, //月份     
            "d+": this.getDate(), //日     
            "h+": this.getHours() % 12 == 0 ? 12 : this.getHours() % 12, //小时     
            "H+": this.getHours(), //小时     
            "m+": this.getMinutes(), //分     
            "s+": this.getSeconds(), //秒     
            "q+": Math.floor((this.getMonth() + 3) / 3), //季度     
            "S": "000" //毫秒     
        };
        var week = {
            "0": "\u65e5",
            "1": "\u4e00",
            "2": "\u4e8c",
            "3": "\u4e09",
            "4": "\u56db",
            "5": "\u4e94",
            "6": "\u516d"
        };
        if (/(y+)/.test(fmt)) {
            fmt = fmt.replace(RegExp.$1, (this.getFullYear() + "").substr(4 - RegExp.$1.length));
        }
        if (/(E+)/.test(fmt)) {
            fmt = fmt.replace(RegExp.$1, ((RegExp.$1.length > 1) ? (RegExp.$1.length > 2 ? "\u661f\u671f" : "\u5468") : "") + week[this.getDay() + ""]);
        }
        for (var k in o) {
            if (new RegExp("(" + k + ")").test(fmt)) {
                fmt = fmt.replace(RegExp.$1, (RegExp.$1.length == 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
            }
        }
        return fmt;
    }




    $.fn.extend({
        insertAtCaret: function(myValue) {
            try {
                var $t = $(this)[0];
                if (document.selection) {
                    this.focus();
                    sel = document.selection.createRange();
                    sel.text = myValue;
                    this.focus();
                    $(this).val(myValue)
                }
                else
                    if ($t.selectionStart || $t.selectionStart == '0') {
                    var startPos = $t.selectionStart;
                    var endPos = $t.selectionEnd;
                    var scrollTop = $t.scrollTop;
                    $t.value = $t.value.substring(0, startPos) + myValue + $t.value.substring(endPos, $t.value.length);
                    this.focus();
                    $t.selectionStart = startPos + myValue.length;
                    $t.selectionEnd = startPos + myValue.length;
                    $t.scrollTop = scrollTop;

                    $(this).val(myValue)
                }
                else {
                    this.value += myValue;
                    this.focus();

                    $(this).val(myValue)
                }
            }
            catch (e) {
                $(this).val(myValue)
            }
        },

        FocusStart: function() {
            try {
                var $t = $(this)[0];
                $t.selectionStart = 0;
                $t.selectionEnd = 0;
                $t.focus();
            }
            catch (ex) {
                //alert(ex);
            }
        },

        setCaretPosition: function(pos) {
            try {
                var ctrl = $(this)[0];

                if (ctrl) {
                    this.keyup();

                    if (ctrl.setSelectionRange) {

                        ctrl.focus();
                        ctrl.setSelectionRange(pos, pos);
                    }
                    else if (ctrl.createTextRange) {

                        var range = ctrl.createTextRange();
                        range.collapse(true);
                        range.moveEnd('character', pos);
                        range.moveStart('character', pos);
                        range.select();
                    }
                }
            }
            catch (ex) {

            }
        }



    })
})(jQuery);