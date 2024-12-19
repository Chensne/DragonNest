using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.OleDb;

namespace DNQuest
{
    public partial class SelectItem : Form
    {
        public ListBox.SelectedObjectCollection selectedItems;
        private string excelPath = @"R:\Gameres\WorkSheet\Tables\ItemTable(퀘스트).xlsb";
        private TextBox parentTextBox;

        public SelectItem()
        {
            InitializeComponent();
            LoadItemFromExcel();
        }

        public SelectItem(TextBox tb)
        {
            InitializeComponent();
            LoadItemFromExcel();
            this.parentTextBox = tb;
        }

        private void LoadItemFromExcel()
        {
            string ConnectionString = @"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + excelPath + ";Extended Properties=\"Excel 12.0;HDR=YES;IMEX=1;READONLY=TRUE;\"";

            string strSql = "Select * From [퀘스트$]";

            try
            {
                using (OleDbConnection oleConnection = new OleDbConnection(ConnectionString))
                {
                    DataSet ds = new DataSet();
                    using (OleDbDataAdapter adapt = new OleDbDataAdapter(strSql, oleConnection))
                    {
                        adapt.Fill(ds);

                        //DATA Sheet의 모든 정보를 가져오므로, excel상의 첫번째 Row명 삭제
                        ds.Tables[0].Rows.RemoveAt(0);

                        foreach (DataRow dr in ds.Tables[0].Select("UIString_NameID LIKE '%" + listBox_Item.Text + "%'"))
                        {
                            if (dr.ItemArray.Length > 1 && dr[1].ToString() != "")
                                listBox_Item.Items.Add(dr[4] + ":" + dr[2] + "-" + dr[3]);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "\n" + ex.StackTrace);
            }
        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void listBox_Item_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            parentTextBox.Text = listBox_Item.SelectedItem.ToString();
        }

        private void button_Select_Click(object sender, EventArgs e)
        {
            parentTextBox.Text = listBox_Item.SelectedItems.ToString().Split(':')[0];
        }

        private void btnSearch_Enter(object sender, EventArgs e)
        {
            this.LoadItemFromExcel();
        }
    }
}
