using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Conditions
{
    public partial class Condition_GeneralItem : Base_UserControl
    {
        public Condition_GeneralItem() : base()
        {
            InitializeComponent();

            this.textBox_Item.Validating +=new CancelEventHandler(Validate);
            this.textBox_Oper.Validating += new CancelEventHandler(Validate);
            this.textBox_Value.Validating += new CancelEventHandler(Validate);
        }

        public void Set_Value(string p_item, string p_oper, string p_value)
        {
            textBox_Item.Text = p_item;
            textBox_Oper.Text = p_oper;
            textBox_Value.Text = p_value;
        }
    }
}
