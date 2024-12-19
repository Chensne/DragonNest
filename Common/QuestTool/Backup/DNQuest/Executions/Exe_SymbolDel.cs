using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_SymbolDel : UserControl
    {
        public Exe_SymbolDel()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_item, string p_count)
        {
            textBox_Item.Text = p_item;
            textBox_Count.Text = p_count;
        }
    }
}
