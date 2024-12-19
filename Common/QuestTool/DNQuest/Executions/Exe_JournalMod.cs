using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_JournalMod : Base_UserControl
    {
        public Exe_JournalMod() : base()
        {
            InitializeComponent();

            this.textBox_Journal.Validating +=new CancelEventHandler(Validate);
        }

        public void Set_Journal(string p_journal)
        {
            textBox_Journal.Text = p_journal;
        }
    }
}
