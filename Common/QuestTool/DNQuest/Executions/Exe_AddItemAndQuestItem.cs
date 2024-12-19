using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace DNQuest.Executions
{
    public partial class Exe_AddItemAndQuestItem : Base_UserControl
    {
        public Exe_AddItemAndQuestItem() : base()
        {
            InitializeComponent();

            this.txt_Item.Validating += new System.ComponentModel.CancelEventHandler(this.Validate);
            this.txt_ItemCount.Validating += new System.ComponentModel.CancelEventHandler(this.Validate);
            this.txt_QuestItem.Validating += new System.ComponentModel.CancelEventHandler(this.Validate);
            this.txt_QuestItemCount.Validating += new System.ComponentModel.CancelEventHandler(this.Validate);
        }

        public void Set_Value(string item, string itemCount, string questItem, string questItemCount)
        {
            txt_Item.Text = item;
            txt_ItemCount.Text = itemCount;
            txt_QuestItem.Text = questItem;
            txt_QuestItemCount.Text = questItemCount;
        }
    }
}
