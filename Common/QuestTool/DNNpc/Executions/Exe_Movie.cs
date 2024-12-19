using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNNpc.Executions
{
    public partial class Exe_Movie : UserControl
    {
        public Exe_Movie()
        {
            InitializeComponent();
        }

        public void Set_Value(string p_movie)
        {
            textBox_Movie.Text = p_movie;
        }
    }
}
