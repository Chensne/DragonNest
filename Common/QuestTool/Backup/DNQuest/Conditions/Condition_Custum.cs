using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Conditions
{
    public partial class Condition_Custum : Base_UserControl
    {
        public Condition_Custum() : base()
        {
            InitializeComponent();
            this.textBox_Code.Validating += new CancelEventHandler(Validate);
        }
    }
}
