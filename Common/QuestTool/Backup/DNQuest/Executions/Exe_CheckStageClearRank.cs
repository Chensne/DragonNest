using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_CheckStageClearRank : Base_UserControl
    {
        public string getStageIndex()
        {
            return txtStageIndex.Text;
        }

        public string getRank()
        {
            return txtRank.Text;
        }

        public string getInequality()
        {
            return txtInequality.Text;
        }

        public string getExecute()
        {
            return txtExecute.Text;
        }

        public void setValue(string stageIndex, string rank, string inequality, string execute )
        {
            txtStageIndex.Text = stageIndex;
            txtRank.Text = rank;
            txtInequality.Text = inequality;
            txtExecute.Text = execute;
        }

        public Exe_CheckStageClearRank() : base()
        {
            InitializeComponent();

            this.txtStageIndex.Validating += new System.ComponentModel.CancelEventHandler(Validate);
            this.txtRank.Validating += new System.ComponentModel.CancelEventHandler(Validate);
            this.txtInequality.Validating += new System.ComponentModel.CancelEventHandler(Validate);
            this.txtExecute.Validating += new System.ComponentModel.CancelEventHandler(Validate);
        }
    }
}
