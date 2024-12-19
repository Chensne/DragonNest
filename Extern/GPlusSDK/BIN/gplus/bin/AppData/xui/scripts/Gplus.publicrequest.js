var openInterface = {

    // Initialize options with default values

    PublicRequest: function(sid, httpurl, data, type, timeout, callback) {


        //    alert(httpurl);
        //        var http = window.external.System.IO.Http;

        //        var taskid = http.NewTask();
        //        http.SetSid(taskid, "mysid");
        //        http.SetURL(taskid, encodeURI(httpurl));
        //        http.SetMethod(taskid, type);
        //        http.SetEnableGZIP(taskid, false);
        //        http.SetEncode(taskid, "utf-8");
        //        
        //        http.SetResponseCallcack(taskid, callback);
        //       
        //    
        //        http.SetTimeout(taskid, timeout);

        //        http.SendRequest(taskid);



        var http = window.external.System.IO.Http;
        var taskid = http.NewTask();
        if (sid) {
            http.SetSid(taskid, sid);
        }
        http.SetURL(taskid, encodeURI(httpurl));

        http.SetMethod(taskid, type);
        http.SetTimeout(taskid, timeout);
        http.SetResponseCallcack(taskid, callback);

        //   http.AppendStringPostData(taskid, data.transutf8());
        http.AppendStringPostData(taskid, data);
        http.AppendHeader(taskid, "Content-Type:application/json,charset=utf-8\r\n");
        http.AppendHeader(taskid, "Accept-Encoding: gzip\r\n");
        http.SendRequest(taskid);

    }




}

