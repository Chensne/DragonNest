using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class SelectNPC : Form
    {
        public bool Portrait { get; set; }
        public string selected_npc = "";
        public string selected_npcportrait = "";
        public ListBox.SelectedObjectCollection selected_npcs;
        
        public SelectNPC(ListBox p_list)
        {
            InitializeComponent();

            foreach (object item in p_list.Items)
                listBox_NPC.Items.Add(item);

            listBox_NPC.SelectionMode = SelectionMode.One;
            Portrait = false;
        }

        public SelectNPC(TextBox tx)
        {
            InitializeComponent();
            Portrait = false;
        }

        public SelectNPC()
        {
            InitializeComponent();
            LoadFromExcel();
            listBox_NPC.SelectionMode = SelectionMode.MultiExtended;
            Portrait = false;
        }

        private string GetNpcPortrait(string npcid)
        {
            string path = @"R:\Gameres\WorkSheet\Tables\NPCTable.xlsb";
            string ConnectionString = @"Provider=Microsoft.ACE.OLEDB.12.0;Mode=Read;Data Source=" + path + ";Extended Properties=\"Excel 12.0;HDR=YES;IMEX=1;READONLY=TRUE;\"";

            try
            {
                using (OleDbConnection oleConnection = new OleDbConnection(ConnectionString))
                {
                    DataSet ds_npc = new DataSet();
                    string strSql = "Select * From [DATA$]";
                    using (OleDbDataAdapter adapt = new OleDbDataAdapter(strSql, oleConnection))
                    {
                        adapt.Fill(ds_npc);
                    }

                    //DATA Sheet의 모든 정보를 가져오므로, column명 삭제
                    ds_npc.Tables[0].Rows.RemoveAt(0);

                    DataTable dt_npc = ds_npc.Tables[0];

                    foreach (DataRow dr_npc in dt_npc.Rows)
                    {
                        if (dr_npc.ItemArray.Length > 2)
                        {
                            if (dr_npc[6].ToString() == npcid)
                            {
                                return dr_npc[12].ToString();
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "\n" + ex.StackTrace);
            }
            return npcid;
        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button_Select_Click(object sender, EventArgs e)
        {
            if (listBox_NPC.SelectedItem != null)
            {
                if (listBox_NPC.SelectionMode == SelectionMode.One)
                {
                    string[] npc = listBox_NPC.SelectedItem.ToString().Split(',');
                    selected_npc = npc[1];

                    if (Portrait)
                    {
                        selected_npcportrait = GetNpcPortrait(npc[0]);
                    }
                }
                else
                {
                    selected_npcs = listBox_NPC.SelectedItems;
                }
            }
        }

        private void LoadFromExcel()
        {
            string path = @"R:\Gameres\WorkSheet\Tables\NPCTable.xlsb";
            string ConnectionString = @"Provider=Microsoft.ACE.OLEDB.12.0;Mode=Read;Data Source=" + path + ";Extended Properties=\"Excel 12.0;HDR=YES;IMEX=1;READONLY=TRUE;\"";

            try
            {
                using (OleDbConnection oleConnection = new OleDbConnection(ConnectionString))
                {
                    DataSet ds_npc = new DataSet();
                    string strSql = "Select * From [DATA$]";
                    using (OleDbDataAdapter adapt = new OleDbDataAdapter(strSql, oleConnection)) {
                        adapt.Fill(ds_npc);
                    }

                    //DATA Sheet의 모든 정보를 가져오므로, column명 삭제
                    ds_npc.Tables[0].Rows.RemoveAt(0);

                    DataTable dt_npc = ds_npc.Tables[0];

                    foreach (DataRow dr_npc in dt_npc.Rows)
                    {
                        if (dr_npc.ItemArray.Length > 2)
                        {
                            if (dr_npc[1].ToString() != "")
                            {
                                string _talkFile =
                                    dr_npc[11].ToString().IndexOf('.') < 0 ?
                                    dr_npc[11].ToString() :
                                    dr_npc[11].ToString().Substring(0, dr_npc[11].ToString().LastIndexOf('.'));

                                listBox_NPC.Items.Add(dr_npc[6].ToString() + "," + _talkFile + "," + dr_npc[7] + "," + dr_npc[2]);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show( ex.Message + "\n" + ex.StackTrace );
            }
        }

        private void SelectNPC_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.Equals(Keys.Escape))
                this.Close();
        }

        private void listBox_NPC_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //listBox_NPC.SelectedItem을 parent의 listBox에 추가
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
