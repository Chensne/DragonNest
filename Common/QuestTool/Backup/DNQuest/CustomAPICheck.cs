using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace DNQuest
{
    class CustomAPICheck
    {
        public static string CheckListFilePath = @"c:\CustomScriptCheck.dat";
        public static Dictionary<string, string> checkList = new Dictionary<string, string>();

        static CustomAPICheck()
        {
            try
            {
                using (StreamReader objInput = new StreamReader(CheckListFilePath, System.Text.Encoding.Default))
                {

                    string row = "";
                    while ((row = objInput.ReadLine()) != null)
                    {
                        string[] splitStrings = System.Text.RegularExpressions.Regex.Split(row, "\\s+", RegexOptions.None);
                        if (!checkList.ContainsKey(splitStrings[0]))
                        {
                            checkList.Add(splitStrings[0], splitStrings[1]);
                        }
                    }

                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.StackTrace);
            }
        }

        public static string compareCheckList(string value )
        {
            if (value.Contains("("))
            {
                string strAPI = value.Substring(0, value.IndexOf("("));
                if (checkList.ContainsKey(strAPI))
                {
                    value = value.Replace(strAPI, checkList[strAPI]);
                }
            }

            return value;
        }

        public static string InsertParameter(string value)
        {
            Regex pattern = new Regex(@"(function)\s\w+\s?\(|(api_)\w+\s?\(|.*_OnTalk_.*\s?\(", RegexOptions.IgnoreCase);
            Match m;
            string gameServerParameter = " pRoom, ";

            if (value.Contains("("))
            {
                for (m = pattern.Match(value); m.Success; m = m.NextMatch())
                {
                    value = value.Replace(m.Groups[0].Value, m.Groups[0].Value + gameServerParameter);
                }
            }

            return value;
        }
    }
}
