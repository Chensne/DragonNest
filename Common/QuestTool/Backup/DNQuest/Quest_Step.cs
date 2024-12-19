using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_Step : UserControl
    {
        public int Select_Count = 0;
        private int m_nBaseHeight = 0;
        private int m_StyleIndex = 0;

        public Quest_NPC qn;

        public Quest_Step(Quest_NPC p_qn)
        {
            InitializeComponent();
            qn = p_qn;
            m_nBaseHeight = groupBox_Step.Height - 18;
            comboBox_Function.SelectedIndex = 0;
            comboBox_Step.SelectedIndex = 0;
        }

        public Quest_Step(Quest_NPC p_qn, string p_script, string p_stepno)
        {
            InitializeComponent();

            qn = p_qn;
            textBox_Script.Text = p_script;
            textBox_StepNo.Text = p_stepno;
            m_nBaseHeight = groupBox_Step.Height - 18;
            comboBox_Function.SelectedIndex = 0;
            comboBox_Step.SelectedIndex = 0;
        }

        public void Set_Vaule(string p_script, string p_stepno)
        {
            textBox_Script.Text = p_script;
            textBox_StepNo.Text = p_stepno;
        }

        public void Add_Select(Quest_SelectControl p_select)
        {
            p_select.Location = new Point(110, m_nBaseHeight + (p_select.Height * Select_Count));
            p_select.Name = "select_" + Select_Count;
            p_select.Tag = Select_Count;

            this.Size = new Size(this.Size.Width, this.Size.Height + p_select.Height);
            groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height + p_select.Height);

            qn.Size = new Size(qn.Size.Width, qn.Size.Height + p_select.Height);
            qn.groupBox_NPC.Height = qn.groupBox_NPC.Height + p_select.Height;

            for (int i = 0; i < qn.Controls.Count; ++i)
            {
                if (qn.Controls[i].Name.IndexOf("step_init") < 0 && qn.Controls[i].Name.IndexOf("step_") >= 0)
                {
                    int controlNo = Convert.ToInt16(qn.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("step_init") < 0)
                        thisNo = Convert.ToInt16(this.Tag);
                }
            }

            qn.qu.Size = new Size(qn.qu.Size.Width, qn.qu.Size.Height + p_select.Height);
            qn.qu.groupBox_Unit.Height = qn.qu.groupBox_Unit.Height + p_select.Height;

            for (int i = 0; i < qn.qu.Controls.Count; ++i)
            {
                if (qn.qu.Controls[i].Name.IndexOf("npc_init") < 0 && qn.qu.Controls[i].Name.IndexOf("npc_") >= 0)
                {
                    int controlNo = Convert.ToInt16(qn.qu.Controls[i].Tag);
                    int thisNo = -1;

                    if (qn.Name.IndexOf("init") < 0)
                        thisNo = Convert.ToInt16(qn.Tag);
                }
            }

            groupBox_Step.Controls.Add(p_select);

            Select_Count++;

            Set_DelFunctionButton(p_select.GetSelectType());
            if (0 < p_select.GetSelectType())
            {
                button_SelectAdd.Enabled = false;
                button_SelectDel.Enabled = false;
            }
        }

        public Quest_SelectControl[] Get_Select()
        {
            Quest_SelectControl[] ret = new Quest_SelectControl[this.Controls["groupBox_Step"].Controls.Count];

            for (int i = 0; i < this.Controls["groupBox_Step"].Controls.Count; ++i)
            {
                if (this.Controls["groupBox_Step"].Controls[i].Name.IndexOf("select_") > -1)
                {
                    ret[i] = (Quest_SelectControl)this.Controls["groupBox_Step"].Controls[i];
                }
            }

            return ret;
        }       

        public string Get_Script()
        {            
            string temp = "<![CDATA[<html>" + this.textBox_Script.Text.Replace("<P>", "").Replace("</P>", "<BR>").Replace("&nbsp;", " ").Replace("<html>", "").Replace("</html>", "") + "</html>]]>";
            return temp.Replace("<BR></html>", "</html>");
        }

        public string Get_StepNo()
        {
            return this.textBox_StepNo.Text;
        }

        public string Get_Image()
        {
            return this.textBox_Image.Text;
        }

        public string Get_ItemIndex()
        {
            return this.txtItemIndex.Text;
        }

        private void Add_Select(int type)
        {
            Quest_SelectControl qs = new Quest_SelectControl(type);

            qs.Location = new Point(110, m_nBaseHeight + (qs.Height * Select_Count));
            qs.Name = "select_" + Select_Count;
            qs.Tag = Select_Count;

            this.Size = new Size(this.Size.Width, this.Size.Height + qs.Height);
            groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height + qs.Height);

            qn.Size = new Size(qn.Size.Width, qn.Size.Height + qs.Height);
            qn.groupBox_NPC.Height = qn.groupBox_NPC.Height + qs.Height;

            for (int i = 0; i < qn.Controls.Count; ++i)
            {
                if (qn.Controls[i].Name.IndexOf("step_init") < 0 && qn.Controls[i].Name.IndexOf("step_") >= 0)
                {
                    int controlNo = Convert.ToInt16(qn.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("step_init") < 0)
                    {
                        thisNo = Convert.ToInt16(this.Tag);
                    }

                    if (controlNo > thisNo)
                    {
                        qn.Controls[i].Location = new Point(qn.Controls[i].Location.X, qn.Controls[i].Location.Y + qs.Height);
                    }
                }
            }

            Quest_Unit parent = (Quest_Unit)qn.Parent;

            parent.Size = new Size(parent.Size.Width, parent.Size.Height + qs.Height);
            parent.groupBox_Unit.Height = parent.groupBox_Unit.Height + qs.Height;

            for (int i = 0; i < parent.Controls.Count; ++i)
            {
                if (parent.Controls[i].Name.IndexOf("npc_init") < 0 && parent.Controls[i].Name.IndexOf("npc_") >= 0)
                {
                    int controlNo = Convert.ToInt16(parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (qn.Name.IndexOf("init") < 0)
                    {
                        thisNo = Convert.ToInt16(qn.Tag);
                    }

                    if (controlNo > thisNo)
                    {
                        parent.Controls[i].Location = new Point(parent.Controls[i].Location.X, parent.Controls[i].Location.Y + qs.Height);
                    }
                }
            }

            groupBox_Step.Controls.Add(qs);

            Select_Count++;
        }        

        private void button_SelectAdd_Click(object sender, EventArgs e)
        {
            if (4 <= Select_Count)
            {
                MessageBox.Show("더 이상 만들 수 없습니다!");
                return;
            }

            Add_Select(0);
            comboBox_Function.Enabled = false;
        }

        private void Delete_Select()
        {
            Quest_SelectControl qs = new Quest_SelectControl(0);
            if (Select_Count > 0)
            {
                this.Size = new Size(this.Size.Width, this.Size.Height - qs.Height);
                groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height - qs.Height);

                qn.Size = new Size(qn.Size.Width, qn.Size.Height - qs.Height);
                qn.groupBox_NPC.Height = qn.groupBox_NPC.Height - qs.Height;

                for (int i = 0; i < qn.Controls.Count; ++i)
                {
                    if (qn.Controls[i].Name.IndexOf("step_init") < 0 && qn.Controls[i].Name.IndexOf("step_") >= 0)
                    {
                        int controlNo = Convert.ToInt16(qn.Controls[i].Tag);
                        int thisNo = -1;

                        if (this.Name.IndexOf("step_init") < 0)
                        {
                            thisNo = Convert.ToInt16(this.Tag);
                        }

                        if (controlNo > thisNo)
                        {
                            qn.Controls[i].Location = new Point(qn.Controls[i].Location.X, qn.Controls[i].Location.Y - qs.Height);
                        }
                    }
                }

                Quest_Unit parent = (Quest_Unit)qn.Parent;

                parent.Size = new Size(parent.Size.Width, parent.Size.Height - qs.Height);
                parent.groupBox_Unit.Height = parent.groupBox_Unit.Height - qs.Height;

                for (int i = 0; i < parent.Controls.Count; ++i)
                {
                    if (parent.Controls[i].Name.IndexOf("npc_init") < 0 && parent.Controls[i].Name.IndexOf("npc_") >= 0)
                    {
                        int controlNo = Convert.ToInt16(parent.Controls[i].Name.Substring(parent.Controls[i].Name.IndexOf('_') + 1));
                        int thisNo = -1;

                        if (qn.Name.IndexOf("npc_init") < 0)
                        {
                            thisNo = Convert.ToInt16(qn.Tag);
                        }

                        if (controlNo > thisNo)
                        {
                            parent.Controls[i].Location = new Point(parent.Controls[i].Location.X, parent.Controls[i].Location.Y - qs.Height);
                        }
                    }
                }


                groupBox_Step.Controls["select_" + (Select_Count - 1)].Dispose();

                Select_Count--;

                if (0 == Select_Count)
                {
                    comboBox_Function.Show();
                    comboBox_Function.Enabled = true;
                }
            }
        }

        private void button_SelectDel_Click(object sender, EventArgs e)
        {
            Delete_Select();
        }

        private void button_StepAdd_Click(object sender, EventArgs e)
        {
            if (this.Tag != null)
                qn.Insert_SubStep(this.Name, this.Tag.ToString());
            else
                qn.Insert_SubStep(this.Name, "init");
        }

        private void button_DelStep_Click(object sender, EventArgs e)
        {
            if (this.Name.IndexOf("_init") < 0 && this.Name != "step_0" && this.Name.IndexOf("step_") > -1)
            {
                if (this.Tag != null)
                    qn.Del_SubStep(this.Name, this.Tag.ToString());
                else
                    qn.Del_SubStep(this.Name, "init");
            }
            else
            {
                MessageBox.Show("첫단계는 삭제할 수 없습니다.");
            }
        }

        private void textBox_Image_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {                    
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    textBox_Image.Text = textBox_Image.Text.Substring(0, textBox_Image.Text.Length - 1);
                    textBox_Image.SelectionStart = textBox_Image.Text.Length;
                }
            }
        }

        private void txtItemIndex_Leave(object sender, EventArgs e)
        {
            if (txtItemIndex.Text.Trim().Length > 0 && textBox_Image.Text.Length < 1 )
                this.textBox_Image.Text = "QS_Default.dds";
        }

        private void textBox_Script_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void comboBox_Function_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (0 == comboBox_Function.SelectedIndex || 0 < Select_Count)
                return;
            
            if (1 == comboBox_Function.SelectedIndex)
            {
                comboBox_Function.SelectedIndex = 0;
                return;
            }

            button_SelectAdd.Enabled = false;
            button_SelectDel.Enabled = false;
            Set_DelFunctionButton(comboBox_Function.SelectedIndex);

            Add_Select(comboBox_Function.SelectedIndex);
        }

        private void button_DelFunction_Click(object sender, EventArgs e)
        {
            button_SelectAdd.Enabled = true;
            button_SelectDel.Enabled = true;
            button_DelFunction.Hide();
            comboBox_Function.Show();
            comboBox_Function.SelectedIndex = 0;

            Delete_Select();
        }

        private void comboBox_Step_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(0 == comboBox_Step.SelectedIndex)
                return;

            if (1 == comboBox_Step.SelectedIndex)
            {
                comboBox_Step.SelectedIndex = 0;
                return;
            }

            if (2 == comboBox_Step.SelectedIndex)
            {
                if (this.Tag != null)
                    qn.Insert_SubStep(this.Name, this.Tag.ToString());
                else
                    qn.Insert_SubStep(this.Name, "init");

                comboBox_Step.SelectedIndex = 0;
                return;
            }

            if (3 == comboBox_Step.SelectedIndex)
            {
                if (this.Tag != null)
                    qn.Insert_SubStep(this.Name, this.Tag.ToString());
                else
                    qn.Insert_SubStep(this.Name, "init");

                comboBox_Step.SelectedIndex = 0;
                return;
            }
        }

        private void Set_DelFunctionButton(int select_type)
        {
            if (0 == select_type)
            {
                comboBox_Function.Enabled = false;
                button_DelFunction.Hide();
                return;
            }

            comboBox_Function.Hide();
            button_DelFunction.Show();

            switch (select_type)
            {
                case 2: button_DelFunction.Text = "계속 삭제"; break;
                case 3: button_DelFunction.Text = "수락 삭제"; break;
                case 4: button_DelFunction.Text = "완료 삭제"; break;
                case 5: button_DelFunction.Text = "대화종료 삭제"; break;
                case 6: button_DelFunction.Text = "돌아가기 삭제"; break;
            }
        }

        public void SetUIStyle(int styleindex)
        {
            if (0 == styleindex) // basic
            {
                label3.Enabled = false;
                label4.Enabled = false;
                textBox_PortraitLeft.Enabled = false;
                textBox_PortraitRight.Enabled = false;                
                comboBox_Function.Enabled = false;
                button_DelFunction.Enabled = false;
                button_SelectAdd.Enabled = true;
                button_SelectDel.Enabled = true;

                if (1 == m_StyleIndex && 0 < Select_Count)
                {
                    Control control = groupBox_Step.Controls["select_" + (Select_Count - 1)];
                    if (typeof(Quest_SelectControl) == control.GetType())
                    {
                        Quest_SelectControl qs = (Quest_SelectControl)control;
                        if (0 < qs.GetSelectType())
                        {
                            textBox_PortraitLeft.Text = "";
                            textBox_PortraitRight.Text = "";
                            Delete_Select();
                        }
                    }
                }
                m_StyleIndex = styleindex;
                return;
            }

            if (1 == styleindex && 0 == m_StyleIndex) // renewal
            {
                label3.Enabled = true;
                label4.Enabled = true;
                textBox_PortraitLeft.Enabled = true;
                textBox_PortraitRight.Enabled = true;
                comboBox_Function.Enabled = true;
                button_DelFunction.Enabled = true;
                button_DelFunction.Hide();

                if (1 < Select_Count)
                {
                    button_SelectAdd.Enabled = true;
                    button_SelectDel.Enabled = true;
                    button_DelFunction.Hide();
                    comboBox_Function.Show();
                    comboBox_Function.SelectedIndex = 0;
                    comboBox_Function.Enabled = false;
                }

                if (1 == Select_Count)
                {
                    Control control = groupBox_Step.Controls["select_" + (Select_Count - 1)];
                    if (typeof(Quest_SelectControl) == control.GetType())
                    {
                        Quest_SelectControl qs = (Quest_SelectControl)control;
                        Set_DelFunctionButton(qs.GetSelectType());
                    }
                }
                m_StyleIndex = styleindex;
                return;
            }
        }

        private void textBox_PortraitLeft_Click(object sender, EventArgs e)
        {
            frm_Main main = (frm_Main)ParentForm;
            SelectNPC select = new SelectNPC(main.Get_NpcList());
            select.Portrait = true;
            if (select.ShowDialog() == DialogResult.OK)
                textBox_PortraitLeft.Text = select.selected_npcportrait;
        }

        private void textBox_PortraitRight_Click(object sender, EventArgs e)
        {
            frm_Main main = (frm_Main)ParentForm;
            SelectNPC select = new SelectNPC(main.Get_NpcList());
            select.Portrait = true;
            if (select.ShowDialog() == DialogResult.OK)
                textBox_PortraitRight.Text = select.selected_npcportrait;
        }

    }
}
