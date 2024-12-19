using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;

namespace QuestTool
{
    public partial class QuestPathSetting : Form
    {
        public QuestPathSetting()
        {
            InitializeComponent();

            Get_RegistryValue();
        }

        private void Get_RegistryValue()
        {
            Quest_Common qc = new Quest_Common();

            textBox_QuestDefault.Text = qc.DefaultQuestPath;
            textBox_QuestKor.Text = qc.KoreaQuestPath;
            textBox_QuestJpn.Text = qc.JapanQuestPath;
            textBox_QuestChn.Text = qc.ChinaQuestPath;
            textBox_QuestUsa.Text = qc.USAQuestPath;
            textBox_QuestTwn.Text = qc.TaiwanQuestPath;
            textBox_QuestSin.Text = qc.SingaporeNMalaysiaQuestPath;
         
            textBox_QuestTha.Text = qc.ThailandQuestPath;
            textBox_QuestIdn.Text = qc.IndonesiaQuestPath;
            textBox_QuestRus.Text = qc.RusiaQuestPath;
            textBox_QuestEU.Text = qc.EuropeQuestPath;

            textBox_NpcDefault.Text = qc.DefaultNPCPath;
            textBox_NpcKor.Text = qc.KoreaNPCPath;
            textBox_NpcJpn.Text = qc.JapanNPCPath;
            textBox_NpcChn.Text = qc.ChinaNPCPath;
            textBox_NpcUsa.Text = qc.USANPCPath;
            textBox_NpcTwn.Text = qc.TaiwanNPCPath;
            textBox_NpcSin.Text = qc.SingaporeNMalaysiaNPCPath;

            textBox_NpcTha.Text = qc.ThailandNPCPath;
            textBox_NpcIdn.Text = qc.IndonesiaNPCPath;
            textBox_NpcRus.Text = qc.RusiaNPCPath;
            textBox_NpcEU.Text = qc.EuropeNPCPath;
        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button_Save_Click(object sender, EventArgs e)
        {
            string regSubkey = "Software\\DNQuestTool";

            RegistryKey rk = Registry.LocalMachine.OpenSubKey(regSubkey, true);

            // 없으면 서브키를 만든다.
            if (rk == null)
            {
                // 해당이름으로 서브키 생성
                rk = Registry.LocalMachine.CreateSubKey(regSubkey);                
            }
            
            rk.SetValue("_DefaultQuestPath", textBox_QuestDefault.Text);
            rk.SetValue("_KoreaQuestPath", textBox_QuestKor.Text);
            rk.SetValue("_JapanQuestPath", textBox_QuestJpn.Text);
            rk.SetValue("_ChinaQuestPath", textBox_QuestChn.Text);
            rk.SetValue("_USAQuestPath", textBox_QuestUsa.Text);
            rk.SetValue("_TaiwanQuestPath", textBox_QuestTwn.Text);
            rk.SetValue("_SingaporeNMalaysiaQuestPath", textBox_QuestSin.Text);
            rk.SetValue("_ThailandQuestPath", textBox_QuestTha.Text);
            rk.SetValue("_IndonesiaQuestPath", textBox_QuestIdn.Text);
            rk.SetValue("_RusiaQuestPath", textBox_QuestRus.Text);
            rk.SetValue("_EuropeQuestPath", textBox_QuestEU.Text);

            rk.SetValue("_DefalutNPCPath", textBox_NpcDefault.Text);
            rk.SetValue("_KoreaNPCPath", textBox_NpcKor.Text);
            rk.SetValue("_JapanNPCPath", textBox_NpcJpn.Text);
            rk.SetValue("_ChinaNPCPath", textBox_NpcChn.Text);
            rk.SetValue("_USANPCPath", textBox_NpcUsa.Text);
            rk.SetValue("_TaiwanNPCPath", textBox_NpcTwn.Text);
            rk.SetValue("_SingaporeNMalaysiaNPCPath", textBox_NpcSin.Text);
            rk.SetValue("_ThailandNPCPath", textBox_NpcTha.Text);
            rk.SetValue("_IndonesiaNPCPath", textBox_NpcIdn.Text);
            rk.SetValue("_RusiaNPCPath", textBox_NpcRus.Text);
            rk.SetValue("_EuropeNPCPath", textBox_NpcEU.Text);

            this.Close();
        }

        private void button_QuestDefault_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestDefault);            
        }

        private void Select_Path(TextBox p_textbox)
        {
            FolderBrowserDialog fbd_path = new FolderBrowserDialog();

            if (fbd_path.ShowDialog() == DialogResult.OK)
            {
                p_textbox.Text = fbd_path.SelectedPath;
            }
        }

        private void button_QuestKor_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestKor);          
        }

        private void button_QuestChn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestChn);  
        }

        private void button_QuestJpn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestJpn);  
        }

        private void button_QuestUsa_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestUsa);  
        }
        private void button_QuestTwn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestTwn);
        }

        private void button_QuestSin_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestSin);
        }


        private void button_NPCDefault_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcDefault);  
        }

        private void button_NPCKor_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcKor);  
        }

        private void button_NPCChn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcChn);  
        }

        private void button_NPCJpn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcJpn);  
        }

        private void button_NPCUsa_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcUsa);  
        }

        private void button_NpcTwn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcTwn);
        }

        private void button_NpcSin_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcSin);
        }


        private void button_QuestTha_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestTha);
        }

        private void button_QuestIdn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestIdn);
        }

        private void button_QuestRus_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestRus);
        }

        private void button_QuestEU_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_QuestEU);
        }

        private void button_NpcTha_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcTha);
        }

        private void button_NpcIdn_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcIdn);
        }

        private void button_NpcRus_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcRus);
        }

        private void button_NpcEU_Click(object sender, EventArgs e)
        {
            Select_Path(textBox_NpcEU);
        }
        
    }
}
