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
    public partial class SelectRewardItem : Form
    {
        public ListBox.SelectedObjectCollection selectedRewardItems;
        private TextBox parentTextBox;
        private DataTable dt;
        private string questID;

        public SelectRewardItem()
        {
            InitializeComponent();
            LoadFromExcel();
        }

        public SelectRewardItem(TextBox tb, string questID)
        {
            InitializeComponent();
            this.parentTextBox = tb;
            this.questID = questID;

            LoadFromExcel();
        }

        /// <summary>
        /// Esc Key 처리
        /// </summary>
        /// <param name="msg"></param>
        /// <param name="keyData"></param>
        /// <returns></returns>
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            try
            {
                if (msg.WParam.ToInt32() == (int)Keys.Escape)
                    this.Close();
                else
                    return base.ProcessCmdKey(ref msg, keyData);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Key overrided Events Error: " + ex.Message);
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button_Select_Click(object sender, EventArgs e)
        {
            if ( listBox_RewardItem.SelectedItem != null)
            {
                if (listBox_RewardItem.SelectionMode == SelectionMode.One)
                {
                    string[] rewardItem = listBox_RewardItem.SelectedItem.ToString().Split(',');
                    selectedRewardItems.Add(rewardItem[0]);
                }
                else
                {
                    selectedRewardItems = listBox_RewardItem.SelectedItems;
                }
            }
        }

        private void LoadFromExcel()
        {
            string path = @"R:\Gameres\WorkSheet\Tables\QuestRewardTable.xlsb";
            string ConnectionString = @"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + path + ";Extended Properties=\"Excel 12.0;HDR=YES;IMEX=1;READONLY=TRUE;\"";

            try
            {
                using (OleDbConnection oleConnection = new OleDbConnection(ConnectionString))
                {
                    DataSet ds = new DataSet();
                    //string strSql = "Select * From [일반$]";
                    string strSql = "Select * From [일반$]";
                    using (OleDbDataAdapter adapt = new OleDbDataAdapter(strSql, oleConnection))
                        adapt.Fill(ds);
                    

                    //DATA Sheet의 모든 정보를 가져오므로, excel상의 첫번째 Row명 삭제
                    //ds.Tables[0].Rows.RemoveAt(0);
                    //ds.Tables[0].Rows.RemoveAt(1);
                    //ds.Tables[0].Rows.RemoveAt(2);
                    //ds.Tables[0].Rows.RemoveAt(3);
                    //ds.Tables[0].Rows.RemoveAt(4);
                    //ds.Tables[0].Rows.RemoveAt(5);

                    DataRow[] tmp = ds.Tables[0].Select("_questID LIKE '%" + questID + "%'");
                    int a = tmp.Length;

                    dt = ds.Tables[0];
                    foreach (DataRow dr in dt.Rows)
                    {
                        //if (dr.ItemArray.Length > 2 && dr[1].ToString() != "")
                        if( dr[0].ToString() == questID )
                            listBox_RewardItem.Items.Add(dr[0] + ":" + dr[16] + " | " + dr[1] + " | " + dr[23]);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "\n" + ex.StackTrace);
            }
        }

        private void btnSearch_Enter(object sender, EventArgs e)
        {
        }

        private void listBox_RewardItem_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            parentTextBox.Text = listBox_RewardItem.SelectedItem.ToString().Split(':')[0];
        }

    }
}