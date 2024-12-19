using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest
{
    public partial class Quest_Unit : UserControl
    {
        public int Unit_Count = 0;

        private Quest_RemoteComplete m_QuestRemoteComplete = null;

        public Quest_Unit()
        {
            InitializeComponent();
        }

        public void Set_Init_Value(string p_NPCName, string p_ExecuteNo, string p_Script)
        {
            Quest_NPC init_npc = (Quest_NPC)this.Controls["npc_init"];

            init_npc.Set_Value(p_NPCName, p_ExecuteNo);

            Quest_Step init_step = (Quest_Step)init_npc.Controls["step_init"];

            init_step.Set_Vaule(p_Script,"단계시작");
        }

        public void Hide_RemoteComplete()
        {
            this.checkBox_RemoteComplete.Hide();
        }

        public void Enable_RemoteComplete()
        {
            this.checkBox_RemoteComplete.Enabled = true;
        }

        public void Disable_RemoteComplete()
        {
            this.checkBox_RemoteComplete.Enabled = false;
        }

        private void Add_NPC()
        {
            if (null != m_QuestRemoteComplete)
            {
                this.Controls.Remove(this.m_QuestRemoteComplete);
                this.Size = new Size(this.Size.Width, this.Size.Height - 75);
                groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height - 74);
            }

            Quest_NPC qn = new Quest_NPC(this);

            qn.Location = new Point(140, groupBox_Unit.Height + 5);
            qn.Name = "npc_" + Unit_Count;
            qn.Tag = Unit_Count;

            this.Size = new Size(this.Size.Width, this.Size.Height + qn.Height);
            groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height + qn.Height);

            Quest_Step qs = new Quest_Step(qn);

            qs.Location = new Point(193, 3);
            qs.Name = "step_0";
            qs.Tag = 0;
            qs.textBox_StepNo.Text = "단계시작";
            qn.Controls.Add(qs);

            this.Controls.Add(qn);

            qn.textBox_ExecuteNo.Focus();

            Unit_Count++;

            if (null != m_QuestRemoteComplete)
            {
                m_QuestRemoteComplete.Location = new Point(140, groupBox_Unit.Height + 5);
                this.Size = new Size(this.Size.Width, this.Size.Height + 75);
                groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height + 74);

                this.Controls.Add(m_QuestRemoteComplete);
            }
        }

        public void Add_NPC(Quest_NPC p_npc)
        {
            if (this.Controls["npc_init"] != null)
            {
                p_npc.Location = new Point(140, this.Controls[this.Controls.Count - 1].Location.Y + this.Controls[this.Controls.Count - 1].Height);                
                p_npc.Name = "npc_" + Unit_Count;
                p_npc.Tag = Unit_Count;

                Quest_NPC temp_npc = new Quest_NPC(this);
                this.Size = new Size(this.Size.Width, this.Size.Height + temp_npc.Height);
                groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height + p_npc.Height);
                               
                this.Controls.Add(p_npc);

                Unit_Count++;
            }
            else
            {
                p_npc.Location = new Point(140, 0);
                p_npc.Name = "npc_init";
                p_npc.Tag = 0;               

                this.Controls.Add(p_npc);                
            }
        }

        public Quest_NPC[] Get_NPC()
        {
            Quest_NPC[] ret = new Quest_NPC[this.Controls.Count];

            for (int i = 0; i < this.Controls.Count; ++i)
            {
                if (this.Controls[i].Name.IndexOf("groupBox_") == -1)
                {
                    ret[i] = (Quest_NPC)this.Controls[i];
                }
            }

            return ret;
        }
     
        private void button_NPCAdd_Click(object sender, EventArgs e)
        {
            Add_NPC();
        }

        private void button_NPCDel_Click(object sender, EventArgs e)
        {
            if (Unit_Count > 0)
            {
                if (null != m_QuestRemoteComplete)
                {
                    this.Controls.Remove(this.m_QuestRemoteComplete);
                    this.Size = new Size(this.Size.Width, this.Size.Height - 75);
                    groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height - 74);
                }

                this.Size = new Size(this.Size.Width, this.Size.Height - this.Controls["npc_" + (Unit_Count - 1)].Height);
                groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height - this.Controls["npc_" + (Unit_Count - 1)].Height);

                this.Controls["npc_" + (Unit_Count - 1)].Dispose();

                Unit_Count--;

                if (null != m_QuestRemoteComplete)
                {
                    m_QuestRemoteComplete.Location = new Point(140, groupBox_Unit.Height + 5);
                    this.Size = new Size(this.Size.Width, this.Size.Height + 75);
                    groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height + 74);

                    this.Controls.Add(m_QuestRemoteComplete);
                }
            }
        }

        private void Quest_Unit_SizeChanged(object sender, EventArgs e)
        {
            if (Parent != null)
            {
                for (int i = 0; i < Parent.Controls.Count; ++i)
                {
                    if (Parent.Controls[i].Name != this.Name && Parent.Controls[i].Location.Y > this.Location.Y)
                    {
                        Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i - 1].Location.Y + Parent.Controls[i - 1].Height + 3);
                    }
                }
            }
        }

        public bool Add_RemoteComplete(string ExecuteNo)
        {
            frm_Main form = (frm_Main)this.ParentForm;
            int registstep = form.GetQuestRemoteCompleteStep();
            if(0 < registstep)
            {
                return false;
            }

            int step = Convert.ToInt32(this.Tag);
            if (0 == step)
            {
                step = 1;
            }
            form.SetQuestRemoteCompleteStep(step);

            m_QuestRemoteComplete = new Quest_RemoteComplete(this);

            m_QuestRemoteComplete.Location = new Point(140, groupBox_Unit.Height + 5);
            m_QuestRemoteComplete.Name = "npc_9999";
            m_QuestRemoteComplete.SetExecuteNo(ExecuteNo);
            m_QuestRemoteComplete.Tag = 9999;

            this.Size = new Size(this.Size.Width, this.Size.Height + 75);
            groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height + 74);

            this.Controls.Add(m_QuestRemoteComplete);

            m_QuestRemoteComplete.textBox_ExecuteNo.Focus();

            if (CheckState.Checked != checkBox_RemoteComplete.CheckState)
            {
                checkBox_RemoteComplete.CheckState = CheckState.Checked;
            }
            return true;
        }

        private void checkBox_RemoteComplete_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox checkBox = (CheckBox)sender;

            frm_Main form = (frm_Main)this.ParentForm;

            if (CheckState.Checked == checkBox.CheckState && null == m_QuestRemoteComplete)
            {
                bool bSuccess = this.Add_RemoteComplete("");
                if (false == bSuccess)
                {
                    checkBox.CheckState = CheckState.Unchecked;
                }
                return;
            }

            if (CheckState.Unchecked == checkBox.CheckState && null != m_QuestRemoteComplete)
            {
                string strExecuteNo = m_QuestRemoteComplete.GetExecuteNo();
                if ("" != strExecuteNo)
                {
                    DialogResult result = MessageBox.Show("입력하신 데이터가 사라집니다. 정말 지우시겠습니까?", "경고!", MessageBoxButtons.YesNo, MessageBoxIcon.Warning,MessageBoxDefaultButton.Button2);

                    if (DialogResult.No == result)
                    {
                        checkBox.CheckState = CheckState.Checked;
                        return;
                    }
                }

                int mystep = Convert.ToInt32( this.Tag );
                if(0 == mystep)
                {
                    mystep = 1;
                }

                int registstep = form.GetQuestRemoteCompleteStep();
                if (registstep == mystep)
                {
                    form.SetQuestRemoteCompleteStep( 0 );
                }
                m_QuestRemoteComplete.Dispose();
                m_QuestRemoteComplete = null;

                this.Size = new Size(this.Size.Width, this.Size.Height - 75);
                groupBox_Unit.Size = new Size(groupBox_Unit.Size.Width, groupBox_Unit.Size.Height - 74);

                return;
            }

        }
    }
}
