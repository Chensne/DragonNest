using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc
{
    public partial class Quest_Step : UserControl
    {
        public int Select_Count = 0;       

        public Quest_Step()
        {
            InitializeComponent();            
        }

        public Quest_Step(string p_script, string p_stepno)
        {
            InitializeComponent();
            
            textBox_Script.Text = p_script;
            textBox_StepNo.Text = p_stepno;            
        }

        public void Set_Vaule(string p_script, string p_stepno)
        {
            textBox_Script.Text = p_script;
            textBox_StepNo.Text = p_stepno;
        }

        private void Add_Select()
        {
            Quest_SelectControl qs = new Quest_SelectControl();

            qs.Location = new Point(110, 160 + (68 * Select_Count));
            qs.Name = "select_" + Select_Count;
            qs.Tag = this.Tag;

            this.Size = new Size(this.Size.Width, this.Size.Height + 68);
            groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height + 68);

            Parent.Height = Parent.Height + 68;

            for (int i = 0; i < Parent.Controls.Count; ++i)
            {
                if (Parent.Controls[i].Name.IndexOf("script_init") < 0)
                {
                    int controlNo = Convert.ToInt16(Parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("init") < 0)
                    {
                        thisNo = Convert.ToInt16(this.Tag);
                    }

                    if (controlNo > thisNo)
                    {
                        Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i].Location.Y + 68);
                    }
                }
            }

            groupBox_Step.Controls.Add(qs);

            Select_Count++; 
        }

        public void Add_Select(Quest_SelectControl p_select)
        {
            p_select.Location = new Point(110, 160 + (68 * Select_Count));
            p_select.Name = "select_" + Select_Count;
            p_select.Tag = this.Tag;

            this.Size = new Size(this.Size.Width, this.Size.Height + 68);
            groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height + 68);

            Parent.Height = Parent.Height + 68;

            for (int i = 0; i < Parent.Controls.Count; ++i)
            {
                if (Parent.Controls[i].Name.IndexOf("script_init") < 0)
                {
                    int controlNo = Convert.ToInt16(Parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("init") < 0)
                    {
                        thisNo = Convert.ToInt16(this.Tag);
                    }

                    if (controlNo > thisNo)
                    {
                        Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i].Location.Y + 68);
                    }
                }
            }

            groupBox_Step.Controls.Add(p_select);

            Select_Count++; 
        }

        public Control[] Get_Select()
        {
            Control[] ret = new Control[this.Controls["groupBox_Step"].Controls.Count];

            for (int i = 0; i < this.Controls["groupBox_Step"].Controls.Count; ++i)
            {
                if (this.Controls["groupBox_Step"].Controls[i].Name.IndexOf("select_") > -1)
                {
                    ret[i] = this.Controls["groupBox_Step"].Controls[i];
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

        public string Get_ItemIndex()
        {
            return this.txtItemIndex.Text;
        }

        private void button_SelectAdd_Click(object sender, EventArgs e)
        {
            Add_Select();             
        }

        private void button_SelectDel_Click(object sender, EventArgs e)
        {
            if (Select_Count > 0)
            {
                this.Size = new Size(this.Size.Width, this.Size.Height - 68);
                groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height - 68);

                Parent.Height = Parent.Height - 68;

                for (int i = 0; i < Parent.Controls.Count; ++i)
                {
                    if (Parent.Controls[i].Name.IndexOf("script_init") < 0)
                    {
                        int controlNo = Convert.ToInt16(Parent.Controls[i].Tag);
                        int thisNo = -1;

                        if (this.Name.IndexOf("init") < 0)
                        {
                            thisNo = Convert.ToInt16(this.Tag);
                        }

                        if (controlNo > thisNo)
                        {
                            Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i].Location.Y - 68);
                        }
                    }
                }

                groupBox_Step.Controls["select_" + (Select_Count-1)].Dispose();

                Select_Count--;
            }
        }

        private void button_QuestAdd_Click(object sender, EventArgs e)
        {
            Add_Xml();    
        }

        private void Add_Xml()
        {
            Quest_Link qs = new Quest_Link();

            qs.Location = new Point(110, 160 + (68 * Select_Count));
            qs.Name = "select_" + Select_Count;
            qs.Tag = Select_Count;

            this.Size = new Size(this.Size.Width, this.Size.Height + 68);
            groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height + 68);

            Parent.Height = Parent.Height + 68;

            for (int i = 0; i < Parent.Controls.Count; ++i)
            {
                if (Parent.Controls[i].Name.IndexOf("script_init") < 0)
                {
                    int controlNo = Convert.ToInt16(Parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("init") < 0)
                    {
                        thisNo = Convert.ToInt16(this.Tag);
                    }

                    if (controlNo > thisNo)
                    {
                        Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i].Location.Y + 68);
                    }
                }
            }

            groupBox_Step.Controls.Add(qs);

            Select_Count++;
        }

        public void Add_Xml(Quest_Link p_link)
        {
            p_link.Location = new Point(110, 160 + (68 * Select_Count));
            p_link.Name = "select_" + Select_Count;
            p_link.Tag = Select_Count;

            this.Size = new Size(this.Size.Width, this.Size.Height + 68);
            groupBox_Step.Size = new Size(groupBox_Step.Size.Width, groupBox_Step.Size.Height + 68);

            Parent.Height = Parent.Height + 68;

            for (int i = 0; i < Parent.Controls.Count; ++i)
            {
                if (Parent.Controls[i].Name.IndexOf("script_init") < 0)
                {
                    int controlNo = Convert.ToInt16(Parent.Controls[i].Tag);
                    int thisNo = -1;

                    if (this.Name.IndexOf("init") < 0)
                    {
                        thisNo = Convert.ToInt16(this.Tag);
                    }

                    if (controlNo > thisNo)
                    {
                        Parent.Controls[i].Location = new Point(Parent.Controls[i].Location.X, Parent.Controls[i].Location.Y + 68);
                    }
                }
            }

            groupBox_Step.Controls.Add(p_link);

            Select_Count++;
        }

        //꺽쇠 체크
        private void Check_GT(TextBox p_txt, KeyEventArgs e)
        {
            if (e.Modifiers == Keys.Shift)
            {
                if (e.KeyCode == Keys.Oemcomma || e.KeyCode == Keys.OemPeriod)
                {
                    MessageBox.Show("< 혹은 > 는 사용할 수 없습니다!", "오류", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    p_txt.Text = p_txt.Text.Substring(0, p_txt.Text.Length - 1);
                    p_txt.SelectionStart = p_txt.Text.Length;
                }
            }
        }

        private void textBox_Image_KeyDown(object sender, KeyEventArgs e)
        {
            Check_GT((TextBox)sender, e);
        }
    }
}
