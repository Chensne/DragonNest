using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_ItemDel : Base_UserControl
    {
        public Exe_ItemDel() : base()
        {
            InitializeComponent();

            this.textBox_Item.Validating += new System.ComponentModel.CancelEventHandler(this.Validate);
            this.textBox_Count.Validating += new System.ComponentModel.CancelEventHandler(this.Validate);
        }

        public void Set_Value(string p_item, string p_count)
        {
            textBox_Item.Text = p_item;
            textBox_Count.Text = p_count;
        }
    }
}
