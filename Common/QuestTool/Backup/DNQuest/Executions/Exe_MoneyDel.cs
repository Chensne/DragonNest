using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_MoneyDel : Base_UserControl
    {
        public Exe_MoneyDel() : base()
        {
            InitializeComponent();

            this.txtDeleteMoney.Validating += new CancelEventHandler(Validate);
            this.txtMoreExecute.Validating += new CancelEventHandler(Validate);
            this.txtLessExecute.Validating += new CancelEventHandler(Validate);
        }

        internal void Set_Value(string money, string moreExecute, string lessExecute)
        {
            this.txtDeleteMoney.Text = money;
            this.txtMoreExecute.Text = moreExecute;
            this.txtLessExecute.Text = lessExecute;
        }
    }
}
