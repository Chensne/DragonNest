using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace DNQuest
{
    /// <summary>
    /// Validate interface
    /// </summary>
    public interface Validatable
    {
        void Validate();
    }

    /// <summary>
    /// UserControl에 ErrorProvider를 사용하기 위한 sub Class 구현
    /// </summary>
    public class Base_UserControl : UserControl, Validatable
    {
        //ErrorProvider
        public ErrorProvider errPvdr;

        public Base_UserControl()
        {
            errPvdr = new ErrorProvider();
        }

        /// <summary>
        /// Interface 구현
        /// </summary>
        void Validatable.Validate()
        {
            foreach (Control control in this.Controls)
            {
                if (this.errPvdr.GetError(control).Length != 0)
                {
                    break;
                }
            }
        }

        public void Validate(object sender, CancelEventArgs e)
        {
            Control ctrl = (Control)sender;

            if (ctrl.Text.Trim().Length < 1)
            {
                errPvdr.SetError(ctrl, "입력이 필요합니다.");
                return;
            }
        }
    }
}
