using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.OleDb;

namespace DNQuest.Popup
{
    public partial class SelectDds : Form
    {
        public ListBox.SelectedObjectCollection selectedItems;
        private string excelPath = @"R:\Plan\1.기획서\퀘스트\제작관리\(발주) 퀘스트이미지_리스트.xls";
        //private ListBox parentListBox;

        public SelectDds()
        {
            InitializeComponent();
        }

        private void LoadItemFromExcel()
        {
            string ConnectionString = @"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + excelPath + ";Extended Properties=\"Excel 12.0;HDR=YES;\"";

            string strSql = "Select * From [퀘스트$]";

            try
            {
                using (OleDbConnection oleConnection = new OleDbConnection(ConnectionString))
                {
                    DataSet ds = new DataSet();
                    using (OleDbDataAdapter adapt = new OleDbDataAdapter(strSql, oleConnection))
                    {
                        adapt.Fill(ds);
                    }

                    //DATA Sheet의 모든 정보를 가져오므로, excel상의 첫번째 Row명 삭제
                    ds.Tables[0].Rows.RemoveAt(0);

                    DataTable dt = ds.Tables[0];
                    foreach (DataRow dr in dt.Rows)
                    {
                        if (dr.ItemArray.Length > 1 && dr[1].ToString() != "")
                            listBox_Dds.Items.Add(dr[4] + ":" + dr[2] + "-" + dr[3]);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "\n" + ex.StackTrace);
            }
        }
    }
}
